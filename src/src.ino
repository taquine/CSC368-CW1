/*
    EmSys Cw1 - Low Powered IoT node
    
    Fung Wah Westley - 1901519
    Karim Stefan Nouda - 1914729

*/
#include "dotDevice.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#define mS_TO_S_FACTOR 1000  // Conversion factor for milli-seconds to seconds

OneWire oneWire(26);
DallasTemperature sensors(&oneWire);

const char* ssid = "VM4273021";
const char* password = "xjKghgtY5rcc";
const char* server = "ws://ec2-52-15-138-171.us-east-2.compute.amazonaws.com:1234"; 
const char* gid = "Nx4gVDM1";

dotDevice s_con(ssid, password, server); //connection object

float tempsForAvg[30];
int tmpCntr;


float getAvg() {
    float sum = 0.0;

    for (int i = 0; i <= 15; ++i) {
        sum += tempsForAvg[i];
    }

    return sum / 16.0;
}

float getTemp() {
    sensors.requestTemperatures();

    return sensors.getTempCByIndex(0);
}

String getTempJSON() {
    String jsonReading;

    for (int i = 0; i <=15; ++i){
        tempsForAvg[i] = getTemp();
        jsonReading += "{\"timestamp\" : " + String(millis()) + ", \"value\": " + String(tempsForAvg[tmpCntr]) + "}";
        if (i < 15){
            jsonReading += ",";
        } else {
            jsonReading += "]}";
        }
        tmpCntr += 1;
    }

    return jsonReading;
}

String buildPayload() {
    String tmpData = getTempJSON(); //add temp readings (had to do this b4 so the average was right)
    String payload = "{\"device\": \"Nx4gVDM1\",\"average\": " + String(getAvg()) + ",\"values\":[";
    payload += tmpData;
    tmpCntr = 0;

    return payload;
}

void setup() {
    s_con.connect();
}

void loop() {
    s_con.sendJSON(payload);
    delay(50);

    esp_sleep_enable_timer_wakeup((30000 - millis()) * mS_TO_S_FACTOR); //no idea why this doesnt get a consistent result
    esp_deep_sleep_start();

}
