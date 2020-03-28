package main

import (
	"log"
	"os"
	"os/signal"

	"github.com/kapitanov/led-clock/server/module"
)

func main() {
	log.SetFlags(log.LstdFlags | log.Lmsgprefix)

	server := createServer()
	defer server.Close()

	module.Initialize(server)

	err := server.Init()
	if err != nil {
		panic(err)
	}

	interrupt := make(chan os.Signal)
	done := make(chan interface{})

	signal.Notify(interrupt, os.Interrupt)
	go func() {
		<-interrupt

		log.Printf("SIGINT received, shutting down")
		server.Stop()

		done <- nil
	}()

	err = server.Start()
	if err != nil {
		panic(err)
	}

	<-done
	log.Printf("Good bye")
}

func createServer() Server {
	httpServer := NewHttpServer()
	mqttServer := NewMqttServer()

	return NewServer(httpServer, mqttServer)
}
