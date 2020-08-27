
#ifndef ESP_RELAY
#define ESP_RELAY

void RELAY_setup();

String RELAY_getInfo();

void RELAY_switch(int id, bool setval);

#endif