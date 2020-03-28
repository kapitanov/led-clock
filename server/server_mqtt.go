package main

import (
	"bytes"
	"encoding/json"
	"log"
	"net/url"
	"os"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/kapitanov/led-clock/server/module"
)

const (
	clientID = "miot-led-service"
	mqttPort = 1883
)

func NewMqttServer() Server {
	s := &mqttServerImpl{
		log:     log.New(log.Writer(), "mqtt: ", log.Flags()),
		options: mqtt.NewClientOptions(),
	}

	return s
}

type mqttServerImpl struct {
	endpoint string
	log      *log.Logger
	options  *mqtt.ClientOptions
	client   mqtt.Client
}

func (s *mqttServerImpl) Init() error {
	endpoint := os.Getenv("MQTT_ENDPOINT")
	if endpoint == "" {
		s.log.Printf("No MQTT connection is configured")
		return ErrNotConfigured
	}

	serverUrl, err := url.Parse(endpoint)
	if err != nil {
		s.log.Printf("ERROR! Bad MQTT_EDNPOINT value")
		return err
	}

	s.options.Servers = []*url.URL{serverUrl}
	s.options.ClientID = clientID
	s.options.Username = os.Getenv("MQTT_USERNAME")
	s.options.Password = os.Getenv("MQTT_PASSWORD")
	s.options.OnConnect = s.onConnected
	s.client = mqtt.NewClient(s.options)

	return nil
}

func (s *mqttServerImpl) Publish(topic string, message interface{}) {
	bytes, err := json.Marshal(message)
	if err == nil {
		token := s.client.Publish(topic, 0, false, bytes)
		token.Wait()
		err = token.Error()
	}

	if err != nil {
		s.log.Printf("failed to publish messag to \"%s\". %s\n", topic, err)
	}
}

func (s *mqttServerImpl) Start() error {
	s.log.Printf("connecting to %s\n", s.options.Servers[0])
	token := s.client.Connect()
	if token.Wait() && token.Error() != nil {
		err := token.Error()
		s.log.Printf("failed to connect. %s\n", err)
		return err
	}

	for _, mod := range module.ListModules() {
		mod.Module.Start()
	}

	return nil
}

func (s *mqttServerImpl) Stop() {
	if s.client != nil {
		s.client.Disconnect(1000)
		s.client = nil
	}
}

func (s *mqttServerImpl) Close() {

}

func (s *mqttServerImpl) onConnected(_ mqtt.Client) {
	for _, endpoint := range module.ListEndpoints() {
		s.client.Subscribe(endpoint.RequestTopic, 0, func(_ mqtt.Client, msg mqtt.Message) {
			go func() {
				s.handleRequest(msg)
			}()
		})
	}
}

func (s *mqttServerImpl) handleRequest(msg mqtt.Message) {
	defer func() {
		if e := recover(); e != nil {
			s.log.Printf("Unable to process request %d \"%s\"", msg.MessageID(), msg.Topic())
			return
		}
	}()

	topic := msg.Topic()

	endpoint := module.SelectEndpoint(topic)
	if endpoint == nil {
		s.log.Printf("Endpoint \"%s\" is not supported", topic)
		return
	}

	response, err := endpoint.Handler(bytes.NewBuffer(msg.Payload()))
	if err != nil {
		s.log.Printf("Endpoint \"%s\" failed to process request: %s", topic, err)
		return
	}

	s.Publish(endpoint.ResponseTopic, response)
}
