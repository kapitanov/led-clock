#pragma once

#include "../scheduler/scheduler.h"
#include "terminal.h"

namespace os
{
extern term_handler _handler;
extern const size_t BUFFER_LEN;
extern char _buffer[];
extern size_t _buffer_index;

extern thread_id _thread_id;

void _logo();
void _thread();
void _execute(const String& command);

void _cmd_ps();
void _cmd_restart();
void _cmd_stats();
}