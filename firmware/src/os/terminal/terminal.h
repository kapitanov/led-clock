#pragma once

#include <Arduino.h>

namespace os
{
typedef bool (*term_handler)(const String &command, const String &args);

void init(term_handler handler, int baud_rate = 115200);

void println();
void println(const __FlashStringHelper *str);
void println(const String &str);
void println(const char str[]);
void println(char x);

void print(const __FlashStringHelper *str);
void print(const String &str);
void print(const char str[]);
void print(char x);
void print(unsigned char x, int format = DEC);
void print(int x, int format = DEC);
void print(unsigned int x, int format = DEC);
void print(long x, int format = DEC);
void print(unsigned long x, int format = DEC);
void print(double x, int decimal_places = 2);
void print(const Printable &p);

void printf(const char *format, ...);
void printf(const __FlashStringHelper *format, ...);
} // namespace os
