
#include <global.h>
#include <esp_mqtt.h>
#include <esp_http.h>

const uint8_t relaycount = 10;
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


void page_relay()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='relayform' name='relay' method='get' action='/store'>\
<input type='hidden' name='relaysetup' value='1'>\
<table border='0'><tr><td><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < relaycount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='pinid" + String(i) + "' value='" + (relayPins[i] > -1 ? String(relayPins[i]) : "") + "'>";
    }

    message += "</td><td><b>State on boot:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='onboot" + String(i) + "' value='1' ";
        if (relayOnBoot[i]) message += "checked ";
        message += "> ON &nbsp; <input type='radio' name='onboot" + String(i) + "' value='0' ";
        if (! relayOnBoot[i]) message += "checked ";
        message += "> OFF";
    }

    message += "</td></tr></table><br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Relay Setup:", message);
}


void page_switch()
{
    if(!HTTP_auth())
        return;

    int id = httpserver.arg("s").toInt();
    String on = httpserver.arg("on");

    RELAY_switch(id, on == "true");

    httpserver.send(200, "text/html", "OK");
}


void RELAY_setup()
{
    httpserver.on("/relay", HTTP_GET, page_relay);
    httpserver.on("/switch", HTTP_GET, page_switch);
    for (byte i = 0; i < relaycount; i++)
    {
        if(relayPins[i] > -1)
        {
            pinMode(relayPins[i], OUTPUT);
            digitalWrite(relayPins[i], relayOnBoot[i]);
        }
    }
}


String RELAY_getInfo()
{
    String message = "";
    for (byte i = 0; i < relaycount; i++)
    {
        if (relayPins[i] > -1)
        {
            message += " <input type='checkbox' class='checkbox' id='relay" + String(i) +
                       "' onchange=\"openUrl('/switch?s=" + String(i) + "&on=' + this.checked);\"";
            if (digitalRead(relayPins[i]))
                message += "checked ";

            message += "> <label for='relay" + String(i) + "'> #" + String(i) + " (PIN: " + String(relayPins[i]) + ")</label>";
        }
    }
    return message;
}
