#include <global.h>
#include <Preferences.h>

int8_t LED_PIN = 5;
uint8_t analogcount = 5;
int8_t analogPins[] = {-1, -1, -1, -1, -1};


Preferences prefs;

void eeprom_writeConfig()
{
    prefs.putUChar("config", 1);

    prefs.putString("WF_passwordSTA", WF_passwordSTA);
    prefs.putString("WF_ssidSTA", WF_ssidSTA);

    prefs.putString("HTTP_user", HTTP_user);
    prefs.putString("HTTP_password", HTTP_password);

    prefs.putString("MQTT_user", MQTT_user);
    prefs.putString("MQTT_pass", MQTT_pass);
    prefs.putString("MQTT_server", MQTT_server);
    prefs.putString("MQTT_client", MQTT_client);
    prefs.putUShort("MQTT_port", MQTT_port);

    prefs.putChar("LED_PIN", LED_PIN);
    prefs.putChar("ONEWIRE_PIN", ONEWIRE_PIN);
    prefs.putUShort("TEMPER_timeout", TEMPER_timeout);

    byte i;

    for (i = 0; i < relaycount; i++)
        prefs.putBool(("relayOnBoot" + String(i)).c_str(), relayOnBoot[i]);

    for (i = 0; i < relaycount; i++)
        prefs.putChar(("relayPins" + String(i)).c_str(), relayPins[i]);

    for (i = 0; i < inputcount; i++)
        prefs.putChar(("inputPins" + String(i)).c_str(), inputPins[i]);

    for (i = 0; i < inputcount; i++)
        prefs.putChar(("inputTypes" + String(i)).c_str(), inputTypes[i]);

    for (i = 0; i < analogcount; i++)
        prefs.putChar(("analogPins" + String(i)).c_str(), analogPins[i]);

}

void eeprom_readConfig()
{
    if(prefs.getUChar("config", 0) == 1)
    {
        WF_ssidSTA = prefs.getString("WF_ssidSTA", WF_ssidSTA);
        WF_passwordSTA = prefs.getString("WF_passwordSTA", WF_passwordSTA);

        HTTP_user = prefs.getString("HTTP_user", HTTP_user);
        HTTP_password = prefs.getString("HTTP_password", HTTP_password);

        MQTT_user = prefs.getString("MQTT_user", MQTT_user);
        MQTT_pass = prefs.getString("MQTT_pass", MQTT_pass);
        MQTT_server = prefs.getString("MQTT_server", MQTT_server);
        MQTT_client = prefs.getString("MQTT_client", MQTT_client);
        MQTT_port = prefs.getUShort("MQTT_port", MQTT_port);

        LED_PIN = prefs.getChar("LED_PIN", LED_PIN);
        ONEWIRE_PIN = prefs.getChar("ONEWIRE_PIN", ONEWIRE_PIN);
        TEMPER_timeout = prefs.getUShort("TEMPER_timeout", TEMPER_timeout);

        byte i;

        for (i = 0; i < relaycount; i++)
            relayOnBoot[i] = prefs.getBool(("relayOnBoot" + String(i)).c_str(), relayOnBoot[i]);

        for (i = 0; i < relaycount; i++)
            relayPins[i] = prefs.getChar(("relayPins" + String(i)).c_str(), relayPins[i]);

        for (i = 0; i < inputcount; i++)
            inputPins[i] = prefs.getChar(("inputPins" + String(i)).c_str(), inputPins[i]);

        for (i = 0; i < inputcount; i++)
            inputTypes[i] = prefs.getChar(("inputTypes" + String(i)).c_str(), inputTypes[i]);

        for (i = 0; i < analogcount; i++)
            analogPins[i] = prefs.getChar(("analogPins" + String(i)).c_str(), analogPins[i]);
    }
}


void eeprom_setup()
{
    prefs.begin("nvs", false); // RW mode
    eeprom_readConfig();
}


void eeprom_reset(bool SaveWifi)
{
    prefs.clear();
    if(SaveWifi)
    {
        prefs.putUChar("config", 1);
        prefs.putString("WF_passwordSTA", WF_passwordSTA);
        prefs.putString("WF_ssidSTA", WF_ssidSTA);
    }
}

/*
  Put
  ---
  size_t putChar   (const char* key,      int8_t value);
  size_t putUChar  (const char* key,     uint8_t value);
  
  size_t putShort  (const char* key,     int16_t value);
  size_t putUShort (const char* key,    uint16_t value);
  
  size_t putInt    (const char* key,     int32_t value);
  size_t putUInt   (const char* key,    uint32_t value);
  size_t putLong   (const char* key,     int32_t value);
  size_t putULong  (const char* key,    uint32_t value);
  
  size_t putLong64 (const char* key,     int64_t value);
  size_t putULong64(const char* key,    uint64_t value);
  
  size_t putFloat  (const char* key,     float_t value);
  size_t putDouble (const char* key,    double_t value);
  
  size_t putBool   (const char* key,        bool value);
  
  size_t putString (const char* key, const char* value);
  size_t putString (const char* key,      String value);
  
  size_t putBytes  (const char* key, const void* value, size_t len);

  Get
  ---
  int8_t   getChar   (const char* key, int8_t   defaultValue = 0);
  uint8_t  getUChar  (const char* key, uint8_t  defaultValue = 0);

  int16_t  getShort  (const char* key, int16_t  defaultValue = 0);
  uint16_t getUShort (const char* key, uint16_t defaultValue = 0);

  int32_t  getInt    (const char* key, int32_t  defaultValue = 0);
  uint32_t getUInt   (const char* key, uint32_t defaultValue = 0);

  int32_t  getLong   (const char* key, int32_t  defaultValue = 0);
  uint32_t getULong  (const char* key, uint32_t defaultValue = 0);

  int64_t  getLong64 (const char* key, int64_t  defaultValue = 0);
  uint64_t getULong64(const char* key, uint64_t defaultValue = 0);

  float_t  getFloat  (const char* key, float_t  defaultValue = NAN);
  double_t getDouble (const char* key, double_t defaultValue = NAN);

  bool     getBool   (const char* key, bool     defaultValue = false);

  size_t   getString (const char* key, char* value, size_t maxLen);
  String   getString (const char* key, String   defaultValue = String());

  size_t   getBytes  (const char* key, void * buf, size_t maxLen);
*/

