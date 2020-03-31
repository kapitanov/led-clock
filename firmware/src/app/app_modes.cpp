#include "app.h"

app_mode_connect_wifi _app_mode_connect_wifi;
app_mode_connect_mqtt _app_mode_connect_mqtt;
app_mode_time _app_mode_time;
app_mode_weather _app_mode_weather;
app_mode_covid_active _app_mode_covid_active;
app_mode_covid_recovered _app_mode_covid_recovered;
app_mode_covid_deaths _app_mode_covid_deaths;

app_mode *app_current_mode;

void app_set_mode(app_mode &mode)
{
    if (app_current_mode != &mode)
    {
        rt_logf(F("app: activated %s"), mode.name());

        app_current_mode = &mode;
        mode.activate();
    }
}

app_mode &app_get_mode()
{
    return *app_current_mode;
}

void app_task(void *arg);

void app_init()
{
    app_set_mode(_app_mode_connect_wifi);
    rt_create_task(app_task);
}

void app_task(void *arg)
{
    app_get_mode().update();
}

// ----------------------------------------------------------------------------
// app_mode
// ----------------------------------------------------------------------------

void app_mode::activate() {}

void app_mode::update() {}

void app_mode::on_network_available() {}

void app_mode::on_network_lost()
{
    app_set_mode(_app_mode_connect_wifi);
}

void app_mode::on_mqtt_connected() {}

void app_mode::on_mqtt_disconnected()
{
    app_set_mode(_app_mode_connect_mqtt);
}

void app_mode::on_data_updated(app_available_data_enum data) {}

void app_mode::on_button_event(rt_button_event e) {}

// ----------------------------------------------------------------------------
// app_mode_connect_wifi
// ----------------------------------------------------------------------------

const char *app_mode_connect_wifi::name()
{
    return "app_mode_connect_wifi";
}

void app_mode_connect_wifi::activate()
{
    ui_set_screen(_screen);
}

void app_mode_connect_wifi::on_network_available()
{
    app_set_mode(_app_mode_connect_mqtt);
}

void app_mode_connect_wifi::on_mqtt_connected()
{
    auto data = app_has_data();

    on_data_updated(data);
}

void app_mode_connect_wifi::on_data_updated(app_available_data_enum data)
{
    if ((data && APP_DATA_TIME) != 0)
    {
        app_set_mode(_app_mode_time);
    }
    else if ((data && APP_DATA_WEATHER) != 0)
    {
        app_set_mode(_app_mode_weather);
    }
}

// ----------------------------------------------------------------------------
// app_mode_connect_mqtt
// ----------------------------------------------------------------------------

const char *app_mode_connect_mqtt::name()
{
    return "app_mode_connect_mqtt";
}

void app_mode_connect_mqtt::activate()
{
    ui_set_screen(_screen);
}

void app_mode_connect_mqtt::on_mqtt_connected()
{
    auto data = app_has_data();
    on_data_updated(data);
}

void app_mode_connect_mqtt::on_data_updated(app_available_data_enum data)
{
    if ((data && APP_DATA_TIME) != 0)
    {
        app_set_mode(_app_mode_time);
    }
    else if ((data && APP_DATA_WEATHER) != 0)
    {
        app_set_mode(_app_mode_weather);
    }
}

// ----------------------------------------------------------------------------
// app_mode_active
// ----------------------------------------------------------------------------

void app_mode_active::on_button_event(rt_button_event e)
{
    if (e == RT_BUTTON_CLICK)
    {
        if (app_has_data(APP_DATA_COVID))
        {
            app_set_mode(_app_mode_covid_active);
        }

        return;
    }
}

// ----------------------------------------------------------------------------
// app_mode_time
// ----------------------------------------------------------------------------

const char *app_mode_time::name()
{
    return "app_mode_time";
}

void app_mode_time::activate()
{
    app_time_data data;
    if (app_get_time_data(data))
    {
        _screen.set(data.h, data.m);
    }

    ui_set_screen(_screen);
    _activation_time = millis();
}

