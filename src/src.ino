/*
        EmSys Cw1 - Low Powered IoT node
    
    Fung Wah Westley - 1901519
    Karim Stefan Nouda - 1914729

*/
typedef uint16_t fixed_point_t;

#include "dotDevice.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#define mS_TO_S_FACTOR 1000  // Conversion factor for milli-seconds to seconds
#define FIXED_POINT_FRACTIONAL_BITS 8 //amount of bits dedicated to the fractional section

OneWire oneWire(26); //set up the oneWire protocol on pin 26

DallasTemperature sensors(&oneWire);

const char* ssid = "NETGEAR35";
const char* password = "magicalfinch482";
const char* server = "ws://192.168.1.2:1234"; 
const char* gid = "Nx4gVDM1";

dotDevice s_con(ssid, password, server); //connection object

float tempsForAvg[16]; // list of recent temps to be used for the average

struct reading
{
    uint16_t time;
    uint16_t temp;
};


#pragma pack(1)
struct tempPayload { //total of 76 bytes
    char gid[8] = {'N','x','4','g','V','D','M','1'}; //8 bytes (1 per char)
    uint16_t cmd = 1; //2 bytes
    uint16_t avg; //2 bytes
    struct reading values[16]; // 16 * (2 bytes for time + 2 bytes for fixed point temp) even:time odd:temp
}data_packet;
#pragma pop(1)

uint16_t floating_to_fixed(float inp) {
    return (uint16_t)(round(inp * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}

void getBINpayload() {
    for (int i = 0; i <16; ++i){
        data_packet.values[i].time = (uint16_t)millis();
        tempsForAvg[i] = getTemp();
        data_packet.values[i].temp = floating_to_fixed(tempsForAvg[i]);
    }
    data_packet.avg = floating_to_fixed(getAvg());
    return;
}

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
        jsonReading += "{\"timestamp\" : " + String(millis()) + ", \"value\": " + String(tempsForAvg[i]) + "}";
        if (i < 15){
            jsonReading += ",";
        } else {
            jsonReading += "]}";
        }
    }

    return jsonReading;
}

String buildPayload() {
    String tmpData = getTempJSON(); //add temp readings (had to do this b4 so the average was right)
    String payload = "{\"device\": \"Nx4gVDM1\",\"average\": " + String(getAvg()) + ",\"values\":[";
    payload += tmpData;

    return payload;
}

void setup() {
    s_con.connect();
}

void loop() {
    getBINpayload();
    s_con.sendBIN((char*)&data_packet,sizeof(data_packet));
    delay(10);

    esp_sleep_enable_timer_wakeup((30000 - millis()) * mS_TO_S_FACTOR); //no idea why this doesnt get a consistent result
    esp_deep_sleep_start();

}
