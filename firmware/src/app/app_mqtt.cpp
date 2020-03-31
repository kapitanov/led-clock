#include "app.h"

void app_on_mqtt_receive(const rt_mqtt_event &event)
{
    const JsonDocument &json = *event.json;

    if (strcmp(event.topic, "/time/update") == 0)
    {
        // {"h":0,"m":12,"s":34}

        app_time_data data;
        data.h = json["h"];
        data.m = json["m"];

        app_set_time_data(data);
        return;
    }

    if (strcmp(event.topic, "/weather/update") == 0)
    {
        // {"now":3,"city":"Zelenograd"}

        app_weather_data data;
        data.now = (int)json["now"];

        app_set_weather_data(data);
        return;
    }

    if (strcmp(event.topic, "/covid19/update") == 0)
    {
        // {"country":"Russia","deaths":8,"recovered":64,"active":1462}

        app_covid_data data;
        data.active = json["active"];
        data.recovered = json["recovered"];
        data.deaths = json["deaths"];

        app_set_covid_data(data);
        return;
    }

    rt_logf(F("app: got unknown event: %s"), event.topic);
}

void app_on_mqtt_event(const rt_mqtt_event &event)
{
    if (event.type == RT_MQTT_EVENT_NETWORK_AVAILABLE)
    {
        app_get_mode().on_network_available();
        return;
    };

    if (event.type == RT_MQTT_EVENT_NETWORK_LOST)
    {
        app_get_mode().on_network_lost();
        return;
    };

    if (event.type == RT_MQTT_EVENT_CONNECTED)
    {
        app_get_mode().on_mqtt_connected();
        return;
    };

    if (event.type == RT_MQTT_EVENT_DISCONNECTED)
    {
        app_get_mode().on_mqtt_disconnected();
        return;
    };

    if (event.type == RT_MQTT_EVENT_RECEIVE)
    {
        app_on_mqtt_receive(event);
        return;
    };
}