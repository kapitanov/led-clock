#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class mqtt_event_handler
{
  public:
    virtual void on_time(int h, int m) = 0;
    virtual void on_weather(float t) = 0;    
    virtual void on_error() = 0;    
};

void mqtt_init(mqtt_event_handler &callback);
bool mqtt_is_connected();
const char *mqtt_get_hostname();
const char *mqtt_get_username();
const char *mqtt_get_password();
void mqtt_connect(const String &host, const String &username, const String &password);
void mqtt_reset();