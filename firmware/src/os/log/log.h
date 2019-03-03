#pragma once

#include <Arduino.h>

namespace os
{
void log(const __FlashStringHelper *str);
void log(const String &str);
void log(const char str[]);

void logf(const char *format, ...);
void logf(const __FlashStringHelper *format, ...);
} // namespace os
