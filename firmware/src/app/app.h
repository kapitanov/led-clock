#pragma once

#include <Arduino.h>

#include "../rt/rt.h"
#include "../ui/ui.h"

enum app_available_data_enum
{
    APP_DATA_NONE = 0,
    APP_DATA_TIME = 1,
    APP_DATA_WEATHER = 2,
    APP_DATA_COVID = 4,
};

class app_time_data
{
public:
    int h;
    int m;
};

class app_weather_data
{
public:
    float now;
};

class app_covid_data
{
public:
    int active;
    int recovered;
    int deaths;
};

class app_mode
{
public:
    virtual const char* name() = 0;
    virtual void activate();
    virtual void update();
    virtual void on_network_available();
    virtual void on_network_lost();
    virtual void on_mqtt_connected();
    virtual void on_mqtt_disconnected();
    virtual void on_data_updated(app_available_data_enum data);
    virtual void on_button_event(rt_button_event e);
};

class app_mode_connect_wifi : public app_mode
{
public:
    virtual const char* name();
    virtual void activate();
    virtual void on_network_available();
    virtual void on_data_updated(app_available_data_enum data);
    virtual void on_mqtt_connected();

private:
    ui_wifi_screen _screen;
};

class app_mode_connect_mqtt : public app_mode
{
public:
    virtual const char* name();
    virtual void activate();
    virtual void on_data_updated(app_available_data_enum data);
    virtual void on_mqtt_connected();

private:
    ui_wait_screen _screen;
};

class app_mode_active : public app_mode
{
public:
    virtual void on_button_event(rt_button_event e);
};

class app_mode_time : public app_mode_active
{
public:
    virtual const char* name();
    virtual void activate();
    virtual void update();
    virtual void on_data_updated(app_available_data_enum data);

private:
    ui_time_screen _screen;
    long _activation_time;
};

class app_mode_weather : public app_mode_active
{
public:
    virtual const char* name();
    virtual void activate();
    virtual void update();
    virtual void on_data_updated(app_available_data_enum data);

private:
    ui_weather_screen _screen;
    long _activation_time;
};

class app_mode_covid : public app_mode
{
public:
    virtual void activate();
    virtual void on_button_event(rt_button_event e);
    virtual void on_completed() = 0;

protected:
    virtual bool activate_core(ui_text_screen& screen) = 0;
    void leave_mode();

private:
    ui_text_screen _screen;
};

class app_mode_covid_active : public app_mode_covid
{
public:
    virtual const char* name();
    virtual void on_completed();

protected:
    virtual bool activate_core(ui_text_screen& screen);
};

class app_mode_covid_recovered : public app_mode_covid
{
public:
    virtual const char* name();
    virtual void on_completed();

protected:
    virtual bool activate_core(ui_text_screen& screen);
};

class app_mode_covid_deaths : public app_mode_covid
{
public:
    virtual const char* name();
    virtual void on_completed();

protected:
    virtual bool activate_core(ui_text_screen& screen);
};

extern app_mode_connect_wifi _app_mode_connect_wifi;
extern app_mode_connect_mqtt _app_mode_connect_mqtt;
extern app_mode_time _app_mode_time;
extern app_mode_weather _app_mode_weather;
extern app_mode_covid_active _app_mode_covid_active;
extern app_mode_covid_recovered _app_mode_covid_recovered;
extern app_mode_covid_deaths _app_mode_covid_deaths;

void app_set_mode(app_mode &mode);
app_mode &app_get_mode();

void app_init();
void app_on_mqtt_event(const rt_mqtt_event &event);

app_available_data_enum app_has_data();
bool app_has_data(app_available_data_enum data);

bool app_get_time_data(app_time_data &data);
bool app_get_weather_data(app_weather_data &data);
bool app_get_covid_data(app_covid_data &data);

void app_set_time_data(const app_time_data &data);
void app_set_weather_data(const app_weather_data &data);
void app_set_covid_data(const app_covid_data &data);
