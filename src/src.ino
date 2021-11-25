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

dotDevice s_con(ssid, password, server);

float temp_in_c; //the current temp [MIGHT BE USELESS]
unsigned long payloadTimeSent;  //the time payload last sent to be able to timestamp readings

const String test_payload = "{\"device\": \"Nx4gVDM1\","
    "\"average\": 19.4,"
    "\"values\":["
    "{\"timestamp\" : 1034, \"value\": 19.5},"
    "{\"timestamp\" : 1134, \"value\": 19.4},"
    "{\"timestamp\" : 1234, \"value\": 19.2},"
    "{\"timestamp\" : 1334, \"value\": 19.4},"
    "{\"timestamp\" : 1434, \"value\": 19.5},"
    "{\"timestamp\" : 1534, \"value\": 19.4},"
    "{\"timestamp\" : 1634, \"value\": 19.2},"
    "{\"timestamp\" : 1734, \"value\": 19.5},"
    "{\"timestamp\" : 1834, \"value\": 19.4},"
    "{\"timestamp\" : 1934, \"value\": 19.2},"
    "{\"timestamp\" : 2034, \"value\": 19.5},"
    "{\"timestamp\" : 2134, \"value\": 19.4},"
    "{\"timestamp\" : 2234, \"value\": 19.2},"
    "{\"timestamp\" : 2334, \"value\": 19.5},"
    "{\"timestamp\" : 2434, \"value\": 19.4},"
    "{\"timestamp\" : 2534, \"value\": 19.2}]}";

float getTemp() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

// TODO: IMPLEMENT CLOCK LIKE FROM THE lab5? ######################################################
int getTime() {
    return millis() - payloadTimeSent;
}

String getTempJSON() {
    return "{\"timestamp\" : " + String(getTime()) + ", \"value\": " + String(getTemp()) + "}";
}

void sendPayload(String payload) {
    s_con.sendJSON(payload); //change to bin protocol later
    payloadTimeSent = millis();
    return;
}

void setup() {
    Serial.begin(115200);
    Serial.println("CONNECTING");
    s_con.connect();
}

void loop() {
    delay(5000);
    Serial.println(getTempJSON());
    delay(5000);
    Serial.println(getTempJSON());
    delay(5000);
    Serial.println(getTempJSON());
    delay(5000);
    Serial.println(getTempJSON());
    delay(5000);
    Serial.println(getTempJSON());
    delay(5000);
    Serial.println(getTempJSON());
    s_con.sendJSON(test_payload);

}
