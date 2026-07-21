#include <Arduino.h>

#include "radar_config.h"
#include "radar_sensor.h"
#include "radar_sweep.h"
#include "radar_web_server.h"
#include "sonar_map.h"
#include "stepper_motor.h"

namespace {
HardwareSerial &console = Serial0;

RadarSensor radar(RadarConfig::ULTRASONIC_TRIG_PIN, RadarConfig::ULTRASONIC_ECHO_PIN,
                  RadarConfig::MAX_DISTANCE_CM);
StepperMotor motor(RadarConfig::STEPPER_IN1_PIN, RadarConfig::STEPPER_IN2_PIN,
                    RadarConfig::STEPPER_IN3_PIN, RadarConfig::STEPPER_IN4_PIN);
SonarMap sonarMap(RadarConfig::MAX_DISTANCE_CM);
RadarSweep radarSweep(radar, motor, sonarMap);
RadarWebServer webServer(radarSweep, sonarMap);
}

void setup()
{
  console.begin(115200);
  // Dem seriellen Monitor kurz Zeit zum Oeffnen geben. Das ist besonders
  // hilfreich, wenn er direkt nach dem Upload gestartet wird.
  delay(1000);
  console.println();
  console.println("BOOT: ESP32-S3 Radar startet");
  console.println("Serielle Ausgabe: UART0 / CH343 bei 115200 Baud");
  console.flush();

  radar.begin();
  motor.begin();
  radarSweep.begin(console);
  webServer.begin(console);
}

void loop()
{
  radarSweep.update(console);
  webServer.update();
}
