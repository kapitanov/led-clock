#include "../os/os.h"
#include "mqtt_impl.h"

using namespace mqtt;
using namespace os;

void mqtt::fsm_init()
{
    cfg_read();

    _mqtt.onMessage(on_message);
    _mqtt.begin(_config.host, _wifi_client);

    if (strlen(_config.host) <= 0)
    {
        lock locker;
        attr(RED);
        println(F("MQTT connection is not configured"));
        attr(RESET);

        mqtt::_handler->on_error();
        set_state(MQTT_DISCONNECTED);
        return;
    }

    set_state(MQTT_CONNECT);
}

void mqtt::fsm_reconnect()
{
    if (_mqtt.connected())
    {
        _mqtt.disconnect();
    }

    set_state(MQTT_CONNECT);
}

void mqtt::fsm_connect()
{
    if (strlen(_config.host) <= 0)
    {
        mqtt::_handler->on_error();
        set_state(MQTT_DISCONNECTED);
        return;
    }

    set_state(MQTT_WAIT);
}

void mqtt::fsm_wait_connect()
{
    if (strlen(_config.host) <= 0)
    {
        mqtt::_handler->on_error();
        set_state(MQTT_DISCONNECTED);
        return;
    }

    printf(F("mqtt: connecting to \"%s\" as \"%s\:%s\"\r\n"), _config.host, _config.username, _config.password);
    _mqtt.setHost(_config.host);
    if (!_mqtt.connect(MQTT_CLIENT, _config.username, _config.password))
    {
        lock lock;
        attr(RED);
        printf(F("mqtt: failed to connect to \"%s\"\r\n"), _config.host);
        attr(RESET);

        mqtt::_handler->on_error();

        set_delay(1000);
        return;
    }

    set_state(MQTT_CONNECTED);
}

void mqtt::fsm_connected()
{
    _mqtt.subscribe(MQTT_TIME_TOPIC);
    _mqtt.subscribe(MQTT_WEATHER_TOPIC);

    _mqtt.publish(MQTT_REQUEST_TIME_TOPIC, "REQ");
    _mqtt.publish(MQTT_REQUEST_WEATHER_TOPIC, "REQ");

    set_state(MQTT_RUNNING);
}

void mqtt::fsm_loop()
{
    _mqtt.loop();

    if (!_mqtt.connected())
    {
        attr(RED);
        println(F("mqtt:  connection lost"));
        attr(RESET);

        mqtt::_handler->on_error();
        set_state(MQTT_CONNECT);
    }

    set_delay(10);
}

void mqtt::fsm_disconnected() {}