#include "terminal_impl.h"

using namespace os;

term_handler os::_handler;
const size_t os::BUFFER_LEN = 128;
char os::_buffer[BUFFER_LEN + 1] = {};
int os::_buffer_index = 0;

void os::_logo()
{
    _attr(GREEN);
    _println(F("___  ________ _____ _____ "));
    _println(F("|  \\/  |_   _|  _  |_   _|"));
    _println(F("| .  . | | | | | | | | |  "));
    _println(F("| |\\/| | | | | | | | | |  "));
    _println(F("| |  | |_| |_\\ \\_/ / | |  "));
    _println(F("\\_|  |_/\\___/ \\___/  \\_/  "));
    _println();
    _attr(RESET);
    _println();
}

void os::_clear_buffer()
{
    memset(_buffer, 0, BUFFER_LEN + 1);
    _buffer_index = 0;
}

void os::_prompt()
{
    _attr(YELLOW);
    _attr(BG_NORMAL);
    _print('>');
    _attr(RESET);
    _print(' ');

    for (int i = 0; i < _buffer_index; i++)
    {
        _print(_buffer[i]);
    }
}

void os::init(term_handler handler, int baud_rate)
{
    _handler = handler;
    Serial.begin(baud_rate);

    _clear_buffer();

    _thread_id = os::create_thread(_thread, "cli");

    _attr(os::RESET);
    _println();
    _println();

    _logo();
    _prompt();
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
    if (command.equalsIgnoreCase("cls"))
    {
        _cmd_cls();
    }
    else if (command.equalsIgnoreCase("stats"))
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
        attr(WHITE, BG_RED);
        print(F(" ERROR! "));
        attr(RESET);
        print(F(" Bad command: "));
        attr(RED);
        print(args);
        attr(RESET);
        println();
        print(F("Type "));
        attr(INVERSE);
        print(F("help"));
        attr(RESET);
        println(F(" to get help on commands"));
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
            _println();

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
            }

            _clear_buffer();
            _prompt();

            break;

        case 8 /* BS */:
        case 0x7F /* BS */:
            if (_buffer_index > 0)
            {
                _buffer[_buffer_index - 1] = 0;
                _buffer_index--;

                os::_cursor_left();
                os::_erase_line();
            }
            break;

        default:
            if (!isPrintable(ch))
            {
                os::_bell();
                break;
            }

            if ((!isSpace(ch) || os::_buffer_index > 0) && os::_buffer_index < os::BUFFER_LEN - 1)
            {
                os::_buffer[os::_buffer_index] = ch;
                os::_buffer_index++;

                _print(ch);
            }
            break;
        }
    }
}
