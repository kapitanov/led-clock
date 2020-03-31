#include "rt.h"
#include <JC_Button.h>

void rt_button_task(void *arg);

rt_button::rt_button(
    rt_button_event_handler handler,
    int pin,
    bool pull_up,
    bool invert,
    int debounce_ms,
    int long_press_ms)
    : _button(pin, pull_up, invert, debounce_ms),
      _handler(handler),
      _invert(invert),
      _long_press_ms(long_press_ms)
{
    _button.begin();
    _task = rt_create_task(rt_button_task, this);
}

rt_button::~rt_button()
{
    rt_destroy_task(_task);
}

void rt_button::update()
{
    _button.read();

    if (_button.wasReleased())
    {
        _handler(RT_BUTTON_CLICK);
    }
    else
    {
        bool longPress;
        if (_invert)
        {
            longPress = _button.releasedFor(_long_press_ms);
        }
        else
        {
            longPress = _button.pressedFor(_long_press_ms);
        }
        if (longPress)
        {
            _handler(RT_BUTTON_LONG_CLICK);
        }
    }
}

void rt_button_task(void *arg)
{
    ((rt_button *)arg)->update();
}