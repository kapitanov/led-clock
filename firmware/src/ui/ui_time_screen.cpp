#include "ui.h"
#include "../rt/rt.h"

void ui_time_screen::init()
{
    rt_log(F("ui: activated time_screen"));
    _step = 0;
}

void ui_time_screen::set(int h, int m)
{
    _h = h;
    _m = m;
    ui_invalidate();
}

void ui_time_screen::draw(ui_led_matrix &matrix, int &delay_ms)
{
    char buff[8] = "";
    sprintf(buff, "%02d%c%02d", _h, _step % 2 == 0 ? ':' : ' ', _m);
    if (buff[0] == '0')
    {
        buff[0] = 100;
    }

    matrix.text(buff, UI_FONT_CLOCK, 0, 0);

    _step++;
    delay_ms = 500;
}