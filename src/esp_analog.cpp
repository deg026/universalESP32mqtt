
#include <global.h>
#include <esp_mqtt.h>

const uint8_t analogcount = 5;
int8_t analogPins[] = {-1, -1, -1, -1, -1};
uint16_t analogDifToUpdate = 20; //Minimal difference from last read to agreed updated value
int16_t analogLastValue[] = {0, 0, 0, 0, 0};
uint32_t ANALOG_CHECKtimeout = 5000; // Check analog status every 5 sec
uint32_t ANALOG_CHECKnext = 0; // Next analog check

void ANALOG_setup()
{
    for (byte i = 0; i < analogcount; i++)
    {
        if(analogPins[i] > -1)
        {
            pinMode(analogPins[i], INPUT);
        }
    }
}


void ANALOG_handle()
{
    if(millis() > ANALOG_CHECKnext)
    {
        int analogvalue;
        for (byte i = 0; i < analogcount; i++)
        {
            if (analogPins[i] > -1)
            {
                analogvalue = analogRead(analogPins[i]);
                if (abs(analogLastValue[i] - analogvalue) > analogDifToUpdate)
                {
                    analogLastValue[i] = analogvalue;
                    MQTT_publish("analogs/a" + String(i), String(analogvalue));
                }
            }
        }
        ANALOG_CHECKnext = millis() + ANALOG_CHECKtimeout;
    }
}


String ANALOG_getInfo()
{
    String message = "";
    for (byte i = 0; i < analogcount; i++)
    {
        if (analogPins[i] > -1)
        {
            message += " <span class='analoginfo'>#" + String(i) + " (" + String(analogPins[i]) + ") = " + String(analogLastValue[i]) + "</span>";
        }
    }
    return message;
}