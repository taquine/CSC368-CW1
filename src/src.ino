/*
    EmSys Cw1 - Low Powered IoT node
    
    Fung Wah Westley - 1901519
    Karim Stefan Nouda - 1914729

*/
#include "dotDevice.h"
#include "OneWire.h"
#include "DallasTemperature.h"

OneWire oneWire(26);
DallasTemperature sensors(&oneWire);

float temp_in_c; //the current temp


void setup() {
    Serial.begin(115200);

}

void loop() {
    sensors.requestTemperatures();
    temp_in_c = sensors.getTempCByIndex(0);
    Serial.println(temp_in_c);

}
