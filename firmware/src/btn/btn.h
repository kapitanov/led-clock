#pragma once

#include <Arduino.h>

enum btn_cmd_t
{
    BTN_CLICK,
    BTN_LONG_CLICK,
};

typedef void (*btn_handler)(btn_cmd_t cmd);

void btn_init(btn_handler handler);