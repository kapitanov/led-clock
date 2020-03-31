#include "rt.h"

void rt_println();
void rt_println(const __FlashStringHelper *str);
void rt_println(const char *str);
void rt_print(const char *x);
void rt_print(char x);

void rt_print_log_header()
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

    char buffer[64];
    sprintf_P(buffer, (const char*)F("[%02d:%02d:%02d.%03d] "), hour, min, sec, msec);
    rt_print(buffer);
}

void rt_log(const __FlashStringHelper *str)
{
    rt_print_log_header();
    rt_println(str);
}

void rt_log(const String &str)
{
    rt_print_log_header();
    rt_println(str.c_str());
}

void rt_log(const char str[])
{
    rt_print_log_header();
    rt_println(str);
}

void rt_logf(const char *format, ...)
{
    rt_print_log_header();

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
        rt_print(buffer[i]);
    }

    rt_println();
    Serial.flush();

    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void rt_logf(const __FlashStringHelper *format, ...)
{
    rt_print_log_header();

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
        rt_print(buffer[i]);
    }

    rt_println();
    Serial.flush();

    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void rt_println()
{
    Serial.println();
}

void rt_println(const __FlashStringHelper *str)
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

        rt_print((char)c);
        i++;
    }
     Serial.println();
}

void rt_print(const char *str)
{
    int i = 0;
    while (str[i] != 0)
    {
        rt_print(str[i]);
        i++;
    }
}

void rt_println(const char *str)
{
    rt_print(str);
    rt_println();
}

void rt_print(char x)
{
    Serial.print(x);
}