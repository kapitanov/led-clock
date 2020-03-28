package time

import (
	"io"
	"log"
	"os"
	"sync"
	systime "time"

	"github.com/kapitanov/led-clock/server/module"
)

const (
	name          = "time"
	topicRequest  = "request"
	topicResponse = "update"
	topicPush     = "update"
)

func init() {
	mod := new(moduleImpl)
	module.Register(mod)
}

type timeInfo struct {
	H int `json:"h"`
	M int `json:"m"`
	S int `json:"s"`
}

type moduleImpl struct {
	publisher module.Publisher
	log       *log.Logger
	timezone  *systime.Location
	timeValue *timeInfo
	timeLock  sync.Mutex
}

func (m *moduleImpl) Name() string {
	return name
}

func (m *moduleImpl) Init(context module.InitContext) error {
	m.publisher = context.Publisher()
	m.log = context.Log()

	tz := os.Getenv("TIMEZONE")
	if tz == "" {
		tz = "UTC"
	}

	var err error
	m.timezone, err = systime.LoadLocation(tz)
	if err != nil {
		return err
	}
	m.log.Printf("using timezone %s\n", m.timezone)

	err = context.AddHandler(topicRequest, topicResponse, m.HandleTimeRequest)
	if err != nil {
		return err
	}

	m.Update(true)
	return nil
}

func (m *moduleImpl) Start() {
	m.Update(true)

	go func() {
		for {
			systime.Sleep(systime.Second * 10)
			if m.Update(false) {
				m.Publish()
			}
		}
	}()
}

func (m *moduleImpl) HandleTimeRequest(r io.Reader) (interface{}, error) {
	value := m.GetValue()
	return value, nil
}

func (m *moduleImpl) Update(force bool) bool {
	t := systime.Now().In(m.timezone)

	m.timeLock.Lock()
	defer m.timeLock.Unlock()
	val := &timeInfo{H: t.Hour(), M: t.Minute(), S: t.Second()}

	if m.timeValue == nil || m.timeValue.H != val.H || m.timeValue.M != val.M || m.timeValue.S != val.S {
		m.timeValue = val

		if force {
			m.log.Printf("now %2d:%02d:%02d\n", val.H, val.M, val.S)
		}
		return true
	}

	return false
}

func (m *moduleImpl) Publish() {
	value := m.GetValue()
	m.publisher.Publish(topicPush, value)
}

func (m *moduleImpl) GetValue() *timeInfo {
	m.timeLock.Lock()
	defer m.timeLock.Unlock()

	return m.timeValue
}
