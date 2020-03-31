#include "ui.h"
#include "font.h"
#include "FS.h"

static const byte OP_DECODEMODE = 9;
static const byte OP_INTENSITY = 10;
static const byte OP_SCANLIMIT = 11;
static const byte OP_SHUTDOWN = 12;
static const byte OP_DISPLAYTEST = 15;

ui_led_matrix::ui_led_matrix(int pin_din, int pin_cs, int pin_clk, int count)
{
    _pin_din = pin_din;
    _pin_cs = pin_cs;
    _pin_clk = pin_clk;
    _count = count;

    _front_buffer = new byte[8 * count];
    _back_buffer = new byte[8 * count];
    _render_buffer = new byte[8 * count];
}

ui_led_matrix::~ui_led_matrix()
{
    delete[] _front_buffer;
    delete[] _back_buffer;
    delete[] _render_buffer;
}

int ui_led_matrix::height() const
{
    return 8;
}

int ui_led_matrix::width() const
{
    return _count * 8;
}

void ui_led_matrix::init()
{
    font_init();

    pinMode(_pin_din, OUTPUT);
    pinMode(_pin_clk, OUTPUT);
    pinMode(_pin_cs, OUTPUT);
    digitalWrite(_pin_cs, HIGH);

    _spi_transfer(OP_SCANLIMIT, 7);
    _spi_transfer(OP_DECODEMODE, 0);
    _spi_transfer(OP_SHUTDOWN, 1);
    _spi_transfer(OP_DISPLAYTEST, 0);

    clear();
    sync();

    _spi_transfer(OP_INTENSITY, 0x0f);
}

void ui_led_matrix::intensity(int intensity)
{
    for (int i = 0; i < _count; i++)
    {
        _spi_transfer(i, OP_INTENSITY, intensity);
    }
}

