#pragma once

#include <Arduino.h>

namespace os
{
typedef uint8_t thread_id;
const thread_id UNKNOWN_THREAD = 0xFF;

typedef void (*thread_func)();

thread_id create_thread(thread_func func, const char name[8]);
thread_id current_thread_id();

void set_func(thread_id id, thread_func func, const char name[8]);
void set_func(thread_func func, const char name[8]);
void set_delay(int32_t ms);

void run();
}