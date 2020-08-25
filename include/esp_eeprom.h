
#ifndef ESP_EEPROM
#define ESP_EEPROM

void eeprom_setup();

void eeprom_reset(bool SaveWifi);

void eeprom_writeConfig();

void eeprom_readConfig();

#endif