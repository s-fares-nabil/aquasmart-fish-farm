#ifndef BLYNK_HANDLER_H
#define BLYNK_HANDLER_H

#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include "sensors.h"

class BlynkHandler {
public:
    BlynkHandler();
    bool connect();
    void update(const SensorReadings& data);
    void sendAlert(const char* message);
    bool isConnected();
    void run();

private:
    unsigned long lastBlynkUpdate;
    bool connected;
    bool checkWiFi();
};

#endif
