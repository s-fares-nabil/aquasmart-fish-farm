#ifndef CONFIG_H
#define CONFIG_H

#define WIFI_SSID        "your_wifi_ssid"
#define WIFI_PASS        "your_wifi_password"
#define BLYNK_AUTH_TOKEN "your_blynk_auth_token"
#define BLYNK_TEMPLATE_ID   "your_template_id"
#define BLYNK_TEMPLATE_NAME "Smart Fish Farm"

#define SERIAL_BAUD 115200
#define SENSOR_READ_INTERVAL_MS 10000
#define BLYNK_UPDATE_INTERVAL_MS 15000
#define DATA_LOG_INTERVAL_MS 60000

#define PH_SENSOR_PIN          34
#define TEMP_SENSOR_PIN        4
#define WATER_LEVEL_SENSOR_PIN 35
#define FLOW_SENSOR_PIN        27
#define PUMP_CONTROL_PIN       26
#define BUZZER_PIN             25

#define PH_OFFSET 0.25
#define PH_SAMPLES 10

#define WATER_LEVEL_MIN_CM 5.0
#define WATER_LEVEL_MAX_CM 30.0
#define PH_MIN 6.5
#define PH_MAX 8.5
#define TEMP_MIN_C 22.0
#define TEMP_MAX_C 32.0
#define DO_MIN_MG_L 4.0

#endif
