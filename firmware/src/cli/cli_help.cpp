#include "cli.h"
#include "os/os.h"

using namespace os;

bool cmd_help(const String &args)
{
    attr(BLACK, BG_CYAN);
    print(F("Available commands"));
    attr(RESET);
    println(); 

    attr(YELLOW);
    println(F("WiFi commands"));
    attr(RESET);
    println(F("wifi            Print WiFI connection status "));
    println(F("wifi ls         List available WiFi networks"));
    println(F("wifi up         Connect to Wifi"));
    println(F("                Usage:"));
    println(F("                wifi up <SSID> [<KEY>]"));
    println(F("wifi down       Disconnect from WiFi and erase connection settings"));
    println(F("wifi test       Check Internet connection"));
    println(F("wifi diag       Print WiFi diagnostics"));
    // attr(YELLOW);
    // println(F("Indicator commands"));
    // attr(RESET);    
    // println(F("state           Print current indicator state"));
    attr(YELLOW);
    println(F("MQTT commands"));
    attr(RESET);
    println(F("mqtt            Print MQTT connection status"));
    println(F("mqtt reset      Disconnect from MQTT broker and erase connection settings"));
    println(F("mqtt connect    Connect to MQTT broker"));
    println(F("                Usage:"));
    println(F("                mqtt connect <HOST> <USERNAME> <PASSWORD>"));
    attr(YELLOW);
    println(F("Common commands"));
    attr(RESET);
    println(F("help            Print commands help"));
    println(F("cls             Clear screen"));
    println(F("stats           Print OS stats"));
    println(F("restart         Reboot"));

    return true;
}
