#include "ui.h"

#include "../rt/rt.h"
#include "../config.h"

int ui_intensity;
ui_led_matrix ui_matrix(/* DIN */ CONFIG_LED_DIN, /* CS */ CONFIG_LED_CS, /* CLK */ CONFIG_LED_CLK, CONFIG_LED_COUNT);
ui_screen *ui_current_screen;
ui_transition ui_current_transition;
int ui_animation_step;
bool ui_needs_update;

void ui_task(void* arg);

void ui_init()
{
    rt_create_task(ui_task);

    ui_needs_update = true;
    ui_animation_step = 0;

    ui_intensity = 1;
    ui_matrix.init();
    ui_matrix.intensity(ui_intensity);
    ui_matrix.clear();
    ui_matrix.sync();
    ui_matrix.swap();
}

void ui_set_screen(ui_screen &screen, ui_transition transition)
{
    ui_current_screen = &screen;

    ui_current_screen->init();
    ui_current_transition = transition;
    ui_animation_step = 0;
    ui_needs_update = true;

    //ui_matrix.clear();
    //int delay_ms;
    // ui_current_screen->draw(ui_matrix);

    if (transition == UI_TRANSITION_NONE)
    {
        ui_matrix.sync();
        ui_matrix.swap();
    }
}

void ui_invalidate()
{
    ui_needs_update = true;
}

void ui_render_transition()
{
    if (ui_current_transition == UI_TRANSITION_NONE)
    {
        ui_needs_update = true;
        return;
    }

    if (ui_animation_step == 0)
    {
        rt_log(F("ui: initial redraw"));

        ui_matrix.clear();
        int delay_ms;
        ui_current_screen->draw(ui_matrix, delay_ms);
    }

    ui_animation_step++;
    if (ui_matrix.is_completed(ui_current_transition, ui_animation_step))
    {
        ui_matrix.sync();
        ui_matrix.swap();
        ui_animation_step = 0;

        ui_current_transition = UI_TRANSITION_NONE;

        rt_log(F("ui: end of transition"));
        ui_needs_update = true;
    }
    else
    {
        ui_matrix.sync(ui_current_transition, ui_animation_step);
        rt_set_delay(CONFIG_LED_ANIMATION_STEP);
    }
}

void ui_task(void* arg)
{
    ui_render_transition();

    if (!ui_needs_update || ui_current_screen == nullptr)
    {
        return;
    }

    int delay_ms = 0;

    ui_matrix.clear();
    ui_current_screen->draw(ui_matrix, delay_ms);
    ui_matrix.sync();
    ui_matrix.swap();

    if (delay_ms > 0)
    {
        rt_set_delay(delay_ms);
    }
    else
    {
        ui_needs_update = false;
    }
}

void ui_set_intensity(int value)
{
    if (ui_intensity != value)
    {
        ui_intensity = value;
        ui_matrix.intensity(ui_intensity);
    }
}