#pragma once

#include <Arduino.h>
#include "ui.h"

void font_init();
String font_utf8_to_win1251(const String& source);
int font_measure(ui_led_matrix &m, ui_font_id font, char c);
int font_draw(ui_led_matrix &m, ui_font_id font, char c, int x, int y);