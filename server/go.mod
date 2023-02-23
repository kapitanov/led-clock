module github.com/kapitanov/led-clock/server

go 1.14

replace github.com/kapitanov/led-clock/server/module => ./module

require (
	github.com/eclipse/paho.mqtt.golang v1.2.0
	github.com/kapitanov/led-clock/server/module v0.0.0-00010101000000-000000000000
	golang.org/x/net v0.0.0-20220722155237-a158d28d115b // indirect
)
