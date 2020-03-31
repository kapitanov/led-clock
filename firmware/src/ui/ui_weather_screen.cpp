#include "ui.h"
#include "../rt/rt.h"

void ui_weather_screen::init()
{
    rt_log(F("ui: activated weather_screen"));
}

void ui_weather_screen::set(float t)
{
    _t = t;
    ui_invalidate();
}

void ui_weather_screen::draw(ui_led_matrix &matrix, int &delay_ms)
{
    char buff[12] = "";
    float t = abs(_t);
    int hi = ((int)t / 10) % 10;
    int lo = (int)t % 10;
    buff[0] = _t >= 0 ? '+' : '-';
    buff[1] = hi != 0 ? hi + '0' : ' ';
    buff[2] = lo + '0';
    buff[3] = 0xB0;
    buff[4] = 'C';
    buff[5] = 0;

    int width = matrix.measure_text(buff, UI_FONT_DEFAULT);
    int x = matrix.width() - width;

    matrix.text(buff, UI_FONT_DEFAULT, x, 0);
}