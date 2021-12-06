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
#define TEMPERATURE_PRECISION 9 //x bit data reading
//#define ASYNC_DELAY 188 //10 bit requires 187 ms for conversion

OneWire oneWire(26); //set up the oneWire protocol on pin 26
DallasTemperature sensors(&oneWire);


const char* ssid = "VM4273021";
const char* password = "xjKghgtY5rcc";
const char* server = "ws://ec2-52-15-138-171.us-east-2.compute.amazonaws.com:1234"; 
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
} data_packet;
#pragma pop(1)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t floating_to_fixed(float inp) {
    return (uint16_t)(round(inp * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}

void getBINpayload() {
    float sum = 0.0;
    for (int i = 0; i <16; ++i){
        data_packet.values[i].time = (uint16_t)millis();
        tempsForAvg[i] = getTemp();
        data_packet.values[i].temp = floating_to_fixed(tempsForAvg[i]);
        sum += tempsForAvg[i];
    }
    data_packet.avg = floating_to_fixed(sum/16.0);
    return;
}

float getTemp() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

void setup() {
    sensors.setResolution(TEMPERATURE_PRECISION);
    setCpuFrequencyMhz(80);
    //sensors.setWaitForConversion(false);
    //esp_sleep_enable_timer_wakeup(ASYNC_DELAY * mS_TO_S_FACTOR);
}

void loop() {
    getBINpayload();

    //setCpuFrequencyMhz(120);

    s_con.connect();
    s_con.sendBIN((char*)&data_packet,sizeof(data_packet));
    delay(8);

    esp_sleep_enable_timer_wakeup((30000 - millis()) * mS_TO_S_FACTOR); //no idea why this doesnt get a consistent result
    esp_deep_sleep_start();

}
