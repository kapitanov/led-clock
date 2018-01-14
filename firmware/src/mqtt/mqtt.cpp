#include "../os/os.h"
#include "mqtt.h"
#include "mqtt_impl.h"
#include <EEPROM.h>
#include <CRC32.h>

using namespace mqtt;
using namespace os;

WiFiClient mqtt::_wifi_client;
MQTTClient mqtt::_mqtt;

StaticJsonBuffer<512> mqtt::_json_buffer;
mqtt_event_handler *mqtt::_handler;
thread_id mqtt::_thread_id;

config_t mqtt::_config;
mqtt_state_t mqtt::_state;

void mqtt::on_message(String &topic, String &payload)
{
    {
        lock lock;
        printf(F("mqtt: <<< [%s] "), topic.c_str());
        attr(INVERSE);
        print(payload.c_str());
        attr(RESET);
        println();
    }

    _json_buffer.clear();
    JsonObject &json = _json_buffer.parseObject(payload);

    if (topic.equals(MQTT_TIME_TOPIC))
    {
        _handler->on_time(json.get<int>("h"), json.get<int>("m"));
    }
    else if (topic.equals(MQTT_WEATHER_TOPIC))
    {
        _handler->on_weather(json.get<float>("now"));
    }
}

void mqtt_init(mqtt_event_handler &handler)
{
    _handler = &handler;
    cfg_read();

    _thread_id = create_thread(fsm_init, "mqtt");
    _state = MQTT_INIT;
}

bool mqtt_is_connected()
{
    return _mqtt.connected();
}

mqtt_state_t mqtt_get_state()
{
    return _state;
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

    set_state(MQTT_RECONNECT);
}

void mqtt_reset()
{
    strcpy(_config.host, "");
    strcpy(_config.username, "");
    strcpy(_config.password, "");

    cfg_write();

    set_state(MQTT_RECONNECT);
}

void mqtt::cfg_read()
{
    lock l;
    println(F("mqtt: reading config"));

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
        attr(RED);
        printf(F("mqtt: config is damaged (CRC 0x%08X)\r\n"), crc);
        attr(RESET);

        mqtt_reset();
        return;
    }

    printf(F("mqtt: config is ready (CRC 0x%08X)\r\n"), crc);
}

void mqtt::cfg_write()
{
    lock l;
    println(F("mqtt: writing config"));

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

    printf(F("mqtt: config is saved (CRC 0x%08X)\r\n"), crc);
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

void mqtt::set_state(mqtt_state_t state)
{
    switch (state)
    {
    case MQTT_INIT:
        set_func(_thread_id, fsm_init, "init");
        break;
    case MQTT_CONNECT:
        set_func(_thread_id, fsm_connect, "connect");
        break;
    case MQTT_RECONNECT:
        set_func(_thread_id, fsm_reconnect, "reconnect");
        break;
    case MQTT_DISCONNECTED:
        set_func(_thread_id, fsm_disconnected, "disconnected");
        break;
    case MQTT_RUNNING:
        set_func(_thread_id, fsm_loop, "loop");
        break;
    case MQTT_CONNECTED:
        set_func(_thread_id, fsm_connected, "connected");
        break;
    case MQTT_WAIT:
        set_func(_thread_id, fsm_wait_connect, "wait_connect");
        break;
    default:
        return;
    }
    _state = state;
}