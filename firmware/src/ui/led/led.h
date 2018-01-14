#pragma once

#include <Arduino.h>

enum font_id {
    FONT_DEFAULT,
    FONT_MONOSPACE,
    FONT_SPECIAL
};

static const char FONT_SPECIAL_LINE = '0';
static const char FONT_SPECIAL_BAR = '1';

enum transition_type {
    TRANSITION_FADE_UP,
    TRANSITION_FADE_DOWN,
    TRANSITION_SCROLL_UP,
    TRANSITION_SCROLL_DOWN,
};

class led_matrix_t
{
public:
    led_matrix_t(int pin_din, int pin_cs, int pin_clk, int count);
    ~led_matrix_t();

    void init();
    bool get(int x, int y);
    void set(int x, int y, boolean state);
    void intensity(int intensity);
    void text(const char* str, font_id font = FONT_DEFAULT, int x = 0, int y = 0);
    void clear();
    void fill();

    void sync();
    void sync(transition_type type, int step);
    void swap();

private:
    int _pin_din;
    int _pin_cs;
    int _pin_clk;
    int _count;

    byte* _front_buffer;
    byte* _back_buffer;

    void _set_scan_limit(int addr, int limit);
    void _shutdown(bool value);
    void _spi_transfer(byte opcode, byte data);
    void _spi_transfer(int addr, byte opcode, byte data);
};
