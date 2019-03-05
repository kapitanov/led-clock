#pragma once

#define CONFIG_LED_DIN            D5
#define CONFIG_LED_CS             D7
#define CONFIG_LED_CLK            D6
#define CONFIG_LED_COUNT          4
#define CONFIG_LED_ANIMATION_STEP 10

#define CONFIG_BUTTON_PIN           D8
#define CONFIG_BUTTON_PULLUP        true
#define CONFIG_BUTTON_INVERT        true
#define CONFIG_BUTTON_DEBOUNCE_MS   20
#define CONFIG_BUTTON_LONG_PRESS    2000

#define MQTT_CLIENT "led_informer_esp8266_%X"
#define MQTT_PORT 1883

#define MQTT_REQUEST_TIME_TOPIC    "/time/request"
#define MQTT_REQUEST_WEATHER_TOPIC "/weather/request"

#define MQTT_TIME_TOPIC    "/time/update"
#define MQTT_WEATHER_TOPIC "/weather/update"