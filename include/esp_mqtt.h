
#ifndef ESP_MQTT
#define ESP_MQTT

void MQTT_setup();

void MQTT_handle();

bool MQTT_isconnected();

bool MQTT_publish(const String& topic, const String& value);

#endif