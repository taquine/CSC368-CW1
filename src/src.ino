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

unsigned long payloadTimeSent;  //the time payload last sent to be able to timestamp readings

float getTemp() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

int getTime() {
    return millis() - payloadTimeSent;
}

String getTempJSON() {
    tempsForAvg[tmpCntr] = getTemp();
    tmpCntr += 1;
    return "{\"timestamp\" : " + String(getTime()) + ", \"value\": " + String(tempsForAvg[tmpCntr-1]) + "}";
}

float getAvg() {
    float sum = 0.0;

    for (int i = 0; i < 30; ++i) {
        sum += tempsForAvg[i];
    }

    return sum / 30.0;
}

void sendPayload(String payload) {
    s_con.sendJSON(payload); //change to bin protocol later
    payloadTimeSent = millis();

    tmpCntr = 0;
    return;
}

void setup() {
    Serial.begin(115200);
    Serial.println("CONNECTING");
    s_con.connect();


}

void loop() {
    for (int i; i <= 30; ++i){
        Serial.println(getTempJSON());
        delay(1000);
    }
    Serial.println(String(getAvg()));
    sendPayload("");

}
