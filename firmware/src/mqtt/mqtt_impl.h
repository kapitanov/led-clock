#include "os/os.h"
#include "mqtt.h"

#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "../config/config.h"

namespace mqtt
{
extern WiFiClient _wifi_client;
extern AsyncMqttClient _mqtt;

extern bool _tryConnect;

extern StaticJsonDocument<512> _json;
extern mqtt_event_handler* _handler;

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
}
