#include "cli.h"
#include "os/os.h"

using namespace os;

bool cmd_help(const String &args)
{
    println(F("Available commands"));
    println(F("=================="));
    println(F("WiFi commands"));
    println(F("    wifi            Print WiFI connection status "));
    println(F("    wifi ls         List available WiFi networks"));
    println(F("    wifi up         Connect to Wifi"));
    println(F("                    Usage:"));
    println(F("                    wifi up <SSID> [<KEY>]"));
    println(F("    wifi down       Disconnect from WiFi and erase connection settings"));
    println(F("    wifi test       Check Internet connection"));
    println(F("    wifi diag       Print WiFi diagnostics"));
    println(F("Indicator commands"));
    println(F("    state           Print current indicator state"));
    println(F("MQTT commands"));
    println(F("    mqtt            Print MQTT connection status"));
    println(F("    mqtt reset      Disconnect from MQTT broker and erase connection settings"));
    println(F("    mqtt connect    Connect to MQTT broker"));
    println(F("                    Usage:"));
    println(F("                    mqtt connect <HOST> <USERNAME> <PASSWORD>"));    
    println(F("Common commands"));
    println(F("    help            Print commands help"));
    println(F("    cls             Clear screen"));
    println(F("    stats           Print OS stats"));
    println(F("    restart         Reboot"));

    return true;
}
