#include "terminal_impl.h"

using namespace os;

int os::_col = 1;

void os::_attr(attr_t c)
{
    Serial.print(F("\x1b["));
    Serial.print((int)c);
    Serial.print("m");
}

void os::_attr(attr_t c1, attr_t c2)
{
    Serial.print(F("\x1b["));
    Serial.print((int)c1);
    Serial.print(';');
    Serial.print((int)c2);
    Serial.print('m');
}

void os::_attr(attr_t c1, attr_t c2, attr_t c3)
{
    Serial.print(F("\x1b["));
    Serial.print((int)c1);
    Serial.print(';');
    Serial.print((int)c2);
    Serial.print(';');
    Serial.print((int)c3);
    Serial.print('m');
}

void os::_print(const char c)
{
    switch (c)
    {
    case '\r':
        break;
    case '\n':
        _col = 1;
        break;
    default:
        _col++;
        break;
    }

    Serial.print(c);
}

void os::_print(const char *str)
{
    int i = 0;
    while (str[i] != 0)
    {
        _print(str[i]);
        i++;
    }
}

void os::_print(const String &str)
{
    for (int i = 0; i < str.length(); i++)
    {
        _print(str.charAt(i));
    }
}

void os::_print(const __FlashStringHelper *str)
{
    PGM_P p = reinterpret_cast<PGM_P>(str);

    int i = 0;

    while (true)
    {
        uint8_t c = pgm_read_byte(p++);
        if (c == 0)
        {
            break;
        }

        _print((char)c);
        i++;
    }
}

void os::_println()
{
    _print('\r');
    _print('\n');
}

void os::_println(const char *str)
{
    _print(str);
    _println();
}

void os::_println(const String &str)
{
    _print(str);
    _println();
}

void os::_println(const __FlashStringHelper *str)
{
    _print(str);
    _println();
}

void os::_printf(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char *buffer = temp;
    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1)
    {
        buffer = new char[len + 1];
        if (!buffer)
        {
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }

    for (int i = 0; i < len; i++)
    {
        _print(buffer[i]);
    }
    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void os::_printf(const __FlashStringHelper *format, ...)
{
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char *buffer = temp;
    size_t len = vsnprintf_P(temp, sizeof(temp), (const char *)format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1)
    {
        buffer = new char[len + 1];
        if (!buffer)
        {
            return;
        }
        va_start(arg, format);
        vsnprintf_P(buffer, len + 1, (const char *)format, arg);
        va_end(arg);
    }

    for (int i = 0; i < len; i++)
    {
        _print(buffer[i]);
    }
    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void os::_cursor_col(int n)
{
    Serial.printf("\x1b[%0dG", n);
    _col = n;
    if (_col < 1)
    {
        _col = 1;
    }
}

void os::_cursor_left(int n)
{
    Serial.printf("\x1b[%0dD", n);
    _col -= n;
    if (_col < 1)
    {
        _col = 1;
    }
}

void os::_cursor_right(int n)
{
    Serial.printf("\x1b[%0dC", n);
    _col += n;
}

void os::_cursor_up(int n)
{
    Serial.printf("\x1b[%0dA", n);
}

void os::_cursor_down(int n)
{
    Serial.printf("\x1b[%0dB", n);
}

void os::_bell()
{
    Serial.print(F("\x1b[\007m"));
}

void os::_erase_line()
{
    Serial.print(F("\x1b[0K"));
}

void os::_clear()
{
    Serial.print(F("\x1b[2J\x1b[1d\x1b[1G"));
}