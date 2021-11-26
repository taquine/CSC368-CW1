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

const char* ssid = "VM4273021";
const char* password = "xjKghgtY5rcc";
const char* server = "ws://ec2-52-15-138-171.us-east-2.compute.amazonaws.com:1234"; 
const char* gid = "Nx4gVDM1";

dotDevice s_con(ssid, password, server); //connection object

float tempsForAvg[30];
int tmpCntr;

String temperaturePayload;


unsigned long payloadTimeSent;  //the time payload last sent to be able to timestamp readings

float getTemp() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

int getTime() {
    return millis() - payloadTimeSent;
}

void getTempJSON() {
    String jsonReading;
    tempsForAvg[tmpCntr] = getTemp();
    jsonReading = "{\"timestamp\" : " + String(getTime()) + ", \"value\": " + String(tempsForAvg[tmpCntr]) + "}";
    if (tmpCntr == 15) {
      temperaturePayload += jsonReading + "]}";
    } else {
      temperaturePayload += jsonReading + ",";
    }
    tmpCntr += 1;
    return;
}

float getAvg() {
    float sum = 0.0;

    for (int i = 0; i <= 15; ++i) {
        sum += tempsForAvg[i];
    }

    return sum / 16.0;
}

String buildPayload() {
    return "{\"device\": \"Nx4gVDM1\",\"average\": " + String(getAvg()) + ",\"values\":[" + temperaturePayload;
}

void sendPayload(String payload) {
    s_con.sendJSON(payload); //change to bin protocol later
    payloadTimeSent = millis();
    tmpCntr = 0;
    temperaturePayload = "";
    return;
}

void setup() {
    s_con.connect();
}

void loop() {
    for (int i = 0; i <= 15; ++i){
        getTempJSON();
        delay(100);
    }
    delay(18000);
    sendPayload(buildPayload());

}
