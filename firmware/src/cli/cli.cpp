#include "../os/os.h"
#include "cli.h"
#include "cli_commands.h"

using namespace os;

typedef bool (*cmd_handler)(const String &args);

bool cli_handler(const String &command, const String &args)
{
    cmd_handler handler = NULL;

    if (command.equalsIgnoreCase("help"))
    {
        handler = cmd_help;
    }
    else if (command.equalsIgnoreCase("wifi"))
    {
        handler = cmd_wifi;
    }
    else if (command.equalsIgnoreCase("mqtt"))
    {
        handler = cmd_mqtt;
    }    
    else if (command.equalsIgnoreCase("state"))
    {
        handler = cmd_state;
    }

    return (handler != NULL && handler(args));
}
