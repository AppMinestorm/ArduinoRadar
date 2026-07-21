#pragma once

#include <Arduino.h>

#include "radar_config.h"
#include "radar_sensor.h"
#include "sonar_map.h"
#include "stepper_motor.h"

class RadarSweep {
public:
  RadarSweep(RadarSensor &sensor, StepperMotor &motor, SonarMap &sonarMap);

  void begin(Stream &output);
  void update(Stream &output);
  uint8_t currentAngleDeg() const;
  uint16_t scanNumber() const;
  bool isMovingForward() const;

private:
  RadarSensor &sensor_;
  StepperMotor &motor_;
  SonarMap &sonarMap_;

  uint16_t positionSteps_ = 0;
  uint16_t stepsSinceSample_ = 0;
  uint16_t scanNumber_ = 1;
  uint32_t lastStepUs_ = 0;
  int8_t direction_ = RadarConfig::FORWARD_DIRECTION;

  uint16_t stepsBetweenSamples() const;
  void sampleCurrentPosition(Stream &output);
  void finishSweep(Stream &output);
};