bool ui_led_matrix::get_front_buffer(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_front_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

bool ui_led_matrix::_get_render_buffer(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_render_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

bool ui_led_matrix::get(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_back_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

void ui_led_matrix::set(int x, int y, bool state)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return;
    }

    bitWrite(_back_buffer[x / 8 + _count * (y % 8)], 7 - x % 8, state);
}

void ui_led_matrix::text_char(const char c, ui_font_id font, int x, int y)
{
    font_draw(*this, font, c, x, y);
}

void ui_led_matrix::text(const char *str, ui_font_id font, int x, int y)
{
    text(str, strlen(str), font, x, y);
}

void ui_led_matrix::text(const char *str, int len, ui_font_id font, int x, int y)
{
    int width = x;

    int i = 0;
    while (*str != 0 && width < _count * 8 && i < len)
    {
        int w = font_draw(*this, font, *str, width, y);
        width += w;
        str++;
        i++;
    }
}

void ui_led_matrix::text_utf8(const char *str, int len, ui_font_id font, int x, int y)
{
    String input(str);
    String output = font_utf8_to_win1251(input);
    str = output.c_str();

    text(str, len, font, x, y);
}

int ui_led_matrix::measure_text(const char *str, ui_font_id font)
{
    String input(str);
    String output = font_utf8_to_win1251(input);
    str = output.c_str();

    int width = 0;

    while (*str != 0)
    {
        int w = font_measure(*this, font, *str);
        width += w;
        str++;
    }

    return width;
}

void ui_led_matrix::clear()
{
    for (int i = 0; i < _count * 8; i++)
    {
        _back_buffer[i] = 0;
    }
}

void ui_led_matrix::fill()
{
    swap();
    for (int i = 0; i < _count * 8; i++)
    {
        _back_buffer[i] = 0xFF;
    }
}

void ui_led_matrix::_shutdown(bool value)
{
    for (int i = 0; i < _count; i++)
    {
        _spi_transfer(i, OP_SHUTDOWN, value ? 0 : 1);
    }
}

void ui_led_matrix::sync()
{
    sync(UI_TRANSITION_NONE, 0);
}

void ui_led_matrix::sync(ui_transition type, int step)
{
    _transition(type, step);

    for (int y = 0; y < 8; y++)
    {
        digitalWrite(_pin_cs, LOW);

        for (int addr = 0; addr < _count; addr++)
        {
            shiftOut(_pin_din, _pin_clk, MSBFIRST, y + 1);
            shiftOut(_pin_din, _pin_clk, MSBFIRST, _render_buffer[y * _count + addr]);
        }

        digitalWrite(_pin_cs, HIGH);
    }
}

bool ui_led_matrix::is_completed(ui_transition type, int step)
{
    return step >= _transition_steps(type);
}

int ui_led_matrix::_transition_steps(ui_transition type)
{
    switch (type)
    {
    case UI_TRANSITION_FADE_UP:
    case UI_TRANSITION_FADE_DOWN:
    case UI_TRANSITION_SCROLL_UP:
    case UI_TRANSITION_SCROLL_DOWN:
        return height();

    case UI_TRANSITION_SCROLL_LEFT:
    case UI_TRANSITION_SCROLL_RIGHT:
        return width();

    default:
        return 1;
    }
}

void ui_led_matrix::_transition(ui_transition type, int step)
{
    step = step % _transition_steps(type);

    switch (type)
    {
    case UI_TRANSITION_FADE_UP:
    {
        int y = 0;
        for (; y < step; y++)
        {
            for (int x = 0; x < width(); x++)
            {
                _set_render_buffer(x, y, get_front_buffer(x, y));
            }
        }

        for (; y < height(); y++)
        {
            for (int x = 0; x < width(); x++)
            {
                _set_render_buffer(x, y, get(x, y));
            }
        }
    }
    break;
    case UI_TRANSITION_FADE_DOWN:
    {
        int y = 0;
        for (; y < step; y++)
        {
            for (int x = 0; x < width(); x++)
            {
                _set_render_buffer(x, y, get(x, y));
            }
        }

        for (; y < height(); y++)
        {
            for (int x = 0; x < width(); x++)
            {
                _set_render_buffer(x, y, get_front_buffer(x, y));
            }
        }
    }
    break;

    case UI_TRANSITION_SCROLL_UP:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < height(); y++)
            {
                if (height() - y > step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x, y + step));
                }
                else
                {
                    _set_render_buffer(x, y, get(x, y + step - height()));
                }
            }
        }
        break;

    case UI_TRANSITION_SCROLL_DOWN:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < height(); y++)
            {
                if (y >= step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x, y - step));
                }
                else
                {
                    _set_render_buffer(x, y, get(x, height() - step + y));
                }
            }
        }
        break;

    case UI_TRANSITION_SCROLL_LEFT:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < height(); y++)
            {
                if (x >= step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x - step, y));
                }
                else
                {
                    _set_render_buffer(x, y, get(width() - step + x, y));
                }
            }
        }
        break;

    case UI_TRANSITION_SCROLL_RIGHT:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < height(); y++)
            {
                if (width() - x > step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x + step, y));
                }
                else
                {
                    _set_render_buffer(x, y, get(x + step - width(), y));
                }
            }
        }
        break;

    default:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < 8; y++)
            {
                _set_render_buffer(x, y, get(x, y));
            }
        }
        break;
    }
}

void ui_led_matrix::_set_render_buffer(int x, int y, bool state)
{
    bitWrite(_render_buffer[x / 8 + _count * (y % 8)], 7 - x % 8, state);
}

void ui_led_matrix::swap()
{
    for (int i = 0; i < 8 * _count; i++)
    {
        _front_buffer[i] = _back_buffer[i];
    }
}

void ui_led_matrix::_spi_transfer(byte command, byte data)
{
    digitalWrite(_pin_cs, LOW);
    for (int i = 0; i < _count; i++)
    {
        shiftOut(_pin_din, _pin_clk, MSBFIRST, command);
        shiftOut(_pin_din, _pin_clk, MSBFIRST, data);
    }
    digitalWrite(_pin_cs, LOW);
    digitalWrite(_pin_cs, HIGH);
}

void ui_led_matrix::_spi_transfer(int addr, byte command, byte data)
{
    digitalWrite(_pin_cs, LOW);
    for (int i = 0; i < _count; i++)
    {
        if (addr == i)
        {
            shiftOut(_pin_din, _pin_clk, MSBFIRST, command);
            shiftOut(_pin_din, _pin_clk, MSBFIRST, data);
        }
        else
        {
            shiftOut(_pin_din, _pin_clk, MSBFIRST, 0);
            shiftOut(_pin_din, _pin_clk, MSBFIRST, 0);
        }
    }

    digitalWrite(_pin_cs, LOW);
    digitalWrite(_pin_cs, HIGH);
}
