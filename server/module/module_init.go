package module

import (
	"fmt"
	"log"
)

func initializeModule(module Interface) {
	loggerName := fmt.Sprintf("%s: ", module.Name())
	logger := log.New(log.Writer(), loggerName, log.Flags())

	context, err := createInitContextImpl(module, logger)
	if err != nil {
		logger.Printf("Failed to initialize module \"%s\": %s", module.Name(), err)
		return
	}

	err = module.Init(context)
	if err != nil {
		logger.Printf("Failed to initialize module \"%s\": %s", module.Name(), err)
		return
	}

	context.Register()
	logger.Printf("module is initialized")
}

var (
	publisher Publisher
	modules   []Interface = make([]Interface, 0)

	moduleDefs   map[string]*ModuleDefinition   = make(map[string]*ModuleDefinition)
	endpointDefs map[string]*EndpointDefinition = make(map[string]*EndpointDefinition)
)

type initContextImpl struct {
	module    *ModuleDefinition
	log       *log.Logger
	endpoints map[string]*EndpointDefinition
}

func createInitContextImpl(module Interface, logger *log.Logger) (*initContextImpl, error) {
	if _, exists := moduleDefs[module.Name()]; exists {
		log.Printf("ERROR! Module \"%s\" is already defined", module.Name())
		return nil, ErrDuplicateModule
	}

	moduleDef := ModuleDefinition{
		Name:      module.Name(),
		Module:    module,
		Endpoints: make([]*EndpointDefinition, 0),
	}

	context := &initContextImpl{
		module:    &moduleDef,
		log:       logger,
		endpoints: make(map[string]*EndpointDefinition),
	}
	return context, nil
}

func (c *initContextImpl) Publisher() Publisher {
	return publisher
}

func (c *initContextImpl) Log() *log.Logger {
	return c.log
}

func (c *initContextImpl) AddHandler(requestTopic string, responseTopic string, handler HandlerFunc) error {
	requestTopic = fmt.Sprintf("/%s/%s", c.module.Name, requestTopic)
	responseTopic = fmt.Sprintf("/%s/%s", c.module.Name, responseTopic)

	if _, exists := c.endpoints[requestTopic]; exists {
		c.log.Printf("ERROR! Endpoint \"%s\" is already defined by module \"%s\"", requestTopic, c.module.Name)
		return ErrDuplicateEndpoint
	}

	if duplicate, exists := endpointDefs[requestTopic]; exists {
		c.log.Printf("ERROR! Endpoint \"%s\" is already defined by module \"%s\"", requestTopic, duplicate.Module)
		return ErrDuplicateEndpoint
	}

	endpoint := &EndpointDefinition{
		RequestTopic:  requestTopic,
		ResponseTopic: responseTopic,
		Module:        c.module,
		Handler:       handler,
	}

	c.endpoints[requestTopic] = endpoint
	c.log.Printf("registered endpoint \"%s\" -> \"%s\"", requestTopic, responseTopic)
	return nil
}

func (c *initContextImpl) Register() {
	moduleDefs[c.module.Name] = c.module
	for topic, endpoint := range c.endpoints {
		c.module.Endpoints = append(c.module.Endpoints, endpoint)
		endpointDefs[topic] = endpoint
	}
}
