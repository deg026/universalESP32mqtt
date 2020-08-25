
#include "global.h"
#include "esp_wifi.h"
#include "esp_http.h"
#include "esp_eeprom.h"
#include "esp_mqtt.h"
#include "esp_relay.h"
#include "esp_temper.h"

const char* appversion = "0.26";

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    eeprom_setup();
    TEMPER_setup();
    WF_setup();
    http_setup();
    MQTT_setup();
    RELAY_setup();
}

void loop()
{
    delay(5);
    WF_handle();
    http_handle();
    MQTT_handle();
    TEMPER_handle();
}


