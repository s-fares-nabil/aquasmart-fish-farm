#include "blynk_handler.h"
#include "config.h"

BlynkHandler::BlynkHandler() {
    connected = false;
    lastBlynkUpdate = 0;
}

bool BlynkHandler::checkWiFi() {
    return WiFi.status() == WL_CONNECTED;
}

bool BlynkHandler::connect() {
    if (checkWiFi()) {
        return true;
    }

    Serial.printf("[BLYNK] Connecting to WiFi %s...\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[BLYNK] WiFi connected. IP: %s\n", WiFi.localIP().toString().c_str());
        Blynk.config(BLYNK_AUTH_TOKEN);
        if (Blynk.connect()) {
            connected = true;
            Serial.println("[BLYNK] Connected to Blynk server");
            return true;
        }
    }

    Serial.println("[BLYNK] Connection failed, running in offline mode");
    connected = false;
    return false;
}

void BlynkHandler::update(const SensorReadings& data) {
    if (!connected) {
        if (checkWiFi()) {
            Blynk.config(BLYNK_AUTH_TOKEN);
            connected = Blynk.connect();
        }
        return;
    }

    Blynk.virtualWrite(V0, data.pH);
    Blynk.virtualWrite(V1, data.temperature);
    Blynk.virtualWrite(V2, data.waterLevel);
    Blynk.virtualWrite(V3, data.flowRate);
    Blynk.virtualWrite(V4, data.dissolvedOxygen);
    Blynk.virtualWrite(V5, data.waterLevelPercent);

    Blynk.execute();

    if (data.pH < PH_MIN || data.pH > PH_MAX) {
        sendAlert("WARNING: pH level out of safe range!");
    }
    if (data.temperature < TEMP_MIN_C || data.temperature > TEMP_MAX_C) {
        sendAlert("WARNING: Water temperature out of safe range!");
    }
    if (data.waterLevel < WATER_LEVEL_MIN_CM) {
        sendAlert("WARNING: Water level critically low!");
    }
    if (data.dissolvedOxygen < DO_MIN_MG_L) {
        sendAlert("WARNING: Dissolved oxygen level too low!");
    }
}

void BlynkHandler::sendAlert(const char* message) {
    if (connected) {
        Blynk.virtualWrite(V6, message);
        Blynk.logEvent("water_alert", message);
        Serial.printf("[ALERT] %s\n", message);
    }
}

bool BlynkHandler::isConnected() {
    connected = Blynk.connected();
    return connected;
}

void BlynkHandler::run() {
    if (connected) {
        Blynk.run();
    }
}
