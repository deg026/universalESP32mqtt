
#include "global.h"
#include "esp_wifi.h"
#include "esp_http.h"
#include "esp_eeprom.h"
#include "esp_mqtt.h"
#include "esp_relay.h"
#include "esp_input.h"
#include "esp_analog.h"
#include "esp_temper.h"

const char* appversion = "0.26";

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    eeprom_setup();
    TEMPER_setup(); //1wire better to initialize before WiFi
    WF_setup();
    http_setup();
    MQTT_setup();
    RELAY_setup();
    INPUT_setup();
    ANALOG_setup();
}

void loop()
{
    delay(5);
    WF_handle();
    http_handle();
    MQTT_handle();
    TEMPER_handle();
    INPUT_handle();
    ANALOG_handle();
}

/**

 Best PINs usage, from best to worst (Input, Output, Analog, u - pullup, d - pulldown):
 39 - Id
 36 - Id
 35 - Id
 34 - Id/A
 33 - I/O/A
 32 - I/O/A
 27 - I/O
 26 - I/O
 25 - I/O
 23 - I/O
 22 - I/O
 21 - I/O
 19 - I/O
 18 - I/O
 14 - I/O
  4 - I/O
 17 - I/O (in some device used for internal Flash)
 16 - I/O (in some device used for internal Flash)
 15 - I (if pulled low there will be no output on the Serial port)
  5 - I/O (connected to internal LED on some devices)
  2 - Iu/O (connected to internal LED on most devices)

**/