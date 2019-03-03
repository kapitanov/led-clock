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

const int MAX_ANIM_STEPS = 8;

int intensity;

led_matrix_t LedMatrix(/* DIN */ CONFIG_LED_DIN, /* CS */ CONFIG_LED_CS, /* CLK */ CONFIG_LED_CLK, CONFIG_LED_COUNT);

void thread_func();
void draw_preloader();
void draw_time();
void draw_weather();
void print_buffers();
} // namespace ui

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
    ui::LedMatrix.swap();
}

ui_mode ui_get_mode()
{
    return ui::mode;
}

void ui_set_mode(ui_mode mode)
{
    if (ui::mode == UI_LOADING && mode != UI_LOADING)
    {
        os::logf(F("ui_set_mode(): clearing view"));
        ui::LedMatrix.clear();
    }
    ui::mode = mode;
    ui::needs_update = true;
    ui::anim_step = 0;

    os::logf(F("ui_set_mode(): state: 0x%02X, needs_update: %c, anim_step: %d"), mode, ui::needs_update ? 'Y' : 'N', ui::anim_step);
}

void ui_set_time(int h, int m)
{
    if (h != ui::hour || m != ui::minute)
    {
        os::logf("Time updated to %02d:%02d", h, m);

        ui::hour = h;
        ui::minute = m;
        ui::needs_update = true;

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
            os::logf("LED intensity changed to %d", intensity);
        }
    }
}

void ui_set_weather(float t)
{
    if (t != ui::temperature)
    {
        os::logf("Weather updated to %f", t);
        ui::temperature = t;
        ui::needs_update = true;
    }
}

void ui_blink()
{
    ui::LedMatrix.fill();
    ui::LedMatrix.sync();
    ui::LedMatrix.swap();
    delay(100);
    ui::LedMatrix.clear();
    ui::LedMatrix.sync();
    ui::LedMatrix.swap();
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
    ui::LedMatrix.swap();

    ui::anim_step++;
    if (ui::anim_step > 4)
    {
        ui::anim_step = 0;
    }

    os::set_delay(250);
}

void ui::draw_time()
{
    if (ui::needs_update)
    {
        char buff[8] = "";
        sprintf(buff, "%2d:%02d", ui::hour, ui::minute);

        ui::LedMatrix.clear();
        ui::LedMatrix.text(buff, FONT_MONOSPACE);

        ui::anim_step++;
        if (ui::anim_step >= MAX_ANIM_STEPS)
        {
            ui::needs_update = false;
            ui::LedMatrix.sync();
            ui::LedMatrix.swap();
            ui::anim_step = 0;
        }
        else
        {
            ui::LedMatrix.sync(TRANSITION_SCROLL_UP, ui::anim_step % MAX_ANIM_STEPS);
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
        buff[3] = 0xB0;
        buff[4] = 'C';
        buff[5] = 0;

        ui::LedMatrix.clear();
        ui::LedMatrix.text(buff);

        ui::anim_step++;
        if (ui::anim_step >= MAX_ANIM_STEPS)
        {
            ui::needs_update = false;
            ui::LedMatrix.sync();
            ui::LedMatrix.swap();
            ui::anim_step = 0;
        }
        else
        {
            ui::LedMatrix.sync(TRANSITION_SCROLL_DOWN, ui::anim_step % MAX_ANIM_STEPS);
            os::set_delay(CONFIG_LED_ANIMATION_STEP);
        }
    }
}

void ui::print_buffers()
{
    for (int y = 0; y < ui::LedMatrix.height(); y++)
    {
        for (int x = 0; x < ui::LedMatrix.width(); x++)
        {
            if (ui::LedMatrix.get_front_buffer(x, y))
            {
                os::print('X');
            }
            else
            {
                os::print('-');
            }
        }

        os::print(F("    "));

        for (int x = 0; x < ui::LedMatrix.width(); x++)
        {
            if (ui::LedMatrix.get(x, y))
            {
                os::print('X');
            }
            else
            {
                os::print('-');
            }
        }
        os::println();
    }
    os::println();
}

void ui_print_state()
{
    os::println(F("Cuurent display state"));
    os::println(F("====================="));
    os::println();

    os::println(F("Front buffer                        Back buffer"));
    ui::print_buffers();

    os::printf(F("mode:         0x%02X\r\n"), ui::mode);
    os::printf(F("hour:         %d\r\n"), ui::hour);
    os::printf(F("minute:       %d\r\n"), ui::minute);
    os::printf(F("temperature:  %f\r\n"), ui::temperature);
    os::printf(F("needs_update: %c\r\n"), ui::needs_update ? 'Y' : 'N');
    os::printf(F("anim_step:    %d\r\n"), ui::anim_step);
    os::println();
}
