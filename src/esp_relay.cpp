
#include <global.h>
#include <esp_mqtt.h>

uint8_t relaycount = 10;
int8_t relayPins[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
bool relayOnBoot[] = {false, false, false, false, false, false, false, false, false, false};

void RELAY_switch(int id, bool setval)
{
    if (id >= 0 && id < relaycount && relayPins[id] > -1 && digitalRead(relayPins[id]) != setval)
    {
        digitalWrite(relayPins[id], setval);
        MQTT_publish("relays/sw" + String(id), String(setval));
    }
}


void RELAY_setup()
{
    for (byte i = 0; i < relaycount; i++)
    {
        if(relayPins[i] > -1)
        {
            pinMode(relayPins[i], OUTPUT);
            digitalWrite(relayPins[i], relayOnBoot[i]);
        }
    }
}


void RELAY_handle()
{
}
