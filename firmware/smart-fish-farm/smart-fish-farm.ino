/******************************************************************************
 * AquaSmart — IoT-based Smart Fish Farm Water Quality Monitoring System
 *
 * Monitors pH, temperature, water level, flow rate, and dissolved oxygen
 * in real-time. Features Blynk dashboard, SD card logging, and alert system.
 *
 * Hardware:
 *   - ESP32 Dev Board
 *   - pH Sensor (analog)
 *   - DS18B20 Temperature Sensor
 *   - Water Level Sensor (analog)
 *   - YF-S201 Flow Rate Sensor
 *   - 5V Relay Module (pump control)
 *   - Active Buzzer (alerts)
 *   - MicroSD Card Module
 *
 * Connections:
 *   ESP32 GPIO 34  ← pH Sensor (ADC)
 *   ESP32 GPIO 4   ← DS18B20 (OneWire)
 *   ESP32 GPIO 35  ← Water Level Sensor (ADC)
 *   ESP32 GPIO 27  ← YF-S201 Flow Sensor (interrupt)
 *   ESP32 GPIO 26  → Relay Module (pump)
 *   ESP32 GPIO 25  → Buzzer
 *   ESP32 GPIO 5   → SD Card CS
 *
 * Libraries required:
 *   - Blynk by Volodymyr Shymanskyy
 *   - DallasTemperature
 *   - OneWire
 *   - SD (built-in)
 *
 * Author: Fares Elsherif
 * Date:   2025
 ******************************************************************************/

#include "src/config.h"
#include "src/sensors.h"
#include "src/blynk_handler.h"
#include "src/data_logger.h"

Sensors sensors;
BlynkHandler blynk;
DataLogger logger;

unsigned long lastSensorRead = 0;
unsigned long lastBlynkUpdate = 0;
bool systemOK = false;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(500);
    Serial.println();
    Serial.println(F("======================================"));
    Serial.println(F("  AquaSmart — Smart Fish Farm v1.0"));
    Serial.println(F("======================================"));

    if (!sensors.begin()) {
        Serial.println(F("[FATAL] Sensor initialization failed!"));
    }

    pinMode(PUMP_CONTROL_PIN, OUTPUT);
    digitalWrite(PUMP_CONTROL_PIN, LOW);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    logger.begin();
    blynk.connect();

    systemOK = true;
    Serial.println(F("[SETUP] System ready"));
}

void loop() {
    blynk.run();
    unsigned long now = millis();

    if (now - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
        lastSensorRead = now;

        SensorReadings data = sensors.readAll();
        logger.log(data);

        if (now - lastBlynkUpdate >= BLYNK_UPDATE_INTERVAL_MS) {
            lastBlynkUpdate = now;
            blynk.update(data);
        }

        checkThresholds(data);
    }
}

void checkThresholds(const SensorReadings& data) {
    bool alert = false;

    if (data.pH < PH_MIN || data.pH > PH_MAX) {
        Serial.println(F("[ALERT] pH out of range"));
        alert = true;
    }

    if (data.temperature < TEMP_MIN_C || data.temperature > TEMP_MAX_C) {
        Serial.println(F("[ALERT] Temperature out of range"));
        alert = true;
    }

    if (data.waterLevel < WATER_LEVEL_MIN_CM) {
        Serial.println(F("[ALERT] Water level low — activating pump"));
        digitalWrite(PUMP_CONTROL_PIN, HIGH);
        alert = true;
    } else {
        digitalWrite(PUMP_CONTROL_PIN, LOW);
    }

    if (data.dissolvedOxygen < DO_MIN_MG_L) {
        Serial.println(F("[ALERT] Dissolved oxygen low"));
        digitalWrite(PUMP_CONTROL_PIN, HIGH);
        alert = true;
    }

    if (alert) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
    }
}
