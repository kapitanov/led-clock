#pragma once

#include <Arduino.h>

enum ui_mode
{
    UI_LOADING,
    UI_TIME,
    UI_WEATHER
};

void ui_init();
ui_mode ui_get_mode();
void ui_set_mode(ui_mode mode);

void ui_set_time(int h, int m);
void ui_set_weather(float t);
void ui_blink();