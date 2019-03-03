#include "../os/os.h"
#include "mqtt.h"
#include "mqtt_impl.h"
#include <EEPROM.h>
#include <CRC32.h>

using namespace mqtt;
using namespace os;

WiFiClient mqtt::_wifi_client;
AsyncMqttClient mqtt::_mqtt;
bool mqtt::_tryConnect = true;

StaticJsonBuffer<512> mqtt::_json_buffer;
mqtt_event_handler *mqtt::_handler;

config_t mqtt::_config;

void mqtt_on_connect(bool sessionPresent)
{
    logf(F("mqtt: connected"));

    _mqtt.subscribe(MQTT_TIME_TOPIC, 0);
    _mqtt.subscribe(MQTT_WEATHER_TOPIC, 0);

    char payload[] = "REQ";
    _mqtt.publish(MQTT_REQUEST_TIME_TOPIC, 0, false, payload);
    _mqtt.publish(MQTT_REQUEST_WEATHER_TOPIC, 0, false, payload);
}

const char *mqtt_disconnect_reason(AsyncMqttClientDisconnectReason reason)
{
    switch (reason)
    {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
        return "TCP_DISCONNECTED";
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        return "MQTT_UNACCEPTABLE_PROTOCOL_VERSION";
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
        return "MQTT_IDENTIFIER_REJECTED";
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
        return "MQTT_SERVER_UNAVAILABLE";
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        return "MQTT_MALFORMED_CREDENTIALS";
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
        return "MQTT_NOT_AUTHORIZED";
    case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
        return "ESP8266_NOT_ENOUGH_SPACE";
    case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
        return "TLS_BAD_FINGERPRINT";
    default:
        return "?";
    }
}

void mqtt_on_disconnect(AsyncMqttClientDisconnectReason reason)
{
    logf(F("mqtt: failed to connect to \"%s\": %s"), _config.host, mqtt_disconnect_reason(reason));

    mqtt::_handler->on_error();

    set_delay(1000);
    _tryConnect = true;
}

void mqtt_on_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    String topicStr(topic);
    String payloadStr(payload);
    logf(F("mqtt: <<< [%s] %s"), topic, payload);

    _json_buffer.clear();
    JsonObject &json = _json_buffer.parseObject(payload);

    if (topicStr.equals(MQTT_TIME_TOPIC))
    {
        _handler->on_time(json.get<int>("h"), json.get<int>("m"));
    }
    else if (topicStr.equals(MQTT_WEATHER_TOPIC))
    {
        _handler->on_weather(json.get<float>("now"));
    }
}

void mqtt_connect()
{
    _tryConnect = false;

    if (strlen(_config.host) <= 0)
    {
        println(F("MQTT connection is not configured"));
        mqtt::_handler->on_error();
        return;
    }

    char clientId[64] = "";
    sprintf(clientId, MQTT_CLIENT, ESP.getChipId());

    logf(F("mqtt: connecting to \"%s\" as \"%s:%s\" as \"%s\""), _config.host, _config.username, _config.password, clientId);

    _mqtt.setClientId(clientId);
    _mqtt.setServer(_config.host, MQTT_PORT);
    _mqtt.setCredentials(_config.username, _config.password);

    _mqtt.connect();
}

void mqtt_loop()
{
    if (!_tryConnect)
    {
        return;
    }

    if (!WiFi.isConnected())
    {
        return;
    }

    if (_mqtt.connected())
    {
        return;
    }

    mqtt_connect();
}

void mqtt_init(mqtt_event_handler &handler)
{
    _handler = &handler;
    cfg_read();

    _mqtt.onConnect(mqtt_on_connect);
    _mqtt.onDisconnect(mqtt_on_disconnect);
    _mqtt.onMessage(mqtt_on_message);

    create_thread(mqtt_loop, "mqtt");
}

bool mqtt_is_connected()
{
    return _mqtt.connected();
}

const char *mqtt_get_hostname()
{
    return _config.host;
}

const char *mqtt_get_username()
{
    return _config.username;
}

const char *mqtt_get_password()
{
    return _config.password;
}

void mqtt_connect(const String &host, const String &username, const String &password)
{
    strcpy(_config.host, host.c_str());
    strcpy(_config.username, username.c_str());
    strcpy(_config.password, password.c_str());

    cfg_write();

    _tryConnect = true;
}

void mqtt_reset()
{
    strcpy(_config.host, "");
    strcpy(_config.username, "");
    strcpy(_config.password, "");

    cfg_write();

    _mqtt.disconnect();
    _tryConnect = true;
}

void mqtt::cfg_read()
{
    logf(F("mqtt: reading config"));

    memset(&_config, 0, sizeof(config_t));
    EEPROM.begin(sizeof(config_t) + sizeof(uint32_t));

    for (size_t i = 0; i < sizeof(config_t); i++)
    {
        *((uint8_t *)&_config + i) = EEPROM.read(i);
    }

    uint32_t crc = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++)
    {
        *((uint8_t *)&crc + i) = EEPROM.read(sizeof(config_t) + i);
    }

    EEPROM.end();

    uint32_t actual_crc = mqtt::crc((uint8_t *)&_config, sizeof(config_t));
    if (crc != actual_crc)
    {
        logf(F("mqtt: config is damaged (CRC 0x%08X)"), crc);

        mqtt_reset();
        return;
    }

    logf(F("mqtt: config is ready (CRC 0x%08X)"), crc);
}

void mqtt::cfg_write()
{
    logf(F("mqtt: writing config"));

    EEPROM.begin(sizeof(config_t) + sizeof(uint32_t));

    const uint8_t *buffer = (uint8_t *)&_config;
    for (size_t i = 0; i < sizeof(config_t); i++)
    {
        EEPROM.write(i, buffer[i]);
    }

    uint32_t crc = mqtt::crc(buffer, sizeof(config_t));
    for (size_t i = 0; i < sizeof(uint32_t); i++)
    {
        EEPROM.write(sizeof(config_t) + i, *((uint8_t *)&crc + i));
    }

    EEPROM.commit();
    EEPROM.end();

    logf(F("mqtt: config is saved (CRC 0x%08X)"), crc);
}

uint32_t mqtt::crc(const uint8_t *buffer, size_t length)
{
    CRC32 crc;
    for (size_t i = 0; i < length; i++)
    {
        crc.update(buffer[i]);
    }

    return crc.finalize();
}