void app_mode_time::update()
{
    if (millis() - _activation_time > 2500)
    {
        if (app_has_data(APP_DATA_WEATHER))
        {
            app_set_mode(_app_mode_weather);
        }
    }
    else
    {
        rt_set_delay(250);
    }
}

void app_mode_time::on_data_updated(app_available_data_enum available)
{
    app_time_data data;
    if (app_get_time_data(data))
    {
        _screen.set(data.h, data.m);
    }
}

// ----------------------------------------------------------------------------
// app_mode_weather
// ----------------------------------------------------------------------------

const char *app_mode_weather::name()
{
    return "app_mode_weather";
}

void app_mode_weather::activate()
{
    app_weather_data data;
    if (app_get_weather_data(data))
    {
        _screen.set(data.now);
    }

    ui_set_screen(_screen);
    _activation_time = millis();
}

void app_mode_weather::update()
{
    if (millis() - _activation_time > 2500)
    {
        if (app_has_data(APP_DATA_TIME))
        {
            app_set_mode(_app_mode_time);
        }
    }
    else
    {
        rt_set_delay(250);
    }
}

void app_mode_weather::on_data_updated(app_available_data_enum available)
{
    app_weather_data data;
    if (app_get_weather_data(data))
    {
        _screen.set(data.now);
    }
}

// ----------------------------------------------------------------------------
// app_mode_covid
// ----------------------------------------------------------------------------

void app_mode_covid_completed_callback(void *arg)
{
    ((app_mode_covid *)arg)->on_completed();
}

void app_mode_covid::activate()
{
    _screen.on_completed(app_mode_covid_completed_callback, this);
    if (!activate_core(_screen))
    {
        leave_mode();
        return;
    }

    ui_set_screen(_screen);
}

void app_mode_covid::leave_mode()
{
    if (app_has_data(APP_DATA_TIME))
    {
        app_set_mode(_app_mode_time);
    }
    else if (app_has_data(APP_DATA_WEATHER))
    {
        app_set_mode(_app_mode_weather);
    }
}

void app_mode_covid::on_button_event(rt_button_event e)
{
    if (e == RT_BUTTON_CLICK)
    {
        leave_mode();
        return;
    }
}

// ----------------------------------------------------------------------------
// app_mode_covid_active
// ----------------------------------------------------------------------------

const char *app_mode_covid_active::name()
{
    return "app_mode_covid_active";
}

bool app_mode_covid_active::activate_core(ui_text_screen &screen)
{
    app_covid_data data;
    if (!app_get_covid_data(data))
    {
        return false;
    }

    char buff[32];
    sprintf(buff, "   A: %d\0", data.active);
    screen.set(buff);

    rt_logf(F("app: \"%s\""), buff);

    return true;
}

void app_mode_covid_active::on_completed()
{
    app_set_mode(_app_mode_covid_recovered);
}

// ----------------------------------------------------------------------------
// app_mode_covid_recovered
// ----------------------------------------------------------------------------

const char *app_mode_covid_recovered::name()
{
    return "app_mode_covid_recovered";
}

bool app_mode_covid_recovered::activate_core(ui_text_screen &screen)
{
    app_covid_data data;
    if (!app_get_covid_data(data))
    {
        return false;
    }

    char buff[32];
    sprintf(buff, "   R: %d\0", data.recovered);
    screen.set(buff);

    rt_logf(F("app: \"%s\""), buff);

    return true;
}

void app_mode_covid_recovered::on_completed()
{
    app_set_mode(_app_mode_covid_deaths);
}

// ----------------------------------------------------------------------------
// app_mode_covid_deaths
// ----------------------------------------------------------------------------

const char *app_mode_covid_deaths::name()
{
    return "app_mode_covid_deaths";
}

bool app_mode_covid_deaths::activate_core(ui_text_screen &screen)
{
    app_covid_data data;
    if (!app_get_covid_data(data))
    {
        return false;
    }

    char buff[32];
    sprintf(buff, "   D: %d\0", data.deaths);
    screen.set(buff);

    rt_logf(F("app: \"%s\""), buff);

    return true;
}

void app_mode_covid_deaths::on_completed()
{
    leave_mode();
}
