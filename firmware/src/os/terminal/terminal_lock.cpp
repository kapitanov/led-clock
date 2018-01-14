#include "terminal_impl.h"

using namespace os;

os::thread_id os::_thread_id;
os::thread_id os::_lock_thread_id;
int os::_lock_depth;

os::lock::lock()
{
    os::lock_term();
}

os::lock::~lock()
{
    os::release_term();
}

void os::lock_term()
{
    os::thread_id t = os::current_thread_id();
    if (t == os::_thread_id)
    {
        return;
    }

    if (t == os::_lock_thread_id)
    {
        os::_lock_depth++;
    }
    else
    {
        os::_lock_thread_id = t;
        os::_lock_depth = 1;

        os::cursor_col();
        os::erase_line();
    }
}

void os::release_term()
{
    os::thread_id t = os::current_thread_id();
    if (t == os::_thread_id)
    {
        return;
    }

    if (t == os::_lock_thread_id)
    {
        os::_lock_depth--;
        if (os::_lock_depth <= 0)
        {
            os::_lock_thread_id = os::UNKNOWN_THREAD;

            if (os::_col > 1)
            {
                _println();
            }

            os::_prompt();
        }
    }
}