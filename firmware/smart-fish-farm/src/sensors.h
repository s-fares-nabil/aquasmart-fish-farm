#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

struct SensorReadings {
    float pH;
    float temperature;
    float waterLevel;
    float flowRate;
    float dissolvedOxygen;
    float waterLevelPercent;
    unsigned long timestamp;
    bool valid;
};

class Sensors {
public:
    Sensors();
    bool begin();
    SensorReadings readAll();
    float readPH();
    float readTemperature();
    float readWaterLevel();
    float readFlowRate();
    void calibratePH(float knownPH);

private:
    OneWire oneWire;
    DallasTemperature tempSensor;
    float phOffset;
    float phCalibrationSlope;
    volatile unsigned long flowPulseCount;
    unsigned long lastFlowTime;
    float flowRate;
    static void IRAM_ATTR flowISR();

    float applyMovingAverage(float newValue, float* buffer, int size, int& index);
    float phBuffer[5];
    int phBufferIndex;
    float tempBuffer[5];
    int tempBufferIndex;
};

#endif
