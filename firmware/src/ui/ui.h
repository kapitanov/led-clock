#pragma once

#include <Arduino.h>

enum ui_transition
{
    UI_TRANSITION_NONE,
    UI_TRANSITION_FADE_UP,
    UI_TRANSITION_FADE_DOWN,
    UI_TRANSITION_SCROLL_UP,
    UI_TRANSITION_SCROLL_DOWN,
    UI_TRANSITION_SCROLL_LEFT,
    UI_TRANSITION_SCROLL_RIGHT,
};

enum ui_font_id
{
    UI_FONT_DEFAULT,
    UI_FONT_MONOSPACE,
    UI_FONT_CLOCK,
    UI_FONT_SPECIAL
};

static const char UI_FONT_SPECIAL_LINE = '0';
static const char UI_FONT_SPECIAL_BAR = '1';

class ui_led_matrix
{
public:
    ui_led_matrix(int pin_din, int pin_cs, int pin_clk, int count);
    ~ui_led_matrix();

    int height() const;
    int width() const;

    void init();
    bool get_front_buffer(int x, int y);
    bool get(int x, int y);
    void set(int x, int y, bool state);
    void intensity(int intensity);
    void text_char(char c, ui_font_id font, int x, int y);
    void text(const char *str, ui_font_id font = UI_FONT_DEFAULT, int x = 0, int y = 0);
    void text(const char *str, int len, ui_font_id font = UI_FONT_DEFAULT, int x = 0, int y = 0);
    void text_utf8(const char *str, int len, ui_font_id font, int x, int y);
    int measure_text(const char *str, ui_font_id font = UI_FONT_DEFAULT);
    void clear();
    void fill();

    void sync();
    void sync(ui_transition type, int step);
    bool is_completed(ui_transition type, int step);
    void swap();

private:
    int _pin_din;
    int _pin_cs;
    int _pin_clk;
    int _count;

    byte *_front_buffer;
    byte *_back_buffer;
    byte *_render_buffer;

    void _set_scan_limit(int addr, int limit);
    bool _get_render_buffer(int x, int y);
    void _set_render_buffer(int x, int y, boolean state);
    int _transition_steps(ui_transition type);
    void _shutdown(bool value);
    void _spi_transfer(byte opcode, byte data);
    void _spi_transfer(int addr, byte opcode, byte data);
    void _transition(ui_transition type, int step);
};

typedef void (*ui_screen_callback)(void *arg);
class ui_screen
{
public:
    virtual void init() = 0;
    virtual void draw(ui_led_matrix &matrix, int &delay_ms) = 0;
};

class ui_wait_screen : public ui_screen
{
public:
    virtual void init();
    virtual void draw(ui_led_matrix &matrix, int &delay_ms);

private:
    int _step;
};

class ui_wifi_screen : public ui_screen
{
public:
    virtual void init();
    virtual void draw(ui_led_matrix &matrix, int &delay_ms);

private:
    int _step;
};

class ui_text_screen : public ui_screen
{
public:
    ui_text_screen();

    void set(const char *str);
    void on_completed(ui_screen_callback func, void *arg = nullptr);
    virtual void init();
    virtual void draw(ui_led_matrix &matrix, int &delay_ms);

private:
    void complete();

    bool _needs_update;
    char *_str;
    size_t _len;
    int _offset;
    int _width;
    bool _active;
    long _time;
    ui_screen_callback _on_completed;
    void *_on_completed_arg;
};

class ui_time_screen : public ui_screen
{
public:
    void set(int h, int m);
    virtual void init();
    virtual void draw(ui_led_matrix &matrix, int &delay_ms);

private:
    int _h;
    int _m;
    int _step;
};

class ui_weather_screen : public ui_screen
{
public:
    void set(float t);
    virtual void init();
    virtual void draw(ui_led_matrix &matrix, int &delay_ms);

private:
    float _t;
};

void ui_init();
void ui_invalidate();
void ui_set_screen(ui_screen &screen, ui_transition transition = UI_TRANSITION_NONE);
void ui_set_intensity(int value);