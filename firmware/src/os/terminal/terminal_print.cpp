#include "terminal_impl.h"

using namespace os;

void os::attr(attr_t c)
{
    os::lock lock;

    _attr(c);
}

void os::attr(attr_t c1, attr_t c2)
{
    os::lock lock;

    _attr(c1, c2);
}

void os::attr(attr_t c1, attr_t c2, attr_t c3)
{
    os::lock lock;

    _attr(c1, c2, c3);
}

void os::cursor_col(int n)
{
    os::lock lock;

    _cursor_col(n);
}

void os::cursor_left(int n)
{
    os::lock lock;

    _cursor_left(n);
}

void os::cursor_right(int n)
{
    os::lock lock;

    _cursor_right(n);
}

void os::cursor_up(int n)
{
    os::lock lock;

    _cursor_up(n);
}

void os::cursor_down(int n)
{
    os::lock lock;

    _cursor_down(n);
}

void os::bell()
{
    os::lock lock;

    _bell();
}

void os::println()
{
    os::lock lock;

    _println();
}

void os::println(const __FlashStringHelper *str)
{
    os::lock lock;

    _println(str);
}

void os::println(const String &str)
{
    os::lock lock;

    _println(str);
}

void os::println(const char str[])
{
    os::lock lock;

    _println(str);
}

void os::println(char x)
{
    os::lock lock;

    _print(x);
    _println();
}

void os::print(const __FlashStringHelper *str)
{
    os::lock lock;

    _print(str);
}

void os::print(const String &str)
{
    os::lock lock;

    _print(str);
}

void os::print(const char str[])
{
    os::lock lock;

    _print(str);
}

void os::print(char x)
{
    os::lock lock;

    _print(x);
}

void os::printf(const char *format, ...)
{
    os::lock lock;

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

void os::printf(const __FlashStringHelper *format, ...)
{
    os::lock lock;

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

void os::erase_line()
{
    os::lock lock;

    _erase_line();
}

void os::clear()
{
    os::lock lock;

    _clear();
}
