# Backing web service for LED clock

## Configuration

App is configured via environment variables:

| Variable          | Default value  | Description                                        |
| ----------------- | -------------- | -------------------------------------------------- |
| `HTTP_ENDPOINT`   | `0.0.0.0:3000` | HTTP endpoint                                      |
| `MQTT_ENDPOINT`   |                | MQTT broker endpoint (e.g. `tcp://localhost:1883)` |
| `MQTT_USERNAME`   |                | MQTT broker login                                  |
| `MQTT_PASSWORD`   |                | MQTT broker password                               |
| `TIMEZONE`        | `UTC`          | Timezone for time provider                         |
| `WEATHER_CITY`    | `Moscow`       | City name for weather provider                     |
| `COVID19_COUNTRY` | `Russia`       | Country name for COVID-19 provider                 |

## How to run

1. Create `.env` file with all required environment variables (see above)
2. Create external docker services (see `external/docker-compose.yml`):

   ```shell
   if [[ -z "$(docker network inspect miot -f '1' 2> /dev/null)" ]]; then
      docker network create miot -d bridge > /dev/null;
   fi
   cd external
   echo "HOSTNAME=MY_HOST_NAME" > .env # replace MY_HOST_NAME with domain name
   docker-compose up -d
   ```

3. Run command:

   ```shell
   docker-compose up -d
   ```
