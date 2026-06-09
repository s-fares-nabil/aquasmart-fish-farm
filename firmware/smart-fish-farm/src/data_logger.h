#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include "sensors.h"

class DataLogger {
public:
    DataLogger();
    bool begin();
    void log(const SensorReadings& data);
    String getCSVHeader();
    void printToSerial(const SensorReadings& data);

private:
    bool sdAvailable;
    unsigned long lastLogTime;
    char filename[32];
    File logFile;
    void writeLine(const String& line);
};

#endif
