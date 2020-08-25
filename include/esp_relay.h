
#ifndef ESP_RELAY
#define ESP_RELAY

void RELAY_setup();

void RELAY_handle();

void RELAY_switch(int id, bool setval);

#endif