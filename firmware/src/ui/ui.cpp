#include "ui.h"

#include "../config/config.h"

#include "led/led.h"
#include "../os/os.h"

namespace ui
{
ui_mode mode;
int hour;
int minute;
float temperature;
bool needs_update;
unsigned char anim_step;
os::thread_id thread;
bool should_blink;

int intensity;

led_matrix_t LedMatrix(/* DIN */ CONFIG_LED_DIN, /* CS */ CONFIG_LED_CS, /* CLK */ CONFIG_LED_CLK, CONFIG_LED_COUNT);

void thread_func();
void draw_preloader();
void draw_time();
void draw_weather();
}

void ui_init()
{
    ui::thread = os::create_thread(ui::thread_func, "ui");
    ui::needs_update = true;
    ui::mode = UI_LOADING;
    ui::anim_step = 0;
    ui::intensity = 1;

    ui::LedMatrix.init();
    ui::LedMatrix.intensity(1);
    ui::LedMatrix.clear();
    ui::LedMatrix.sync();
}

ui_mode ui_get_mode()
{
    return ui::mode;
}

void ui_set_mode(ui_mode mode)
{
    ui::mode = mode;
    ui::needs_update = true;
    ui::anim_step = 0;
}

void ui_set_time(int h, int m)
{
    if (h != ui::hour || m != ui::minute)
    {
        ui::hour = h;
        ui::minute = m;
        ui::needs_update = true;

        os::printf("Time updated to %02d:%02d\r\n", h, m);

        int intensity = 7;
        if (h >= 20 || h < 6)
        {
            intensity = 1;
        }
        else if (h >= 18 || h < 9)
        {
            intensity = 4;
        }
        else
        {
            intensity = 7;
        }

        if (ui::intensity != intensity)
        {
            ui::intensity = intensity;
            ui::LedMatrix.intensity(intensity);
            os::printf("LED intensity changed to %d\r\n", intensity);
        }
    }
}

void ui_set_weather(float t)
{
    if (t != ui::temperature)
    {
        ui::temperature = t;
        ui::needs_update = true;

        os::printf("Current weather updated to %f\r\n", t);
    }
}

void ui_blink()
{
    ui::LedMatrix.fill();
    ui::LedMatrix.sync();
    delay(100);
    ui::LedMatrix.clear();
    ui::LedMatrix.sync();
    ui::needs_update = true;
}

void ui::thread_func()
{
    switch (ui::mode)
    {
    case UI_LOADING:
        ui::draw_preloader();
        break;
    case UI_TIME:
        ui::draw_time();
        break;
    case UI_WEATHER:
        ui::draw_weather();
        break;
    }
}

void ui::draw_preloader()
{
    char buff[2] = "";
    buff[0] = 50 + ui::anim_step;

    ui::LedMatrix.clear();
    ui::LedMatrix.text(buff, FONT_SPECIAL, 13, 0);
    ui::LedMatrix.sync();

    ui::anim_step++;
    if (ui::anim_step > 4)
    {
        ui::anim_step = 0;
    }

    os::set_delay(250);

    /*
    char buff[12] = "";
    for (int i = 0; i < 8; i++)
    {
        buff[i] = FONT_SPECIAL_LINE;
    }

    if (ui::anim_step >= 8)
    {
        buff[16 - ui::anim_step + 1] = FONT_SPECIAL_BAR;
    }
    else
    {
        buff[ui::anim_step] = FONT_SPECIAL_BAR;
    }

    ui::LedMatrix.clear();
    ui::LedMatrix.text(buff, FONT_SPECIAL, 0, 0);
    ui::LedMatrix.sync();

    ui::anim_step++;
    if (ui::anim_step >= 15)
    {
        ui::anim_step = 0;
    }

    os::set_delay(CONFIG_LED_ANIMATION_STEP);
    */
}

void ui::draw_time()
{
    if (ui::needs_update)
    {
        char buff[8] = "";
        sprintf(buff, "%2d:%02d", ui::hour, ui::minute);

        ui::LedMatrix.clear();
        ui::LedMatrix.text(buff, FONT_MONOSPACE);
        ui::LedMatrix.sync(TRANSITION_SCROLL_UP, ui::anim_step);

        ui::anim_step++;
        if (ui::anim_step > 8)
        {
            ui::needs_update = false;
            ui::LedMatrix.swap();
        }
        else
        {
            os::set_delay(CONFIG_LED_ANIMATION_STEP);
        }
    }
}

void ui::draw_weather()
{
    if (ui::needs_update)
    {
        char buff[12] = "";
        float t = abs(ui::temperature);
        int hi = ((int)t / 10) % 10;
        int lo = (int)t % 10;
        buff[0] = ui::temperature >= 0 ? '+' : '-';
        buff[1] = hi != 0 ? hi + '0' : ' ';
        buff[2] = lo + '0';
        buff[3] = '\u00B0';
        buff[4] = 'C';
        buff[5] = '\0';

        ui::LedMatrix.clear();
        ui::LedMatrix.text(buff);
        ui::LedMatrix.sync(TRANSITION_SCROLL_DOWN, ui::anim_step);

        ui::anim_step++;
        if (ui::anim_step > 8)
        {
            ui::needs_update = false;
            ui::LedMatrix.swap();
        }
        else
        {
            os::set_delay(CONFIG_LED_ANIMATION_STEP);
        }
    }
}
