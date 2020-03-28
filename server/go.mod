module github.com/kapitanov/led-clock/server

go 1.14

replace github.com/kapitanov/led-clock/server/module => ./module

require (
	github.com/PuerkitoBio/goquery v1.5.1 // indirect
	github.com/eclipse/paho.mqtt.golang v1.2.0
	github.com/kapitanov/led-clock/server/module v0.0.0-00010101000000-000000000000
	github.com/mattn/go-colorable v0.1.6 // indirect
	github.com/msoap/yandex-weather-cli v0.0.0-20200215212726-49e40a73b0d1 // indirect
	golang.org/x/net v0.0.0-20200324143707-d3edc9973b7e // indirect
	golang.org/x/sys v0.0.0-20200327173247-9dae0f8f5775 // indirect
	golang.org/x/text v0.3.2 // indirect
)
