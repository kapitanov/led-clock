#include "../os/os.h"
#include "cli.h"
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>

using namespace os;

const char *nameof(wl_status_t status)
{
    switch (status)
    {
    case WL_NO_SHIELD:
        return "WL_NO_SHIELD";
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    default:
        return "WL_?";
    }
}

const char *nameof(WiFiMode_t mode)
{
    switch (mode)
    {
    case WIFI_OFF:
        return "WIFI_OFF";
    case WIFI_STA:
        return "WIFI_STA";
    case WIFI_AP:
        return "WIFI_AP";
    case WIFI_AP_STA:
        return "WIFI_AP_STA";
    default:
        return "WIFI_?";
    }
}

const unsigned long WIFI_CONNECT_TIMEOUT = 30 * 1000;

#define WIFI_TEST_ENDPOINT "http://httpbin.org/get"
#define DNS_TEST_HOSTNAME "httpbin.org"

void cmd_wifi_status()
{
    println(F("WiFi status"));
    printf(F("IsConnected: %s\r\n"), WiFi.isConnected() ? "YES" : "NO");
    printf(F("Mode:        %s\r\n"), nameof(WiFi.getMode()));
    printf(F("Status:      %s\r\n"), nameof(WiFi.status()));
    printf(F("SSID:        %s\r\n"), WiFi.SSID().c_str());
    printf(F("LocalIP:     %s\r\n"), WiFi.localIP().toString().c_str());
    printf(F("GatewayIP:   %s\r\n"), WiFi.gatewayIP().toString().c_str());
    printf(F("DnsIP:       %s\r\n"), WiFi.dnsIP().toString().c_str());
    printf(F("Hostname:    %s\r\n"), WiFi.hostname().c_str());
    printf(F("MAC Address: %s\r\n"), WiFi.macAddress().c_str());
}

void cmd_wifi_diag()
{
    println(F("WiFi diagnostics"));
    WiFi.printDiag(Serial);
    println();
}

void cmd_wifi_ls()
{
    println(F("Scanning WiFi networks... "));
    int n = WiFi.scanNetworks();

    println(F("Available WiFi networks:"));

    for (int i = 0; i < n; i++)
    {
        printf(F("* %s\r\n"), WiFi.SSID(i).c_str());
    }

    printf(F("Total %d networks\r\n"), n);
    println();
}

void cmd_wifi_up(const String &args)
{
    int i = args.indexOf(' ');
    String ssid;
    String pass;
    if (i > 0)
    {
        ssid = args.substring(0, i);
        pass = args.substring(i + 1);
    }
    else
    {
        ssid = args;
    }

    printf(F("Connecting to WiFi network %s... "), ssid.c_str());

    wl_status_t status = WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();

    while (status != WL_CONNECTED && status != WL_CONNECT_FAILED)
    {
        if (millis() > start + WIFI_CONNECT_TIMEOUT)
        {
            println(F("Failed to connect (timed out)"));
            println();
            return;
        }

        status = WiFi.status();
        delay(100);
    }

    if (status != WL_CONNECTED)
    {
        printf(F("Failed to connect (%s)"), nameof(status));
        println();
        return;
    }

    WiFi.setAutoReconnect(true);
    WiFi.setAutoConnect(true);

    printf(F("Connected to %s (IP %s)"), ssid.c_str(), WiFi.localIP().toString().c_str());
    println();
}

void cmd_wifi_down()
{
    if (!WiFi.isConnected())
    {
        println(F("Not connected to WiFi"));
        println();
        return;
    }

    String ssid = WiFi.SSID();
    printf(F("Disconnecting from WiFi network %s...\r\n"), ssid.c_str());

    WiFi.disconnect();
    WiFi.setAutoReconnect(false);
    WiFi.setAutoConnect(false);

    printf(F("Disconnected from WiFi network %s"), ssid.c_str());
    println();
}

void cmd_wifi_test()
{
    printf(F("DNS lookup for %s...\r\n"), DNS_TEST_HOSTNAME);
    IPAddress ip;
    if (WiFi.hostByName(DNS_TEST_HOSTNAME, ip))
    {
        printf(F("OK, IP is %s"), ip.toString().c_str());
        println();
    }
    else
    {
        println(F("Error!"));
    }

    printf(F("GET %s..."), WIFI_TEST_ENDPOINT);

    HTTPClient http;
    http.begin(WIFI_TEST_ENDPOINT);
    int httpCode = http.GET();

    if (httpCode >= 200 && httpCode < 400)
    {
        println(F("Success!"));
    }
    else
    {
        println(F("Failure! "));
    }

    printf(F("HTTP status: %d"), httpCode);
    println();
}

bool cmd_wifi(const String &args)
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
        cmd_wifi_status();
        return true;
    }

    if (command.equalsIgnoreCase("diag"))
    {
        cmd_wifi_diag();
        return true;
    }

    if (command.equalsIgnoreCase("ls"))
    {
        cmd_wifi_ls();
        return true;
    }

    if (command.equalsIgnoreCase("up"))
    {
        cmd_wifi_up(command_args);
        return true;
    }

    if (command.equalsIgnoreCase("down"))
    {
        cmd_wifi_down();
        return true;
    }

    if (command.equalsIgnoreCase("test"))
    {
        cmd_wifi_test();
        return true;
    }

    return false;
};