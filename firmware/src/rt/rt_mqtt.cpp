#include "rt.h"

#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <CRC32.h>

WiFiClient rt_mqtt_wifi_client;
AsyncMqttClient rt_mqtt_client;
StaticJsonDocument<512> rt_mqtt_json;

enum rt_mqtt_state_enum
{
    RT_MQTT_OFFLINE = 0,
    RT_MQTT_WAIT_FOR_WIFI = 1,
    RT_MQTT_WAIT_FOR_CONNECTION = 2,
    RT_MQTT_ONLINE = 3,
};
rt_mqtt_state_enum rt_mqtt_state;

bool rt_mqtt_is_initialized;
bool rt_mqtt_is_running;
rt_mqtt_callback rt_mqtt_callback_func;

rt_mqtt_config _rt_mqtt_config;

const char *RT_MQTT_CLIENT_ID = "led_clock_esp8266_%X";

// MQTT callbacks
void rt_mqtt_on_connect(bool sessionPresent);
void rt_mqtt_on_disconnect(AsyncMqttClientDisconnectReason reason);
void rt_mqtt_on_message(
    char *topic,
    char *payload,
    AsyncMqttClientMessageProperties properties,
    size_t len,
    size_t index,
    size_t total);

// Helpers
const char *rt_mqtt_disconnect_reason(AsyncMqttClientDisconnectReason reason);
void rt_mqtt_task(void *arg);

void rt_mqtt_connect(const rt_mqtt_config &config, rt_mqtt_callback callback)
{
    memcpy(&_rt_mqtt_config, &config, sizeof(rt_mqtt_config));

    if (!rt_mqtt_is_initialized)
    {
        rt_mqtt_client.onConnect(rt_mqtt_on_connect);
        rt_mqtt_client.onDisconnect(rt_mqtt_on_disconnect);
        rt_mqtt_client.onMessage(rt_mqtt_on_message);

        rt_create_task(rt_mqtt_task);

        rt_mqtt_is_initialized = true;
    }

    if (rt_mqtt_client.connected())
    {
        rt_mqtt_client.disconnect();
    }

    rt_mqtt_callback_func = callback;

    char clientId[64] = "";
    sprintf(clientId, RT_MQTT_CLIENT_ID, ESP.getChipId());

    rt_mqtt_client.setServer(_rt_mqtt_config.host, _rt_mqtt_config.port);
    rt_mqtt_client.setClientId(clientId);

    rt_logf(F("mqtt: will connect to %s:%d as \"%s\""), _rt_mqtt_config.host, _rt_mqtt_config.port, clientId);

    if (_rt_mqtt_config.username != nullptr && _rt_mqtt_config.password != nullptr)
    {
        rt_logf(F("mqtt: credentials are \"%s:%s\""), _rt_mqtt_config.username, _rt_mqtt_config.password);
        rt_mqtt_client.setCredentials(_rt_mqtt_config.username, _rt_mqtt_config.password);
    }

    rt_mqtt_client.disconnect();
    rt_mqtt_state = RT_MQTT_WAIT_FOR_WIFI;
}

bool rt_mqtt_is_connected()
{
    return rt_mqtt_client.connected();
}

void rt_mqtt_subscribe(const String &topic)
{
    rt_mqtt_client.subscribe(topic.c_str(), 0);
    rt_logf(F("mqtt: subscribed to \"%s\""), topic.c_str());
}

void rt_mqtt_publish(const String &topic)
{
    rt_logf(F("mqtt: >>> [%s] NULL"), topic.c_str());
    rt_mqtt_client.publish(topic.c_str(), 0, false, nullptr);
}

void rt_mqtt_publish(const String &topic, const JsonDocument &payload)
{
    const size_t default_size = 64;
    char *buffer = new char[default_size];
    auto size = serializeJson(payload, buffer, default_size);
    if (size > default_size && size <= 1024)
    {
        delete[] buffer;
        buffer = new char[size];
        serializeJson(payload, buffer, size);
    }

    rt_logf(F("mqtt: >>> [%s] \"%s\""), topic.c_str(), buffer);
    rt_mqtt_client.publish(topic.c_str(), 0, false, buffer);
    delete[] buffer;
}

// MQTT callbacks
void rt_mqtt_on_connect(bool sessionPresent)
{
    rt_log(F("mqtt: connected"));

    rt_mqtt_event e;
    e.type = RT_MQTT_EVENT_CONNECTED;
    rt_mqtt_callback_func(e);
}

void rt_mqtt_on_disconnect(AsyncMqttClientDisconnectReason reason)
{
    rt_logf(F("mqtt: disconnected: %s"), rt_mqtt_disconnect_reason(reason));

    rt_mqtt_event e;
    e.type = RT_MQTT_EVENT_DISCONNECTED;
    rt_mqtt_callback_func(e);

    if (!WiFi.isConnected())
    {
        rt_logf(F("mqtt: network lost"));

        rt_mqtt_state = RT_MQTT_WAIT_FOR_WIFI;

        rt_mqtt_event e;
        e.type = RT_MQTT_EVENT_NETWORK_LOST;
        rt_mqtt_callback_func(e);
    }
    else
    {
        rt_logf(F("mqtt: reconnecting"));

        rt_mqtt_state = RT_MQTT_WAIT_FOR_CONNECTION;
        rt_mqtt_client.disconnect(true);
        rt_mqtt_client.connect();
    }
}

void rt_mqtt_on_message(
    char *topic,
    char *payload,
    AsyncMqttClientMessageProperties properties,
    size_t len,
    size_t index,
    size_t total)
{
    rt_logf(F("mqtt: <<< [%s] %s"), topic, payload);

    rt_mqtt_json.clear();

    auto err = deserializeJson(rt_mqtt_json, payload);
    if (err.code() != DeserializationError::Ok)
    {
        rt_logf(F("mqtt: unable to parse json <<<%s>>> from \"%s\": %s"), payload, topic, err.c_str());
        return;
    }

    rt_mqtt_event e;
    e.type = RT_MQTT_EVENT_RECEIVE;
    e.topic = topic;
    e.json = &rt_mqtt_json;

    rt_mqtt_callback_func(e);
}

// Helpers
const char *rt_mqtt_disconnect_reason(AsyncMqttClientDisconnectReason reason)
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

void rt_mqtt_task(void *arg)
{
    if (rt_mqtt_state == RT_MQTT_WAIT_FOR_WIFI)
    {
        auto status = WiFi.status();
        if (status != WL_CONNECTED)
        {
            // rt_logf(F("mqtt: network not connected (%d)"), status);
            delay(100);
            return;
        }

        rt_logf(F("mqtt: network connected to \"%s\" as \"%s\""), WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

        rt_mqtt_event e;
        e.type = RT_MQTT_EVENT_NETWORK_AVAILABLE;
        rt_mqtt_callback_func(e);

        rt_mqtt_state = RT_MQTT_WAIT_FOR_CONNECTION;
        rt_mqtt_client.connect();
        rt_logf(F("mqtt: waiting for connection"));
        return;
    }

    if (rt_mqtt_state == RT_MQTT_WAIT_FOR_CONNECTION)
    {
        if (!rt_mqtt_client.connected())
        {
            return;
        }

        rt_logf(F("mqtt: connection established"));
        rt_mqtt_state = RT_MQTT_ONLINE;
        return;
    }

    if (!rt_mqtt_is_running)
    {
        return;
    }
}