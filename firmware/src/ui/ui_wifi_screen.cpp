#include "ui.h"
#include "../rt/rt.h"

void ui_wifi_screen::init()
{
    rt_log(F("ui: ui_wifi_screen wait_screen"));
    _step = 0;
}

void ui_wifi_screen::draw(ui_led_matrix &matrix, int &delay_ms)
{
    char buff[2] = "";
    buff[0] = 60 + _step;

    matrix.text(buff, UI_FONT_SPECIAL, 13, 0);

    _step++;
    if (_step > 3)
    {
        _step = 0;
    }

    delay_ms = 250;
}