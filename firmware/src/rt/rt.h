#pragma once

#include <Arduino.h>

#include <ArduinoJson.h>
#include <JC_Button.h>

// Initialization
void rt_init();

// Task scheduler
typedef uint8_t task_id;
extern const task_id RT_UNKNOWN_TASK;
extern const size_t RT_MAX_TASKS;
typedef void (*rt_task_func)(void *arg);

task_id rt_create_task(rt_task_func func, void *arg = nullptr);
void rt_set_delay(int32_t ms);
task_id rt_current_task_id();
void rt_destroy_task(task_id id);
void rt_scheduler_loop();

// Logging to serial
void rt_log(const __FlashStringHelper *str);
void rt_log(const String &str);
void rt_log(const char str[]);
void rt_logf(const char *format, ...);
void rt_logf(const __FlashStringHelper *format, ...);

// MQTT
enum rt_mqtt_event_type
{
    RT_MQTT_EVENT_NONE,
    RT_MQTT_EVENT_NETWORK_AVAILABLE,
    RT_MQTT_EVENT_NETWORK_LOST,
    RT_MQTT_EVENT_CONNECTED,
    RT_MQTT_EVENT_DISCONNECTED,
    RT_MQTT_EVENT_RECEIVE,
};
typedef struct rt_mqtt_event
{
    rt_mqtt_event_type type;
    const char *topic;
    const JsonDocument *json;
} rt_mqtt_event;

typedef void (*rt_mqtt_callback)(const rt_mqtt_event &event);

typedef struct rt_mqtt_config
{
    const char *host;
    int port;
    const char *username;
    const char *password;
} rt_mqtt_config;

void rt_mqtt_connect(const rt_mqtt_config &config, rt_mqtt_callback callback);
void rt_mqtt_subscribe(const String &topic);
void rt_mqtt_publish(const String &topic);
void rt_mqtt_publish(const String &topic, const JsonDocument &payload);
bool rt_mqtt_is_connected();

// EEPROM config
template <class T>
bool rt_config_read(size_t address, T &value)
{
    return rt_config_read(address, &value, sizeof(value));
}
bool rt_config_read(size_t address, void *value, size_t size);

template <class T>
void rt_config_write(const int address, T &value)
{
    rt_config_write(address, &value, sizeof(value));
}
void rt_config_write(size_t address, void *value, size_t size);

// Input

enum rt_button_event
{
    RT_BUTTON_CLICK,
    RT_BUTTON_LONG_CLICK,
};

typedef void (*rt_button_event_handler)(rt_button_event e);

class rt_button
{
public:
    rt_button(
        rt_button_event_handler handler,
        int pin,
        bool pull_up = true,
        bool invert = true,
        int debounce_ms = 20,
        int long_press_ms = 2000);
    ~rt_button();

    void update();

private:
    Button _button;
    task_id _task;
    bool _invert;
    int _long_press_ms;
    rt_button_event_handler _handler;
};
