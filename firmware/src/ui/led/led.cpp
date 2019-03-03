#include "led.h"
#include "font.h"
#include "FS.h"
#include "os/os.h"

static const byte OP_DECODEMODE = 9;
static const byte OP_INTENSITY = 10;
static const byte OP_SCANLIMIT = 11;
static const byte OP_SHUTDOWN = 12;
static const byte OP_DISPLAYTEST = 15;

led_matrix_t::led_matrix_t(int pin_din, int pin_cs, int pin_clk, int count)
{
    _pin_din = pin_din;
    _pin_cs = pin_cs;
    _pin_clk = pin_clk;
    _count = count;

    _front_buffer = new byte[8 * count];
    _back_buffer = new byte[8 * count];
    _render_buffer = new byte[8 * count];
}

led_matrix_t::~led_matrix_t()
{
    delete[] _front_buffer;
    delete[] _back_buffer;
    delete[] _render_buffer;
}

int led_matrix_t::height() const
{
    return 8;
}

int led_matrix_t::width() const
{
    return _count * 8;
}

void led_matrix_t::init()
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

void led_matrix_t::intensity(int intensity)
{
    for (int i = 0; i < _count; i++)
    {
        _spi_transfer(i, OP_INTENSITY, intensity);
    }
}

bool led_matrix_t::get_front_buffer(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_front_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

bool led_matrix_t::_get_render_buffer(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_render_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

bool led_matrix_t::get(int x, int y)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return false;
    }

    return bitRead(_back_buffer[x / 8 + _count * (y % 8)], 7 - x % 8);
}

void led_matrix_t::set(int x, int y, boolean state)
{
    if (x < 0 || x >= _count * 8 || y < 0 || y >= 8)
    {
        return;
    }

    bitWrite(_back_buffer[x / 8 + _count * (y % 8)], 7 - x % 8, state);
}

void led_matrix_t::text(const char *str, font_id font, int x, int y)
{
    int width = x;

    while (*str != 0 && width < _count * 8)
    {
        int w = font_draw(*this, font, *str, width, y);
        width += w;
        str++;
    }
}

void led_matrix_t::clear()
{
    for (int i = 0; i < _count * 8; i++)
    {
        _back_buffer[i] = 0;
    }
}

void led_matrix_t::fill()
{
    swap();
    for (int i = 0; i < _count * 8; i++)
    {
        _back_buffer[i] = 0xFF;
    }
}

void led_matrix_t::_shutdown(bool value)
{
    for (int i = 0; i < _count; i++)
    {
        _spi_transfer(i, OP_SHUTDOWN, value ? 0 : 1);
    }
}

void led_matrix_t::sync()
{
    sync(TRANSITION_NONE, 0);
}

void led_matrix_t::sync(transition_type type, int step)
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

void led_matrix_t::_transition(transition_type type, int step)
{
    switch (type)
    {
    case TRANSITION_FADE_UP:
    case TRANSITION_FADE_DOWN:

    case TRANSITION_SCROLL_UP:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < 8; y++)
            {
                if (8 - y > step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x, y + step));
                }
                else
                {
                    _set_render_buffer(x, y, get(x, y + step - 8));
                }
            }
        }
        break;

    case TRANSITION_SCROLL_DOWN:
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < 8; y++)
            {
                if (y >= step)
                {
                    _set_render_buffer(x, y, get_front_buffer(x, y - step));
                }
                else
                {
                    _set_render_buffer(x, y, get(x, 8 - step + y));
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

void led_matrix_t::_set_render_buffer(int x, int y, boolean state)
{
    bitWrite(_render_buffer[x / 8 + _count * (y % 8)], 7 - x % 8, state);
}

void led_matrix_t::swap()
{
    for (int i = 0; i < 8 * _count; i++)
    {
        _front_buffer[i] = _back_buffer[i];
    }
}

void led_matrix_t::_spi_transfer(byte command, byte data)
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

void led_matrix_t::_spi_transfer(int addr, byte command, byte data)
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
