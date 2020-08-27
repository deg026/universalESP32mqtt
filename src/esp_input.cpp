
#include <global.h>
#include <esp_mqtt.h>

uint32_t ProtectionInterval = 50;
uint32_t buttonLongClickInterval = 750;
uint32_t buttonDoubleClickInterval = 750;
uint32_t buttonLastClick[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t inputLastChange[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool inputLastState[] = {false, false, false, false, false, false, false, false, false, false};
uint8_t inputcount = 10;
int8_t inputPins[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int8_t inputTypes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int8_t inputTrueSignal[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void INPUT_setup()
{
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
            if (digitalRead(inputPins[i]) == inputTrueSignal[i])
                message += " inputon";

            message += "'>" + String(inputPins[i]) + "</span>";
        }
    }
    return message;
}