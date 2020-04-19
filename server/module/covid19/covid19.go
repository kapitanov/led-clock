package covid19

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"sync"
	"time"

	"github.com/kapitanov/led-clock/server/module"
)

const (
	name          = "covid19"
	topicRequest  = "request"
	topicResponse = "update"
	topicPush     = "update"

	defaultCountry = "russia"
	sleepDuration  = time.Minute * 60 * 8
)

func init() {
	mod := new(moduleImpl)
	module.Register(mod)
}

type stats struct {
	Country   string `json:"country"`
	Deaths    int    `json:"deaths"`
	Recovered int    `json:"recovered"`
	Active    int    `json:"active"`
}

type webServiceOutput struct {
	Country     string `json:"country"`
	TodayCases  int    `json:"todayCases"`
	TodayDeaths int    `json:"todayDeaths"`
	Deaths      int    `json:"deaths"`
	Recovered   int    `json:"recovered"`
	Active      int    `json:"active"`
	Critical    int    `json:"critical"`
}

type moduleImpl struct {
	publisher module.Publisher
	log       *log.Logger
	country   string
	current   *stats
	lock      sync.Mutex
}

func (m *moduleImpl) Name() string {
	return name
}

func (m *moduleImpl) Init(context module.InitContext) error {
	m.publisher = context.Publisher()
	m.log = context.Log()

	m.country = os.Getenv("COVID19_COUNTRY")
	if m.country == "" {
		m.country = defaultCountry
	}

	m.log.Printf("using country %s\n", m.country)

	_, err := m.Update(true)
	if err != nil {
		m.log.Printf("Failed to fetch stats for %s: %s", m.country, err)
		return err
	}

	err = context.AddHandler(topicRequest, topicResponse, m.HandleRequest)
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
				m.log.Printf("Failed to fetch stats for %s: %s", m.country, err)
			} else if updated {
				m.Publish()
			}
		}
	}()
}

func (m *moduleImpl) HandleRequest(r io.Reader) (interface{}, error) {
	value := m.GetValue()
	return value, nil
}

func (m *moduleImpl) Update(force bool) (bool, error) {
	s, err := m.Query()
	if err != nil {
		return false, err
	}

	m.lock.Lock()
	defer m.lock.Unlock()

	if m.current == nil ||
		m.current.Deaths != s.Deaths ||
		m.current.Recovered != s.Recovered ||
		m.current.Active != s.Active {
		m.current = s

		if force {
			m.log.Printf("now %d active/%d recovered/%d dead", s.Active, s.Recovered, s.Deaths)
		}
		return true, nil
	}

	return false, nil
}

func (m *moduleImpl) Query() (*stats, error) {
	uri := fmt.Sprintf("https://corona.lmao.ninja/v2/countries/%s", m.country)
	resp, err := http.Get(uri)
	if err != nil {
		return nil, err
	}

	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}

	var output webServiceOutput
	err = json.Unmarshal(body, &output)
	if err != nil {
		return nil, err
	}

	s := &stats{
		Country:   output.Country,
		Active:    output.Active,
		Recovered: output.Recovered,
		Deaths:    output.Deaths,
	}

	return s, nil
}

func (m *moduleImpl) Publish() {
	value := m.GetValue()
	m.publisher.Publish(topicPush, value)
}

func (m *moduleImpl) GetValue() *stats {
	m.lock.Lock()
	defer m.lock.Unlock()

	return m.current
}
