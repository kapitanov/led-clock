package main

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"strings"
	"time"

	"github.com/kapitanov/led-clock/server/module"
)

func NewHttpServer() Server {
	s := &httpServerImpl{
		log:    log.New(log.Writer(), "http: ", log.Flags()),
		router: http.NewServeMux(),
		server: &http.Server{
			ReadTimeout:  5 * time.Second,
			WriteTimeout: 10 * time.Second,
			IdleTimeout:  15 * time.Second,
		},
	}

	s.server.Handler = s.router

	return s
}

type httpServerImpl struct {
	log    *log.Logger
	server *http.Server
	router *http.ServeMux
}

type jsonError struct {
	Error string `json:"error"`
}

type jsonEndpoint struct {
	Module        string `json:"module"`
	Method        string `json:"method"`
	RequestTopic  string `json:"request"`
	ResponseTopic string `json:"response"`
}

func (s *httpServerImpl) Init() error {
	endpoint := os.Getenv("HTTP_ENDPOINT")
	if endpoint == "" {
		endpoint = "0.0.0.0:3000"
	}

	s.server.Addr = endpoint

	s.router.Handle("/", http.FileServer(http.Dir("./www")))
	s.router.HandleFunc("/api", s.handleEndpointList)
	s.router.HandleFunc("/api/", s.handleRequest)
	return nil
}

func (s *httpServerImpl) Publish(topic string, message interface{}) {}

func (s *httpServerImpl) Start() error {
	go func() {
		s.log.Printf("listening on \"%s\"\n", s.server.Addr)

		if err := s.server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			s.log.Fatalf("Could not listen on %s: %v\n", s.server.Addr, err)
		}
	}()

	return nil
}

func (s *httpServerImpl) Stop() {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	s.server.SetKeepAlivesEnabled(false)
	if err := s.server.Shutdown(ctx); err != nil {
		s.log.Fatalf("Could not gracefully shutdown the server: %v\n", err)
	}
}

func (s *httpServerImpl) Close() {}

func (s *httpServerImpl) handleEndpointList(w http.ResponseWriter, request *http.Request) {
	defer s.recoveryFunc(w, request)

	endpoints := module.ListEndpoints()

	response := make(map[string]*jsonEndpoint)
	for _, endpoint := range endpoints {
		path := fmt.Sprintf("/api%s", endpoint.RequestTopic)
		e := &jsonEndpoint{
			Module:        endpoint.Module.Name,
			RequestTopic:  endpoint.RequestTopic,
			ResponseTopic: endpoint.ResponseTopic,
			Method:        "GET",
		}

		if endpoint.HasParameters {
			e.Method = "POST"
		}

		response[path] = e
	}

	s.writeResponse(w, 200, response)
}

func (s *httpServerImpl) handleRequest(w http.ResponseWriter, request *http.Request) {
	defer s.recoveryFunc(w, request)

	statusCode, body := s.handleRequestCore(request)
	s.writeResponse(w, statusCode, body)
}

func (s *httpServerImpl) recoveryFunc(w http.ResponseWriter, request *http.Request) {
	if e := recover(); e != nil {
		msg := fmt.Sprintf("Unable to process request \"%s %s\": %s", request.Method, request.URL.Path, e)
		s.log.Printf(msg)
		s.writeResponse(w, 500, &jsonError{msg})
	}
}

func (s *httpServerImpl) writeResponse(w http.ResponseWriter, statusCode int, jsonBody interface{}) {
	if jsonBody != nil {
		w.Header().Set("Content-Type", "application/json")
	}

	w.WriteHeader(statusCode)
	if jsonBody != nil {
		bytes, err := json.Marshal(jsonBody)
		if err != nil {
			s.log.Printf("Failed to serialize HTTP response: %s", err)
			return
		}

		_, err = w.Write(bytes)
		if err != nil {
			s.log.Printf("Failed to write HTTP response: %s", err)
		}
	}
}

func (s *httpServerImpl) handleRequestCore(request *http.Request) (int, interface{}) {
	topic := request.URL.Path
	topic = strings.TrimPrefix(topic, "/api")

	endpoint := module.SelectEndpoint(topic)
	if endpoint == nil {
		msg := fmt.Sprintf("Endpoint \"%s\" is not supported", topic)
		return 404, &jsonError{msg}
	}

	if endpoint.HasParameters && request.Method != "POST" {
		msg := fmt.Sprintf("Endpoint \"%s\" requires parameters but none were supplied", topic)
		s.log.Printf(msg)
		return 405, &jsonError{msg}
	}

	if !endpoint.HasParameters && request.Method != "GET" {
		msg := fmt.Sprintf("Endpoint \"%s\" requires no parameters but some were supplied", topic)
		s.log.Printf(msg)
		return 405, &jsonError{msg}
	}

	response, err := endpoint.Handler(request.Body)
	if err != nil {
		msg := fmt.Sprintf("Endpoint \"%s\" failed to process request: %s", topic, err)
		s.log.Printf(msg)
		return 500, &jsonError{msg}
	}

	return 200, response
}
