#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>

#include "rt/rt.h"
#include "app/app.h"
#include "ui/ui.h"
#include "config.h"

#define CONFIG_BUTTON_PIN D8

struct app_config
{
    char wifi_ssid[64];
    char wifi_password[64];

    char mqtt_host[64];
    int mqtt_port;
    char mqtt_username[64];
    char mqtt_password[64];
};

app_config app_config_instance = {};

void mqtt_callback(const rt_mqtt_event &event)
{
    if (event.type == RT_MQTT_EVENT_CONNECTED)
    {
        rt_mqtt_subscribe("/time/update");
        rt_mqtt_subscribe("/weather/update");
        rt_mqtt_subscribe("/covid19/update");

        rt_mqtt_publish("/time/request");
        rt_mqtt_publish("/weather/request");
        rt_mqtt_publish("/covid19/request");
    }

    app_on_mqtt_event(event);
}

void on_button_event(rt_button_event e)
{
    app_get_mode().on_button_event(e);
}

rt_button button(
    on_button_event,
    CONFIG_BUTTON_PIN,
    CONFIG_BUTTON_PULLUP,
    CONFIG_BUTTON_INVERT,
    CONFIG_BUTTON_DEBOUNCE_MS,
    CONFIG_BUTTON_LONG_PRESS);

void setup()
{
    rt_init();
    ui_init();
    app_init();

    if (!rt_config_read<app_config>(0, app_config_instance))
    {
        rt_log(F("Chip is not configured"));
        return;
    }

    // Initialize WiFi
    rt_logf(F("connecting to wifi: \"%s\" (password \"%s\")"), app_config_instance.wifi_ssid, app_config_instance.wifi_password);
    WiFi.begin(app_config_instance.wifi_ssid, app_config_instance.wifi_password);
    
    WiFi.setAutoReconnect(true);
    WiFi.setAutoConnect(true);

    // Connect to MQTT
    rt_mqtt_config mqtt_config;
    mqtt_config.host = app_config_instance.mqtt_host;
    mqtt_config.port = app_config_instance.mqtt_port;
    mqtt_config.username = app_config_instance.mqtt_username;
    mqtt_config.password = app_config_instance.mqtt_password;

    rt_mqtt_connect(mqtt_config, mqtt_callback);
}

void loop()
{
    rt_scheduler_loop();
}