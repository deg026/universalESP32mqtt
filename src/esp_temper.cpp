
#include <global.h>
#include <esp_mqtt.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define TEMPERATURE_PRECISION 10

int8_t ONEWIRE_PIN = -1;

OneWire oneWire;
DallasTemperature TEMPER_sensors;
DeviceAddress thermometer[10];
float lastthermometer[10];
int TEMPER_count = 0;

uint32_t TEMPER_nexttimeout = 0; // timeout for next handler run
uint16_t TEMPER_timeout = 20; // timeout in seconds before next read sensors
const uint32_t TEMPER_timeupdate = 7200000; // time to force update sensor's values on MQTT server (in case when value is static, to show that we alive)
uint32_t lastTemp;    //sensor get value list timestamp
uint32_t lastUpdate;  //mqtt value update last timestamp
bool TEMPER_needrequest = true;


void TEMPER_discover()
{
    oneWire.reset_search();
    TEMPER_count = 0;
    while(oneWire.search(thermometer[TEMPER_count]))
    {
        TEMPER_count++;
    }
}


void TEMPER_setup()
{
    if(ONEWIRE_PIN >= 0)
    {
        oneWire.begin(ONEWIRE_PIN);
        TEMPER_sensors.setOneWire(&oneWire);
        TEMPER_sensors.begin();
        TEMPER_sensors.setWaitForConversion(false);
        TEMPER_sensors.setResolution(TEMPERATURE_PRECISION);
        TEMPER_discover();
    }
}


String getSensorAddress(DeviceAddress deviceAddress)
{
    String ret;
    for (uint8_t i = 0; i < 8; i++)
        ret += String(deviceAddress[i], HEX);

    return ret;
}


String TEMPER_getInfo()
{
    String message = "<br><br>";
    for (byte i = 0; i < TEMPER_count; i++)
        message += "<br>T" + String(i) + " [" + getSensorAddress(thermometer[i]) + "]: " + String(lastthermometer[i]) + " &deg;C";
    return message;
}


float getTemperature(DeviceAddress deviceAddress)
{
    float tempC = TEMPER_sensors.getTempC(deviceAddress);
    if(tempC == DEVICE_DISCONNECTED_C)
        return 9999;

    return tempC;
}


void TEMPER_handle()
{
    float curTemp;

    if(TEMPER_count > 0 && (lastTemp == 0 || (lastTemp + TEMPER_nexttimeout) < millis()))
    {
        if(TEMPER_needrequest)
        {
            TEMPER_sensors.requestTemperatures();
            TEMPER_nexttimeout = 1000; //1 second befor temperature will be ready
        }
        else
        {
            if ((lastUpdate + TEMPER_timeupdate) < millis())
                lastUpdate = 0;

            for (byte i = 0; i < TEMPER_count; i++)
            {
                curTemp = getTemperature(thermometer[i]);
                if (curTemp > -50 && curTemp < 99 && (curTemp != lastthermometer[i] || lastUpdate == 0))
                {
                    lastthermometer[i] = curTemp;
                    MQTT_publish("temper/" + getSensorAddress(thermometer[i]), String(curTemp));
                }
            }

            if (lastUpdate == 0)
                lastUpdate = millis();

            TEMPER_nexttimeout = TEMPER_timeout * 1000; //default timeout between temperature requests
        }
        lastTemp = millis();
        TEMPER_needrequest =! TEMPER_needrequest;
    }
}
