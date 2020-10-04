
#include <global.h>
#include <esp_mqtt.h>
#include <esp_http.h>

const uint8_t analogcount = 5;
int8_t analogPins[] = {-1, -1, -1, -1, -1};
uint16_t analogDifToUpdate = 10; //Minimal difference from last read to agreed updated value
int16_t analogLastValue[] = {0, 0, 0, 0, 0};
int16_t analogLastValue1[] = {0, 0, 0, 0, 0};
int16_t analogLastValue2[] = {0, 0, 0, 0, 0};
int16_t analogLastValue3[] = {0, 0, 0, 0, 0};
int16_t analogLastValue4[] = {0, 0, 0, 0, 0};
bool analogSmooth[] = {false, false, false, false, false};
uint32_t ANALOG_CHECKtimeout = 5; // Check analog status every 5 sec
uint32_t ANALOG_CHECKnext = 0; // Next analog check

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
                if(analogSmooth[i])
                {
                    analogLastValue4[i] = (analogLastValue3[i] > 0 ? analogLastValue3[i] : analogvalue);
                    analogLastValue3[i] = (analogLastValue2[i] > 0 ? analogLastValue2[i] : analogvalue);
                    analogLastValue2[i] = (analogLastValue1[i] > 0 ? analogLastValue1[i] : analogvalue);
                    analogLastValue1[i] = analogvalue;
                    analogvalue = int((analogLastValue1[i] + analogLastValue2[i] + analogLastValue3[i] + analogLastValue4[i]) / 4);
                }

                if (abs(analogLastValue[i] - analogvalue) > analogDifToUpdate)
                {
                    analogLastValue[i] = analogvalue;
                    MQTT_publish("analogs/a" + String(i), String(analogvalue));
                }
            }
        }
        ANALOG_CHECKnext = millis() + ANALOG_CHECKtimeout*1000;
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


void page_analog()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='analogform' name='analog' method='get' action='/store'>\
<input type='hidden' name='analogsetup' value='1'>\
<table border='0'><tr><td><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < analogcount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='apinid" + String(i) + "' value='" + (analogPins[i] > -1 ? String(analogPins[i]) : "") + "'>";
    }
    message += "</td><td style='border-left: dashed 1px black;padding: 0px 15px;'><b>Smooth:</b>";

    for (i = 0; i < analogcount; i++)
    {
        message += "<br><input type='checkbox' name='smoothid" + String(i) + "' value='1' ";
        if (analogSmooth[i]) message += "checked ";
        message += "> Smooth";
    }

    // language=HTML
    message += "\
</td></tr></table><br><b>Analog update period:</b>\
<br><input type='text' name='analogupdate' size='2' value='" + String(ANALOG_CHECKtimeout) + "'> sec\
<br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Analog Setup:", message);
}


void ANALOG_setup()
{
    for (byte i = 0; i < analogcount; i++)
    {
        if(analogPins[i] > -1)
        {
            pinMode(analogPins[i], INPUT);
        }
    }
    httpserver.on("/analog", HTTP_GET, page_analog);
}


