#include "terminal_impl.h"

using namespace os;

void os::println()
{
    print('\r');
    print('\n');
}

void os::println(const __FlashStringHelper *str)
{
    print(str);
    println();
}

void os::println(const String &str)
{
    print(str);
    println();
}

void os::println(const char str[])
{
    print(str);
    println();
}

void os::println(char x)
{
    print(x);
    println();
}

void os::print(const __FlashStringHelper *str)
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

        print((char)c);
        i++;
    }
}

void os::print(const String &str)
{
    for (unsigned int i = 0; i < str.length(); i++)
    {
        print(str.charAt(i));
    }
}

void os::print(const char str[])
{
    int i = 0;
    while (str[i] != 0)
    {
        print(str[i]);
        i++;
    }
}

void os::print(char x)
{
    Serial.print(x);
}

void os::printf(const char *format, ...)
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

    for (unsigned int i = 0; i < len; i++)
    {
        print(buffer[i]);
    }

    Serial.flush();

    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void os::printf(const __FlashStringHelper *format, ...)
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

    for (unsigned int i = 0; i < len; i++)
    {
        print(buffer[i]);
    }
    Serial.flush();

    if (buffer != temp)
    {
        delete[] buffer;
    }
}