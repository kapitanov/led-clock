#pragma once

#include <Arduino.h>

namespace os
{
enum attr_t
{
    RESET = 0,

    BOLD = 1,
    DIM = 2,
    UNDERLINE = 4,
    BLINK = 5,
    INVERSE = 7,
    HIDDEN = 8,

    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    LIGHT_GRAY = 37,
    NORMAL = 39,

    DARK_GRAY = 90,
    LIGHT_RED = 91,
    LIGHT_GREEN = 92,
    LIGHT_YELLOW = 93,
    LIGHT_BLUE = 94,
    LIGHT_MAGENTA = 95,
    LIGHT_CYAN = 96,
    WHITE = 97,

    BG_NORMAL = 49,
    BG_BLACK = 40,
    BG_RED = 41,
    BG_GREEN = 4,
    BG_YELLOW = 43,
    BG_BLUE = 44,
    BG_MAGENTA = 45,
    BG_CYAN = 46,
    BG_LIGHT_GRAY = 47,

    BG_DARK_GRAY = 100,
    BG_LIGHT_RED = 101,
    BG_LIGHT_GREEN = 102,
    BG_LIGHT_YELLOW = 103,
    BG_LIGHT_BLUE = 104,
    BG_LIGHT_MAGENTA = 105,
    BG_LIGHT_CYAN = 106,
    BG_WHITE = 107,
};

typedef bool (*term_handler)(const String &command, const String &args);

void init(term_handler handler, int baud_rate = 115200);

void attr(attr_t c);
void attr(attr_t c1, attr_t c2);
void attr(attr_t c1, attr_t c2, attr_t c3);

void bell();

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

void cursor_col(int n = 1);
void cursor_left(int n = 1);
void cursor_right(int n = 1);
void cursor_up(int n = 1);
void cursor_down(int n = 1);

void erase_line();
void clear();

void lock_term();
void release_term();

class lock
{
  public:
    lock();
    ~lock();
};
}
