#include <Button.h>

#include "btn.h"
#include "../config/config.h"
#include "../os/os.h"

namespace btn
{
Button button(CONFIG_BUTTON_PIN, CONFIG_BUTTON_PULLUP, CONFIG_BUTTON_INVERT, CONFIG_BUTTON_DEBOUNCE_MS);
btn_handler handler;
os::thread_id thread;
void thread_func();
}

void btn_init(btn_handler handler)
{
    btn::handler = handler;
    btn::thread = os::create_thread(btn::thread_func, "btn");
}

void btn::thread_func()
{
    btn::button.read();

    if (btn::button.wasReleased())
    {
        btn::handler();
    }
}