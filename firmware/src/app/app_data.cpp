#include "app.h"

app_available_data_enum app_available_data = APP_DATA_NONE;
app_time_data app_data_time;
app_weather_data app_data_weather;
app_covid_data app_data_covid;

app_available_data_enum app_has_data()
{
    return app_available_data;
}

bool app_has_data(app_available_data_enum data)
{
    return (app_available_data & data) != 0;
}

bool app_get_time_data(app_time_data &data)
{
    if ((app_available_data & APP_DATA_TIME) != APP_DATA_TIME)
    {
        return false;
    }

    memcpy(&data, &app_data_time, sizeof(app_time_data));
    return true;
}

bool app_get_weather_data(app_weather_data &data)
{
    if ((app_available_data & APP_DATA_WEATHER) != APP_DATA_WEATHER)
    {
        return false;
    }

    memcpy(&data, &app_data_weather, sizeof(app_weather_data));
    return true;
}

bool app_get_covid_data(app_covid_data &data)
{
    if ((app_available_data & APP_DATA_COVID) != APP_DATA_COVID)
    {   
        return false;
    }

    memcpy(&data, &app_data_covid, sizeof(app_covid_data));
    return true;
}

void app_set_time_data(const app_time_data &data)
{
    memcpy(&app_data_time, &data, sizeof(app_time_data));
    app_available_data = (app_available_data_enum)(app_available_data | APP_DATA_TIME);

    app_get_mode().on_data_updated(app_available_data);
}

void app_set_weather_data(const app_weather_data &data)
{
    memcpy(&app_data_weather, &data, sizeof(app_weather_data));
    app_available_data = (app_available_data_enum)(app_available_data | APP_DATA_WEATHER);

    app_get_mode().on_data_updated(app_available_data);
}

void app_set_covid_data(const app_covid_data &data)
{
    memcpy(&app_data_covid, &data, sizeof(app_covid_data));
    app_available_data = (app_available_data_enum)(app_available_data | APP_DATA_COVID);

    app_get_mode().on_data_updated(app_available_data);
}