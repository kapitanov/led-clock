#include "terminal_impl.h"

using namespace os;

term_handler os::_handler;
const size_t os::BUFFER_LEN = 128;
char os::_buffer[BUFFER_LEN + 1] = {};
size_t os::_buffer_index = 0;
thread_id os::_thread_id;

void os::_logo()
{
    println(F("___  ________ _____ _____ "));
    println(F("|  \\/  |_   _|  _  |_   _|"));
    println(F("| .  . | | | | | | | | |  "));
    println(F("| |\\/| | | | | | | | | |  "));
    println(F("| |  | |_| |_\\ \\_/ / | |  "));
    println(F("\\_|  |_/\\___/ \\___/  \\_/  "));
    println();
    println();
}

void os::init(term_handler handler, int baud_rate)
{
    _handler = handler;
    Serial.begin(baud_rate);

    _thread_id = os::create_thread(_thread, "cli");

    println();

    _logo();
}

void os::_execute(const String &args)
{
    int i = args.indexOf(' ');
    String command;
    String command_args;
    if (i > 0)
    {
        command = args.substring(0, i);
        command_args = args.substring(i + 1);
    }
    else
    {
        command = args;
    }

    bool handled = true;
    if (command.equalsIgnoreCase("stats"))
    {
        _cmd_stats();
    }
    else if (command.equalsIgnoreCase("ps"))
    {
        _cmd_ps();
    }
    else if (command.equalsIgnoreCase("restart"))
    {
        _cmd_restart();
    }
    else
    {
        handled = _handler(command, command_args);
    }

    if (!handled)
    {
        print(F(" ERROR! Bad command: \""));
        print(args);
        println(F("\""));
        println(F("Type \"help\" to get help on commands"));
    }
}

void os::_thread()
{
    while (Serial.available())
    {
        char ch = Serial.read();
        switch (ch)
        {
        case '\n':
            continue;
        case '\r':
            println();

            while (_buffer_index > 0 && isSpace(_buffer[_buffer_index - 1]))
            {
                _buffer[_buffer_index - 1] = 0;
                _buffer_index--;
            }

            if (os::_buffer_index > 0)
            {
                _buffer[_buffer_index] = 0;
                String cmd(_buffer);
                _execute(cmd);

                 memset(_buffer, 0, BUFFER_LEN + 1);
                _buffer_index = 0;
            }
            break;

        case 8 /* BS */:
        case 0x7F /* BS */:
            if (_buffer_index > 0)
            {
                _buffer[_buffer_index - 1] = 0;
                _buffer_index--;
            }
            break;

        default:
            if ((!isSpace(ch) || os::_buffer_index > 0) && os::_buffer_index < os::BUFFER_LEN - 1)
            {
                os::_buffer[os::_buffer_index] = ch;
                os::_buffer_index++;
            }
            break;
        }
    }
}
