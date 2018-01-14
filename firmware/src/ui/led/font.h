#pragma once

#include <Arduino.h>
#include "led.h"

void font_init();
int font_draw(led_matrix_t &m, font_id font, char c, int x, int y);