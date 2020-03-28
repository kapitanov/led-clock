package main

import (
	"errors"
	"fmt"

	"github.com/kapitanov/led-clock/server/module"
)

var (
	ErrNotConfigured = errors.New("Server is not configured")
)

type Server interface {
	module.Publisher
	Init() error
	Start() error
	Stop()
	Close()
}

func NewServer(servers ...Server) Server {
	return &compositeServerImpl{servers}
}

type compositeServerImpl struct {
	servers []Server
}

func (s *compositeServerImpl) Init() error {
	servers := make([]Server, 0)
	for _, server := range s.servers {
		err := server.Init()
		if err == nil {
			servers = append(servers, server)
		}
	}

	if len(servers) == 0 {
		return fmt.Errorf("No servers are configured")
	}

	s.servers = servers
	return nil
}

func (s *compositeServerImpl) Publish(topic string, message interface{}) {
	for _, server := range s.servers {
		server.Publish(topic, message)
	}
}

func (s *compositeServerImpl) Start() error {
	for _, server := range s.servers {
		err := server.Start()
		if err != nil {
			return err
		}
	}

	return nil
}

func (s *compositeServerImpl) Stop() {
	for _, server := range s.servers {
		server.Stop()
	}
}

func (s *compositeServerImpl) Close() {
	for _, server := range s.servers {
		server.Close()
	}
}
