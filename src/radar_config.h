#pragma once

#include <Arduino.h>

namespace RadarConfig {
constexpr uint8_t ULTRASONIC_TRIG_PIN = 8;
constexpr uint8_t ULTRASONIC_ECHO_PIN = 9;
constexpr uint16_t MAX_DISTANCE_CM = 300;

// ULN2003: Diese Reihenfolge entspricht der Beschriftung IN1 bis IN4 auf dem Treiber.
constexpr uint8_t STEPPER_IN1_PIN = 14;
constexpr uint8_t STEPPER_IN2_PIN = 13;
constexpr uint8_t STEPPER_IN3_PIN = 12;
constexpr uint8_t STEPPER_IN4_PIN = 11;

// Standardwert fuer einen 5-V-28BYJ-48 im Vollschrittmodus.
constexpr uint16_t STEPS_PER_REVOLUTION = 2048;
constexpr uint8_t SWEEP_DEGREES = 90;
constexpr uint16_t SWEEP_STEPS = (STEPS_PER_REVOLUTION * SWEEP_DEGREES) / 360;
constexpr uint32_t STEP_INTERVAL_US = 8000;
constexpr int8_t FORWARD_DIRECTION = 1;

constexpr uint8_t SONAR_ANGLE_BINS = 61;
// 31 Zeilen ergeben bei 0 bis 300 cm exakte 50-cm-Reichweitenringe.
constexpr uint8_t SONAR_RANGE_BINS = 31;

// Das Radar erzeugt sein eigenes WLAN. Passwort vor dem Einsatz ausserhalb
// eines privaten Testaufbaus unbedingt aendern.
constexpr char WIFI_AP_SSID[] = "ESP32-Sonar";
constexpr char WIFI_AP_PASSWORD[] = "sonar-radar";
constexpr uint16_t WEB_SERVER_PORT = 80;
}
