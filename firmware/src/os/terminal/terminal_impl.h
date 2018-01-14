#pragma once

#include "../scheduler/scheduler.h"
#include "terminal.h"

namespace os
{
extern term_handler _handler;
extern const size_t BUFFER_LEN;
extern char _buffer[];
extern int _buffer_index;

extern thread_id _thread_id;
extern thread_id _lock_thread_id;
extern int _lock_depth;

extern int _col;

void _logo();
void _prompt();
void _thread();
void _execute(const String& command);
void _clear_buffer();

void _attr(attr_t a);
void _attr(attr_t a, attr_t b);
void _attr(attr_t a, attr_t b, attr_t c);

void _print(const char c);
void _print(const char* str);
void _print(const String& str);
void _print(const __FlashStringHelper* str);

void _println();
void _println(const char* str);
void _println(const String& str);
void _println(const __FlashStringHelper* str);

void _printf(const char *format, ...);
void _printf(const __FlashStringHelper *format, ...);

void _bell();
void _cursor_col(int n = 1);
void _cursor_left(int n = 1);
void _cursor_right(int n = 1);
void _cursor_up(int n = 1);
void _cursor_down(int n = 1);

void _erase_line();
void _clear();

void _cmd_cls();
void _cmd_ps();
void _cmd_restart();
void _cmd_stats();
}