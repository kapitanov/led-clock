#include <Arduino.h>

#include "os/os.h"
#include "ui/ui.h"
#include "btn/btn.h"
#include "cli/cli.h"
#include "mqtt/mqtt.h"

class event_handler : public mqtt_event_handler
{
  private:
    bool _has_time;
    bool _has_weather;
    bool _can_cycle = true;

  public:
    virtual void on_time(int h, int m)
    {
        ui_mode mode = ui_get_mode();
        switch (mode)
        {
        case UI_LOADING:
            ui_set_mode(UI_TIME);
            break;
        }

        ui_set_time(h, m);
        _has_time = true;
    }

    virtual void on_weather(float t)
    {
        ui_set_weather(t);
        _has_weather = true;
    }

    virtual void on_error()
    {
        ui_set_mode(UI_LOADING);
        _has_time = false;
        _has_weather = false;
    }

    void on_btn_pressed(btn_cmd_t cmd)
    {
        switch (cmd)
        {
        case BTN_CLICK:
            os::logf(F("Button pressed"));
            if (!toggle_mode())
            {
                ui_set_mode(ui_get_mode());
            }            
            break;

        case BTN_LONG_CLICK:
            os::logf(F("Button long press detected, will now reboot"));
            ESP.restart();
            break;
        }
    }

    void on_timer()
    {
        if (_can_cycle)
        {
            toggle_mode();
        }
    }

  private:
    bool toggle_mode()
    {
        ui_mode mode = ui_get_mode();
        switch (mode)
        {
        case UI_TIME:
            if (_has_weather)
            {
                ui_set_mode(UI_WEATHER);
                return true;
            }
            os::logf(F("Can't switch mode to UI_WEATHER: no weather data is available"));
            break;

        case UI_WEATHER:
            if (_has_time)
            {
                ui_set_mode(UI_TIME);
                return true;
            }
            os::logf(F("Can't switch mode to UI_TIME: no time data is available"));
            break;
        }
        
        return false;
    }
};

event_handler handler;

void on_btn_pressed(btn_cmd_t cmd)
{
    handler.on_btn_pressed(cmd);
}

void on_timer()
{
    handler.on_timer();
    os::set_delay(5 * 1000);
}

void setup()
{
    ui_init();
    btn_init(on_btn_pressed);
    os::init(cli_handler);
    mqtt_init(handler);
    os::create_thread(on_timer, "ui_timer");
}

void loop()
{
    os::run();
}