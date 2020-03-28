package module

import (
	"errors"
	"io"
	"log"
)

var (
	ErrDuplicateModule   = errors.New("Module is already defined")
	ErrDuplicateEndpoint = errors.New("Endpoint is already defined")
)

type Interface interface {
	Name() string
	Init(context InitContext) error
	Start()
}

type HandlerFunc func(r io.Reader) (interface{}, error)

type InitContext interface {
	Publisher() Publisher
	Log() *log.Logger
	AddHandler(requestTopic string, responseTopic string, handler HandlerFunc) error
}

type Publisher interface {
	Publish(topic string, message interface{})
}

func Register(module Interface) {
	modules = append(modules, module)
}

func Initialize(pub Publisher) {
	publisher = pub
	for _, module := range modules {
		initializeModule(module)
	}
}

type ModuleDefinition struct {
	Name      string                `json:"name"`
	Endpoints []*EndpointDefinition `json:"endpoints"`
	Module    Interface             `json:"-"`
}

type EndpointDefinition struct {
	RequestTopic  string            `json:"request"`
	ResponseTopic string            `json:"response"`
	HasParameters bool              `json:"hasParametes"`
	Module        *ModuleDefinition `json:"-"`
	Handler       HandlerFunc       `json:"-"`
}

func ListModules() []*ModuleDefinition {
	list := make([]*ModuleDefinition, len(moduleDefs))
	i := 0
	for _, module := range moduleDefs {
		list[i] = module
		i++
	}
	return list
}

func ListEndpoints() []*EndpointDefinition {
	list := make([]*EndpointDefinition, len(endpointDefs))
	i := 0
	for _, endpoint := range endpointDefs {
		list[i] = endpoint
		i++
	}
	return list
}

func SelectEndpoint(topic string) *EndpointDefinition {
	endpoint, _ := endpointDefs[topic]
	return endpoint
}
