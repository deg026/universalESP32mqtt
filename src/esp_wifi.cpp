
#include <global.h>
#include <WiFi.h>

const char* WF_ssidAP = "esp32mqtt";

String WF_passwordSTA;
String WF_ssidSTA;

uint32_t WF_ConnectTimeout = 60000;
uint32_t WF_APtimeout = 300000; // AP point 5 min timeout when lost WiFi connection
uint32_t WF_APlast = 0;
uint32_t WF_CHECKtimeout = 5000; // Check WiFi status every 5 sec
uint32_t WF_CHECKnext = 0; // Check WiFi status every 5 sec


void WF_AccessPoint()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WF_ssidAP);
    WF_APlast = millis();
    digitalWrite(LED_PIN, LOW);
}


bool WF_ConnectStation()
{
    uint32_t WF_maxtime = millis() + WF_ConnectTimeout;

    WiFi.mode(WIFI_STA);
    WiFi.begin(WF_ssidSTA.c_str(), WF_passwordSTA.c_str());

    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, LOW);
        delay(300);
        digitalWrite(LED_PIN, HIGH);
        delay(300);
        if (millis() > WF_maxtime)
            return false;
    }
    return true;
}


void WF_setup()
{
    if ((! WF_ssidSTA.length()) || (! WF_ConnectStation()))
        WF_AccessPoint();
}


void WF_handle()
{
    if(millis() > WF_CHECKnext)
    {
        /* Reconnect WiFi if disconnected */
        if (
                (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED)
                ||
                (WiFi.getMode() == WIFI_AP && WF_ssidSTA.length() > 1 && millis() > (WF_APlast + WF_APtimeout))
                )
            WF_setup();
        WF_CHECKnext = millis() + WF_CHECKtimeout;
    }
}
