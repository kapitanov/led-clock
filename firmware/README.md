LED Clock firmware
==================

Installing
----------

```shell
# Upload font files to device
platformio run -t uploadfs
# Build and upload code to device
platformio run -t upload
```

Compile
-------

```shell
platformio run
```

Compile and upload to board
---------------------------

```shell
platformio run --target upload
platformio run --target uploadfs
```

Configure
---------

1. Connect via COM terminal (ANSI compatible, like putty)
2. Type help to get some help
3. Connect to WiFi (see `wifi up` command)
4. Check Internet connection (see `wifi test` command)
5. Connect to MQTT broker (see `mqtt connect` command)
