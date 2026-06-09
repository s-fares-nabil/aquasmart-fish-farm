#include "sensors.h"

Sensors* sensorInstance = nullptr;

Sensors::Sensors() : oneWire(TEMP_SENSOR_PIN), tempSensor(&oneWire) {
    phOffset = PH_OFFSET;
    phCalibrationSlope = 1.0;
    flowPulseCount = 0;
    lastFlowTime = 0;
    flowRate = 0.0;
    phBufferIndex = 0;
    tempBufferIndex = 0;
    for (int i = 0; i < 5; i++) {
        phBuffer[i] = 7.0;
        tempBuffer[i] = 25.0;
    }
    sensorInstance = this;
}

bool Sensors::begin() {
    Serial.println("[SENSORS] Initializing...");
    tempSensor.begin();
    EEPROM.begin(64);

    float savedOffset;
    EEPROM.get(0, savedOffset);
    if (!isnan(savedOffset) && savedOffset > -2.0 && savedOffset < 2.0) {
        phOffset = savedOffset;
    }

    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowISR, RISING);
    lastFlowTime = millis();

    Serial.println("[SENSORS] Initialization complete");
    return true;
}

void IRAM_ATTR Sensors::flowISR() {
    if (sensorInstance) {
        sensorInstance->flowPulseCount++;
    }
}

float Sensors::readPH() {
    int rawSum = 0;
    for (int i = 0; i < PH_SAMPLES; i++) {
        rawSum += analogRead(PH_SENSOR_PIN);
        delay(10);
    }
    float avgRaw = rawSum / (float)PH_SAMPLES;
    float voltage = avgRaw * 3.3 / 4095.0;
    float phValue = 7.0 + ((2.5 - voltage) / 0.18) * phCalibrationSlope + phOffset;
    phValue = constrain(phValue, 0.0, 14.0);
    return applyMovingAverage(phValue, phBuffer, 5, phBufferIndex);
}

float Sensors::readTemperature() {
    tempSensor.requestTemperatures();
    float temp = tempSensor.getTempCByIndex(0);

    if (temp == DEVICE_DISCONNECTED_C || isnan(temp) || temp < -10 || temp > 60) {
        Serial.println("[SENSORS] Temperature sensor error, using default");
        temp = 25.0;
    }
    return applyMovingAverage(temp, tempBuffer, 5, tempBufferIndex);
}

float Sensors::readWaterLevel() {
    int raw = analogRead(WATER_LEVEL_SENSOR_PIN);
    float voltage = raw * 3.3 / 4095.0;
    float levelCm = map(raw, 0, 4095, 0, 30);
    levelCm = constrain(levelCm, 0.0, 30.0);
    return levelCm;
}

float Sensors::readFlowRate() {
    unsigned long now = millis();
    unsigned long elapsed = now - lastFlowTime;

    if (elapsed >= 1000) {
        noInterrupts();
        unsigned long count = flowPulseCount;
        flowPulseCount = 0;
        interrupts();

        flowRate = (count / 7.5);
        lastFlowTime = now;
    }
    return flowRate;
}

SensorReadings Sensors::readAll() {
    SensorReadings data;
    data.timestamp = millis();
    data.pH = readPH();
    data.temperature = readTemperature();
    data.waterLevel = readWaterLevel();
    data.flowRate = readFlowRate();
    data.waterLevelPercent = (data.waterLevel / WATER_LEVEL_MAX_CM) * 100.0;

    data.dissolvedOxygen = 14.6 - 0.41 * data.temperature + 0.008 * (data.temperature * data.temperature) + 0.02 * data.flowRate;
    data.dissolvedOxygen = constrain(data.dissolvedOxygen, 0.0, 15.0);

    data.valid = (data.pH > 0 && data.pH < 14 &&
                  data.temperature > -10 && data.temperature < 60 &&
                  data.waterLevel >= 0);

    return data;
}

void Sensors::calibratePH(float knownPH) {
    float rawVoltage = analogRead(PH_SENSOR_PIN) * 3.3 / 4095.0;
    float expectedVoltage = 2.5 + (7.0 - knownPH) * 0.18;
    phOffset = knownPH - (7.0 + (2.5 - rawVoltage) / 0.18);
    EEPROM.put(0, phOffset);
    EEPROM.commit();
    Serial.printf("[SENSORS] pH calibrated: offset=%.3f\n", phOffset);
}

float Sensors::applyMovingAverage(float newValue, float* buffer, int size, int& index) {
    buffer[index] = newValue;
    index = (index + 1) % size;
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum / size;
}
