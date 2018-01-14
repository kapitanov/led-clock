#pragma once

#include <Arduino.h>

typedef void (*btn_handler)();

void btn_init(btn_handler handler);