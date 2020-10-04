
#include <global.h>
#include <esp_mqtt.h>
#include <esp_http.h>

uint32_t ProtectionInterval = 50;
uint32_t buttonLongClickInterval = 750;
uint32_t buttonDoubleClickInterval = 750;
uint32_t buttonLastClick[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t inputLastChange[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool inputLastState[] = {false, false, false, false, false, false, false, false, false, false};
const uint8_t inputcount = 10;
int8_t inputPins[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int8_t inputTypes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int8_t inputTrueSignal[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};



void page_input()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='inputform' name='input' method='get' action='/store'>\
<input type='hidden' name='inputsetup' value='1'>\
<table border='0'><tr><td style='padding-right: 15px;'><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < inputcount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='inputpin" + String(i) + "' value='" + (inputPins[i] > -1 ? String(inputPins[i]) : "") + "'>";
    }

    message += "</td><td style='border-left: dashed 1px black;padding: 0px 15px;'><b>Input type:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='inputtype" + String(i) + "' value='0' ";
        if (inputTypes[i] % 10 == 0) message += "checked ";
        message += "> NORMAL &nbsp; <input type='radio' name='inputtype" + String(i) + "' value='1' ";
        if (inputTypes[i] % 10 == 1) message += "checked ";
        message += "> PULLUP &nbsp; <input type='radio' name='inputtype" + String(i) + "' value='2' ";
        if (inputTypes[i] % 10 == 2) message += "checked ";
        message += "> PULLDOWN";
    }

    message += "</td><td style='border-left: dashed 1px black;padding: 0px 15px;'><b>MTQQ send:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='inputsend" + String(i) + "' value='0' ";
        if (inputTypes[i] < 10) message += "checked ";
        message += "> HIGH & LOW &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='1' ";
        if (inputTypes[i] >= 10 && inputTypes[i] < 20) message += "checked ";
        message += "> only HIGH &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='2' ";
        if (inputTypes[i] >= 20 && inputTypes[i] < 30) message += "checked ";
        message += "> only LOW &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='3' ";
        if (inputTypes[i] >= 30) message += "checked ";
        message += "> BUTTON (click, longpress, dblclick)";
    }

    message += "</td></tr></table><br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Inputs Setup:", message);
}


void INPUT_setup()
{
    httpserver.on("/input", HTTP_GET, page_input);
    for (byte i = 0; i < inputcount; i++)
    {
        if(inputPins[i] > -1)
        {
            if (inputTypes[i] % 10 == 1)
            {
                pinMode(inputPins[i], INPUT_PULLUP);
                inputTrueSignal[i] = LOW;
            }
            else if (inputTypes[i] % 10 == 2)
                pinMode(inputPins[i], INPUT_PULLDOWN);
            else
                pinMode(inputPins[i], INPUT);
        }
    }
}


void INPUT_handle()
{
    int input;
    uint32_t inputdelay;
    bool state;
    for (byte i = 0; i < inputcount; i++)
    {
        if(inputPins[i] > -1)
        {
            input = digitalRead(inputPins[i]);
            state = (input == inputTrueSignal[i]);
            inputdelay = millis() - inputLastChange[i];

            if(inputdelay > ProtectionInterval &&  //bounce protection
               inputLastState[i] != state) //state is changed, from our last check
            {
                if (inputTypes[i] >= 30) // is button
                {
                    MQTT_publish("inputs/i" + String(i) + "/status", String(state));
                    if(!state) //button released
                    {
                        if(inputdelay > buttonLongClickInterval) //LONG click detected
                        {
                            MQTT_publish("inputs/i" + String(i) + "/longclick", "1");
                        }
                        else if((millis() - buttonLastClick[i]) < buttonDoubleClickInterval) //DOUBLE click detected
                        {
                            MQTT_publish("inputs/i" + String(i) + "/dblclick", "1");
                        }
                        else //single click detected
                        {
                            MQTT_publish("inputs/i" + String(i) + "/click", "1");
                        }
                        buttonLastClick[i] = millis();
                    }
                }
                else
                {
                    if(inputTypes[i] < 10 ||
                       (inputTypes[i] >= 10 && inputTypes[i] < 20 && input == HIGH) ||
                       (inputTypes[i] >= 20 && input == LOW)
                    )
                        MQTT_publish("inputs/i" + String(i), String(state));

                }
                inputLastState[i] = state;
                inputLastChange[i] = millis();
            }
        }
    }
}


String INPUT_getInfo()
{
    String message = "";
    for (byte i = 0; i < inputcount; i++)
    {
        if (inputPins[i] > -1)
        {
            message += " <span class='inputinfo";
            if (inputLastState[i])
                message += " inputon";

            message += "'>" + String(inputPins[i]) + "</span>";
        }
    }
    return message;
}