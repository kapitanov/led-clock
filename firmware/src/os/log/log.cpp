#include "log.h"
#include "../terminal/terminal.h"

using namespace os;

namespace os
{
void print_log_header()
{
    uint32_t cycles = ESP.getCycleCount();
    uint8_t freq = ESP.getCpuFreqMHz();
    uint32_t msec = cycles / 1000;
    msec /= freq;
    uint32_t sec = msec / 1000;
    msec = msec % 1000;
    uint32_t min = sec / 60;
    sec = sec % 60;
    uint32_t hour = min / 60;
    min = min % 60;

    printf(F("[%02d:%02d:%02d.%03d] "), hour, min, sec, msec);
}

void log(const __FlashStringHelper *str)
{
    print_log_header();
    println(str);
}

void log(const String &str)
{
    print_log_header();
    println(str);
}

void log(const char str[])
{
    print_log_header();
    println(str);
}

void logf(const char *format, ...)
{
    print_log_header();

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

    println();
}

void logf(const __FlashStringHelper *format, ...)
{
    print_log_header();
    
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

    println();
}
} // namespace os