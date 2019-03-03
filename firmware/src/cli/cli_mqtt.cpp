#include "../os/os.h"
#include "cli.h"
#include "../mqtt/mqtt.h"

using namespace os;

void cmd_mqtt_status()
{
    println(F("MQTT status"));
    printf(F("Hostname:    \"%s\"\r\n"), mqtt_get_hostname());
    printf(F("Username:    \"%s\"\r\n"), mqtt_get_username());
    printf(F("Password:    \"%s\"\r\n"), mqtt_get_password());
    printf(F("IsConnected: %s\r\n"), mqtt_is_connected() ? "YES" : "NO");
}

void cmd_mqtt_connect(const String &args)
{
    String hostname, username, password;

    int i = args.indexOf(' ');
    if (i > 0)
    {
        hostname = args.substring(0, i);
        String s = args.substring(i + 1);

        i = s.indexOf(' ');
        if (i > 0)
        {
            username = s.substring(0, i);
            password = s.substring(i + 1);
        }
        else
        {
            username = s;
        }
    }
    else
    {
        hostname = args;
    }

    printf(F("Connecting to MQTT broker \"%s\" as \"%s\"\r\n"), hostname.c_str(), username.c_str());

    mqtt_connect(hostname, username, password);
}

void cmd_mqtt_reset()
{
    mqtt_reset();
    println(F("MQTT connection settings were deleted"));
}

bool cmd_mqtt(const String &args)
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

    if (command.equalsIgnoreCase(""))
    {
        cmd_mqtt_status();
        return true;
    }
    if (command.equalsIgnoreCase("connect"))
    {
        cmd_mqtt_connect(command_args);
        return true;
    }
    if (command.equalsIgnoreCase("reset"))
    {
        cmd_mqtt_reset();
        return true;
    }

    return false;
}
