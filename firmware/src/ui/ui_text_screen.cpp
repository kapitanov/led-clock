#include "ui.h"
#include "../rt/rt.h"

ui_text_screen::ui_text_screen() : _str(nullptr)
{
}

void ui_text_screen::init()
{
    rt_logf(F("ui: activated text_screen \"%s\""), _str);
    _active = true;
    _needs_update = true;
    _offset = 0;
    _width = 0;
    _time = millis();
}

void ui_text_screen::set(const char *str)
{
    if (_str != nullptr)
    {
        delete[] _str;
    }

    _len = strlen(str);
    _str = new char[_len + 1];
    memcpy(_str, str, _len + 1);

    _offset = 0;
    _width = 0;
    _active = true;
    _needs_update = true;

    ui_invalidate();
}

void ui_text_screen::on_completed(ui_screen_callback func, void *arg)
{
    _on_completed = func;
    _on_completed_arg = arg;
}

void ui_text_screen::draw(ui_led_matrix &matrix, int &delay_ms)
{
    if (!_active)
    {
        matrix.text(_str, UI_FONT_MONOSPACE, -_offset, 0);
        delay_ms = 100;
        return;
    }

    if (_needs_update)
    {
        _offset = 0;
        _needs_update = false;

        _width = matrix.measure_text(_str, UI_FONT_MONOSPACE);
    }
    else
    {
        if (_width > matrix.width())
        {
            _offset += 2;
            if (_offset >= _width)
            {
                complete();
                delay_ms = 0;
                return;
            }
        }
        else
        {
            auto t = millis();
            if (t - _time >= 2500)
            {
                complete();
                delay_ms = 0;
                return;
            }
        }
    }

    matrix.text(_str, UI_FONT_MONOSPACE, -_offset, 0);
    delay_ms = 50;
}

void ui_text_screen::complete()
{
    rt_logf(F("ui: completed text_screen \"%s\""), _str);
    _offset = 0;
    _active = false;

    if (_on_completed != nullptr)
    {
        _on_completed(_on_completed_arg);
    }
}
