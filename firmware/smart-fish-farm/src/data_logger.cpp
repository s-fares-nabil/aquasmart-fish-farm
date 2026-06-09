#include "data_logger.h"
#include "config.h"

DataLogger::DataLogger() {
    sdAvailable = false;
    lastLogTime = 0;
}

bool DataLogger::begin() {
    Serial.println("[LOGGER] Initializing SD card...");
    if (!SD.begin(SS)) {
        Serial.println("[LOGGER] SD card not found - logging to Serial only");
        sdAvailable = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("[LOGGER] No SD card attached");
        sdAvailable = false;
        return false;
    }

    Serial.printf("[LOGGER] SD Card Type: %s\n",
        cardType == CARD_MMC ? "MMC" :
        cardType == CARD_SD ? "SDSC" :
        cardType == CARD_SDHC ? "SDHC" : "UNKNOWN");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("[LOGGER] SD Card Size: %llu MB\n", cardSize);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        sprintf(filename, "/aquasmart_%04d%02d%02d.csv",
            timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    } else {
        strcpy(filename, "/aquasmart_data.csv");
    }

    logFile = SD.open(filename, FILE_APPEND);
    if (!logFile) {
        Serial.println("[LOGGER] Failed to open file for writing");
        sdAvailable = false;
        return false;
    }

    if (logFile.size() == 0) {
        logFile.println(getCSVHeader());
    }
    logFile.close();

    sdAvailable = true;
    Serial.printf("[LOGGER] Logging to %s\n", filename);
    return true;
}

String DataLogger::getCSVHeader() {
    return "Timestamp,pH,Temperature_C,WaterLevel_cm,FlowRate_Lmin,DissolvedOxygen_mgL,WaterLevelPercent";
}

void DataLogger::log(const SensorReadings& data) {
    unsigned long now = millis();
    if (now - lastLogTime < DATA_LOG_INTERVAL_MS && lastLogTime != 0) {
        return;
    }
    lastLogTime = now;

    char buf[256];
    snprintf(buf, sizeof(buf), "%lu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
        data.timestamp, data.pH, data.temperature,
        data.waterLevel, data.flowRate, data.dissolvedOxygen,
        data.waterLevelPercent);

    String line = String(buf);
    writeLine(line);
    printToSerial(data);
}

void DataLogger::writeLine(const String& line) {
    if (sdAvailable) {
        logFile = SD.open(filename, FILE_APPEND);
        if (logFile) {
            logFile.println(line);
            logFile.close();
        } else {
            Serial.println("[LOGGER] Error opening file for append");
        }
    }
}

void DataLogger::printToSerial(const SensorReadings& data) {
    Serial.println("+-------------------+------------------+");
    Serial.println("| Parameter         | Value            |");
    Serial.println("+-------------------+------------------+");
    Serial.printf("| pH                | %8.2f          |\n", data.pH);
    Serial.printf("| Temperature       | %8.2f °C       |\n", data.temperature);
    Serial.printf("| Water Level       | %8.2f cm       |\n", data.waterLevel);
    Serial.printf("| Flow Rate         | %8.2f L/min    |\n", data.flowRate);
    Serial.printf("| Dissolved Oxygen  | %8.2f mg/L     |\n", data.dissolvedOxygen);
    Serial.printf("| Water Level %%     | %8.2f %%       |\n", data.waterLevelPercent);
    Serial.printf("| Valid             | %8s          |\n", data.valid ? "Yes" : "No");
    Serial.println("+-------------------+------------------+");
}
