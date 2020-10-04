
#ifndef ESP_GLOBAL
#define ESP_GLOBAL

#include <Arduino.h>
#include <WebServer.h>

//MAIN
extern const char* appversion;
extern const char* appdate;

//MQTT
extern String MQTT_user;
extern String MQTT_client;
extern String MQTT_pass;
extern String MQTT_server;
extern uint16_t MQTT_port;

// WIFI
extern String WF_ssidSTA;
extern String WF_passwordSTA;

// HTTP
extern WebServer httpserver;
extern String HTTP_user;
extern String HTTP_password;
extern const byte maxStrParamLength;

//EEPROM
extern int8_t LED_PIN;

//TEMPER
extern int8_t ONEWIRE_PIN;
extern uint16_t TEMPER_timeout;

//RELAY
const extern uint8_t relaycount;
extern int8_t relayPins[];
extern bool relayOnBoot[];

//INPUS
const extern uint8_t inputcount;
extern int8_t inputPins[];
extern int8_t inputTypes[];

//ANALOG
const extern uint8_t analogcount;
extern int8_t analogPins[];
extern bool analogSmooth[];
extern uint32_t ANALOG_CHECKtimeout;

#endif
