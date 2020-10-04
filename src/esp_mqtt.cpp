
#include <global.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_relay.h>
#include <esp_http.h>

String MQTT_user;
String MQTT_client = "ESP_mqtt";
String MQTT_pass;
String MQTT_server;
uint16_t MQTT_port = 1883;

WiFiClient wifiClient;
PubSubClient pubsubClient(wifiClient);


bool MQTT_isconnected()
{
    if(MQTT_server.length() && MQTT_port > 0 && pubsubClient.connected())
        return true;
    else
        return false;
}


bool MQTT_publish(const String& topic, const String& value)
{
    if(MQTT_isconnected())
        return pubsubClient.publish(("/" + MQTT_client + "/" + topic).c_str(), value.c_str());
    else
        return false;
}


void MQTT_callback(char* topic, byte* payload, unsigned int length)
{
    char* topicBody = topic + MQTT_client.length() + 2; // Skip "/ClientName/" from topic
    int id = 0;

    if(String(topicBody).substring(0, 9) == "relays/sw")
    {
        id = String(topicBody).substring(9).toInt();
        if (id >= 0 && id < relaycount && relayPins[id] > -1)
        {
            switch ((char)payload[0])
            {
                case '0':
                    RELAY_switch(id, false);
                    break;
                case '1':
                    RELAY_switch(id, true);
                    break;
                default:
                    bool relay = digitalRead(relayPins[id]);
                    MQTT_publish("relays/sw" + String(id), String(relay));
            }
        }
    }
}


bool MQTT_connect() {
    const uint32_t timeout = 30000;
    static uint32_t lastTime;
    bool result = false;

    if (lastTime == 0 || millis() > (lastTime + timeout))
    {
        if (MQTT_user.length())
            result = pubsubClient.connect(MQTT_client.c_str(), MQTT_user.c_str(), MQTT_pass.c_str());
        else
            result = pubsubClient.connect(MQTT_client.c_str());

        if (result)
        {
            String topic;
            for (byte i = 0; i < relaycount; i++)
            {
                topic = "/" + MQTT_client + "/relays/sw" + String(i);
                result = pubsubClient.subscribe(topic.c_str());
            }
        }
        lastTime = millis();
    }

    return result;
}


void page_mqtt()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "\
<form id='mqttform' name='mqtt' method='get' action='/store'>\
<b>Server:</b>\
<br><input type='text' name='mqttserver' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_server) + "'>\
 (leave blank to ignore MQTT)\
<br><b>Port:</b>\
<br><input type='text' name='mqttport' maxlength=5 value='" + String(MQTT_port) + "'>\
<br><b>Client name:</b>\
<br><input type='text' name='mqttclient' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_client) + "'>\
<br><b>User:</b> (if authorization is required on MQTT server)\
<br><input type='text' name='mqttuser' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_user) + "'>\
 (leave blank to ignore MQTT authorization)\
<br><b>Password:</b>\
<br><input type='password' name='mqttpass' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_pass) + "'>\
<br><input type='submit' value='Save'>\
</form>\
";
    HTML_page("MQTT Setup:", message);
}


void MQTT_setup()
{
    httpserver.on("/mqtt", HTTP_GET, page_mqtt);
    if (MQTT_server.length() && MQTT_port > 0)
    {
        pubsubClient.setServer(MQTT_server.c_str(), MQTT_port);
        pubsubClient.setCallback(MQTT_callback);

        MQTT_connect();
    }
}


void MQTT_handle()
{
    if (MQTT_server.length() && MQTT_port > 0 && WiFi.getMode() == WIFI_STA)
    {
        if (pubsubClient.connected())
            pubsubClient.loop();
        else
            MQTT_connect();
    }
}
