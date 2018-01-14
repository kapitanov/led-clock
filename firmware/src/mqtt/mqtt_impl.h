#include "os/os.h"
#include "mqtt.h"

#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "../config/config.h"

namespace mqtt
{
extern WiFiClient _wifi_client;
extern MQTTClient _mqtt;

extern StaticJsonBuffer<512> _json_buffer;
extern mqtt_event_handler* _handler;

extern os::thread_id _thread_id;

extern mqtt_state_t _state;

struct config_t
{
    char host[64];
    char username[64];
    char password[64];
};

extern config_t _config;

void cfg_read();
void cfg_write();
uint32_t crc(const uint8_t *buffer, size_t length);

void on_message(String &topic, String &payload);
void set_state(mqtt_state_t state);

void fsm_init();
void fsm_connect();
void fsm_wait_connect();
void fsm_connected();
void fsm_reconnect();
void fsm_loop();
void fsm_disconnected();
}
