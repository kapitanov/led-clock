package weather

import (
	"encoding/json"
	"io"
	"log"
	"os"
	"os/exec"
	"sync"
	"time"

	"github.com/kapitanov/led-clock/server/module"
)

const (
	name          = "weather"
	topicRequest  = "request"
	topicResponse = "update"
	topicPush     = "update"

	defaultCity   = "moscow"
	sleepDuration = time.Minute * 60
)

func init() {
	mod := new(moduleImpl)
	module.Register(mod)
}

type weather struct {
	Now  float32 `json:"now"`
	City string  `json:"city"`
}

type yandexWeatherCliOutput struct {
	City string  `json:"city"`
	Now  float32 `json:"term_now"`
}

type moduleImpl struct {
	publisher      module.Publisher
	log            *log.Logger
	city           string
	currentWeather *weather
	lock           sync.Mutex
}

func (m *moduleImpl) Name() string {
	return name
}

func (m *moduleImpl) Init(context module.InitContext) error {
	m.publisher = context.Publisher()
	m.log = context.Log()

	m.city = os.Getenv("WEATHER_CITY")
	if m.city == "" {
		m.city = defaultCity
	}

	m.log.Printf("using city %s\n", m.city)

	_, err := m.Update(true)
	if err != nil {
		m.log.Printf("Failed to fetch weather in %s: %s", m.city, err)
		return err
	}

	err = context.AddHandler(topicRequest, topicResponse, m.HandleWeatherRequest)
	if err != nil {
		return err
	}

	return nil
}

func (m *moduleImpl) Start() {
	m.Update(true)

	go func() {
		for {
			time.Sleep(time.Second * 10)
			updated, err := m.Update(false)
			if err != nil {
				m.log.Printf("Failed to fetch weather in %s: %s", m.city, err)
			} else if updated {
				m.Publish()
			}
		}
	}()
}

func (m *moduleImpl) HandleWeatherRequest(r io.Reader) (interface{}, error) {
	value := m.GetValue()
	return value, nil
}

func (m *moduleImpl) Update(force bool) (bool, error) {
	w, err := m.Query()
	if err != nil {
		return false, err
	}

	m.lock.Lock()
	defer m.lock.Unlock()

	if m.currentWeather == nil || m.currentWeather.Now != w.Now {
		m.currentWeather = w

		if force {
			m.log.Printf("now %02f\n", w.Now)
		}
		return true, nil
	}

	return false, nil
}

func (m *moduleImpl) Query() (*weather, error) {
	cmd := exec.Command("yandex-weather-cli", "--json", m.city, "--days", "0")
	stdout, err := cmd.Output()
	if err != nil {
		return nil, err
	}

	var output yandexWeatherCliOutput
	err = json.Unmarshal(stdout, &output)
	if err != nil {
		return nil, err
	}

	w := &weather{
		Now:  output.Now,
		City: m.city,
	}

	return w, nil
}

func (m *moduleImpl) Publish() {
	value := m.GetValue()
	m.publisher.Publish(topicPush, value)
}

func (m *moduleImpl) GetValue() *weather {
	m.lock.Lock()
	defer m.lock.Unlock()

	return m.currentWeather
}
