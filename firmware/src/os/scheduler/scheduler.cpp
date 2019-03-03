#include "scheduler_impl.h"
#include "../terminal/terminal.h"

using namespace os;

namespace os
{

enum thread_state_t
{
    NONE,
    ACTIVE,
    SLEEP,
};

const size_t NAME_LEN = 8;

struct thread
{
    thread_id id;
    thread_state_t state;
    thread_func func;
    int32_t delay;
    int32_t time;
    char name[NAME_LEN + 1];
    char func_name[NAME_LEN + 1];
};

const size_t MAX_THREADS = 16;
thread threads[MAX_THREADS] = {};
thread *current_thread;

void schedule_task(os::thread &thread);
void safe_strcpy(char *dest, const char *str, size_t len);
}

void os::safe_strcpy(char *dest, const char *src, size_t len)
{
    unsigned int i = 0;
    for (; i < len && src[i] != 0; i++)
    {
        dest[i] = src[i];
    }

    if (i == len)
    {
        dest[i] = '~';
    }
    else
    {
        dest[i] = 0;
    }
    dest[i + 1] = 0;
}

os::thread_id os::create_thread(os::thread_func func, const char name[8])
{
    for (size_t i = 0; i < os::MAX_THREADS; i++)
    {
        if (os::threads[i].state == os::NONE)
        {
            os::threads[i].id = i + 1;
            os::threads[i].state = os::ACTIVE;
            os::threads[i].func = func;
            os::threads[i].delay = 0;
            os::threads[i].time = 0;
            safe_strcpy(os::threads[i].name, name, os::NAME_LEN);
            safe_strcpy(os::threads[i].func_name, "", os::NAME_LEN);
            return os::threads[i].id;
        }
    }

    os::println(F("[  ERROR  ] os::create_thread(): no free thread slots"));

    return os::UNKNOWN_THREAD;
}

os::thread_id os::current_thread_id()
{
    if (os::current_thread == NULL)
    {
        return os::UNKNOWN_THREAD;
    }

    return os::current_thread->id;
}

void os::set_func(os::thread_id id, os::thread_func func, const char name[8])
{
    if (id <= 0 || id >= MAX_THREADS - 1)
    {
        return;
    }

    threads[id - 1].func = func;
    os::safe_strcpy(threads[id - 1].func_name, name, os::NAME_LEN);
}

void os::set_func(os::thread_func func, const char name[8])
{
    if (os::current_thread == NULL)
    {
        os::println(F("[ ERROR ] os::set_func(): no context"));
        return;
    }

    os::current_thread->func = func;
    os::safe_strcpy(os::current_thread->func_name, name, os::NAME_LEN);
}

void os::set_delay(int32_t ms)
{
    if (os::current_thread == NULL)
    {
        os::println(F("[ ERROR ] os::set_delay(): no context"));
        return;
    }

    os::current_thread->state = os::SLEEP;
    os::current_thread->delay = ms;
    os::current_thread->time = millis();
}

void os::schedule_task(os::thread &thread)
{
    os::current_thread = &thread;
    switch (thread.state)
    {
    case os::ACTIVE:
        thread.func();
        break;

    case os::SLEEP:
        int32_t time = millis();
        int32_t delta = time - thread.time;
        thread.delay -= delta;
        thread.time = time;

        if (thread.delay <= 0)
        {
            thread.state = os::ACTIVE;
            thread.delay = 0;
            thread.time = 0;
            thread.func();
        }
        break;
    }

    os::current_thread = NULL;
}

void os::run()
{
    for (size_t i = 0; i < os::MAX_THREADS; i++)
    {
        os::schedule_task(os::threads[i]);
    }
}

void os::_dump_threads()
{
    os::println(F("#  ID   NAME       STATE      ENTRYPOINT DELAY      FUNCTION  "));

    for (size_t i = 0; i < os::MAX_THREADS; i++)
    {
        const char *state = "?";
        switch (os::threads[i].state)
        {
        case os::NONE:
            state = "NONE";
            break;
        case os::ACTIVE:
            state = "ACTIVE";
            break;
        case os::SLEEP:
            state = "SLEEP";
            break;
        }

        os::printf(F("%02d 0x%02X %-10s %-10s 0x%08X %-10d %-10s\r\n"), i, os::threads[i].id, os::threads[i].name, state, os::threads[i].func, os::threads[i].delay, os::threads[i].func_name);
    }
}