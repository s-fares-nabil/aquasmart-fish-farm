# AquaSmart — IoT Smart Fish Farm Monitoring System

[![Platform](https://img.shields.io/badge/Platform-ESP32-blue)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20Core-orange)](https://github.com/espressif/arduino-esp32)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)

A low-cost, IoT-based water quality monitoring system for sustainable aquaculture. Built for Egypt's aquaculture sector to address the grand challenge of water resource management.

## Overview

AquaSmart monitors key water parameters in real-time using an ESP32 microcontroller and a suite of low-cost sensors. Data is visualized on a Blynk dashboard and logged to an SD card for analysis. An alert system notifies farmers of unsafe conditions.

## Features

| Feature | Description |
|---------|-------------|
| **pH Monitoring** | pH-4502C sensor with calibration and moving-average filtering |
| **Temperature** | DS18B20 digital sensor (±0.5°C accuracy) |
| **Water Level** | Analog water level sensor with percentage conversion |
| **Flow Rate** | YF-S201 hall-effect sensor with interrupt-based counting |
| **Dissolved Oxygen** | Calculated from temperature & flow rate using novel equation |
| **Blynk Dashboard** | Real-time widgets, gauges, and push notifications |
| **SD Card Logging** | CSV data logging with auto-rotating daily files |
| **Alert System** | Buzzer + Blynk notifications for out-of-range parameters |
| **Auto Pump Control** | Relay-driven pump activation on low level/low DO |

## Hardware Requirements

| Component | Quantity | Estimated Cost (EGP) |
|-----------|----------|---------------------|
| ESP32 Dev Board | 1 | ~350 |
| pH Sensor (pH-4502C) | 1 | ~450 |
| DS18B20 Temperature Sensor | 1 | ~50 |
| Water Level Sensor | 1 | ~80 |
| YF-S201 Flow Sensor | 1 | ~120 |
| 5V Relay Module | 1 | ~40 |
| Active Buzzer | 1 | ~15 |
| MicroSD Card Module | 1 | ~35 |
| Breadboard & Jumper Wires | 1 set | ~60 |
| **Total** | | **~1,200 EGP** |

## Pin Mapping

```
ESP32 GPIO 34  ← pH Sensor (ADC input)
ESP32 GPIO 4   ← DS18B20 (OneWire data)
ESP32 GPIO 35  ← Water Level Sensor (ADC input)
ESP32 GPIO 27  ← YF-S201 Flow Sensor (interrupt, RISING)
ESP32 GPIO 26  → Relay Module (pump control, active HIGH)
ESP32 GPIO 25  → Buzzer (active HIGH)
ESP32 GPIO 5   → SD Card CS (SPI SS)
```

## Quick Start

### 1. Install PlatformIO (Recommended)

```bash
pip install platformio
```

### 2. Clone & Build

```bash
git clone https://github.com/s-fares-nabil/smart-fish-farm.git
cd smart-fish-farm/firmware/smart-fish-farm
pio run --target upload
```

### 3. Configure Credentials

Edit `src/config.h` with your WiFi and Blynk credentials:

```cpp
#define WIFI_SSID        "your_wifi_ssid"
#define WIFI_PASS        "your_wifi_password"
#define BLYNK_AUTH_TOKEN "your_blynk_auth_token"
```

### 4. Monitor

```bash
pio device monitor
```

## Blynk Dashboard Setup

Create a new Blynk template with these virtual pins:

| Virtual Pin | Parameter | Widget Type |
|-------------|-----------|-------------|
| V0 | pH | Value Display + Gauge |
| V1 | Temperature (°C) | Value Display + Graph |
| V2 | Water Level (cm) | Value Display |
| V3 | Flow Rate (L/min) | Value Display + Graph |
| V4 | Dissolved Oxygen (mg/L) | Value Display + Gauge |
| V5 | Water Level (%) | LED |
| V6 | Alert Messages | Terminal |

## Data Logging

Data is logged to a microSD card in CSV format:

```csv
Timestamp,pH,Temperature_C,WaterLevel_cm,FlowRate_Lmin,DissolvedOxygen_mgL,WaterLevelPercent
0,7.32,26.50,18.20,3.45,6.82,60.67
10000,7.28,26.48,18.15,3.42,6.79,60.50
```

Files are auto-named as `aquasmart_YYYYMMDD.csv`.

## Dissolved Oxygen Calculation

DO is estimated using a novel temperature-flow rate relationship:

```
DO = 14.6 - 0.41 × T + 0.008 × T² + 0.02 × F
```

Where:
- T = Water temperature (°C)
- F = Flow rate (L/min)

This eliminates the need for expensive DO sensors, reducing total system cost.

## Results

Tested over a 7-day period in a 60L aquarium:

| Parameter | Observed Range | Safe Range |
|-----------|---------------|------------|
| pH | 7.0 – 8.2 | 6.5 – 8.5 |
| Temperature | 24.5 – 28.3 °C | 22 – 32 °C |
| Dissolved Oxygen | 4.8 – 6.2 mg/L | > 4.0 mg/L |
| Water Level | 15 – 28 cm | 5 – 30 cm |

## Project Structure

```
smart-fish-farm/
├── firmware/
│   └── smart-fish-farm/
│       ├── src/
│       │   ├── config.h           # WiFi/Blynk config & pin mapping
│       │   ├── sensors.h          # Sensor class header
│       │   ├── sensors.cpp        # Sensor reading implementation
│       │   ├── blynk_handler.h    # Blynk dashboard handler header
│       │   ├── blynk_handler.cpp  # Blynk integration
│       │   ├── data_logger.h      # SD card logger header
│       │   └── data_logger.cpp    # CSV logging implementation
│       ├── smart-fish-farm.ino    # Main firmware entry point
│       └── platformio.ini         # PlatformIO configuration
├── images/                        # Poster images organized by type
│   ├── materials/                 #   Component & hardware photos
│   ├── graphs/                    #   Data charts & visualizations
│   ├── schematics/                #   System & block diagrams
│   ├── tables/                    #   Data tables from testing
│   └── decorative/                #   Background fills & icons
├── poster/                        # Project poster (PPTX)
├── circuit/                       # Circuit diagrams
└── README.md
```

## License

MIT License — see [LICENSE](LICENSE).

## Author

**Fares Elsherif** — Embedded Systems & IoT Developer

---

*This project was developed as part of Egypt's Grand Challenges initiative to advance sustainable aquaculture through technology.*
