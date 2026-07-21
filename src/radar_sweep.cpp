#include "radar_sweep.h"

RadarSweep::RadarSweep(RadarSensor &sensor, StepperMotor &motor, SonarMap &sonarMap)
    : sensor_(sensor), motor_(motor), sonarMap_(sonarMap)
{
}

void RadarSweep::begin(Stream &output)
{
  sonarMap_.clear();
  output.println();
  output.println("ESP32-S3 Sonar-Radar gestartet");
  output.println("Startposition = sicherer Rand (0 Grad). Ein Testscan geht bis 90 Grad und zurueck.");
  output.println("Eine Karte erscheint nach jeder 90-Grad-Bewegung.");
  sampleCurrentPosition(output);
  lastStepUs_ = micros();
}

uint16_t RadarSweep::stepsBetweenSamples() const
{
  return RadarConfig::SWEEP_STEPS / (RadarConfig::SONAR_ANGLE_BINS - 1);
}

uint8_t RadarSweep::currentAngleDeg() const
{
  return map(positionSteps_, 0, RadarConfig::SWEEP_STEPS, 0, RadarConfig::SWEEP_DEGREES);
}

uint16_t RadarSweep::scanNumber() const
{
  return scanNumber_;
}

bool RadarSweep::isMovingForward() const
{
  return direction_ == RadarConfig::FORWARD_DIRECTION;
}

void RadarSweep::sampleCurrentPosition(Stream &output)
{
  const int distanceCm = sensor_.readDistanceCm();
  const uint8_t angleDeg = currentAngleDeg();

  // Diese Zeile erscheint auch vor dem ersten fertigen 90-Grad-Scan. Damit
  // lassen sich ein falscher Monitor-Port und ein fehlendes Sensor-Echo klar
  // voneinander unterscheiden.
  output.print("Messung: ");
  output.print(angleDeg);
  output.print(" Grad, ");
  if (distanceCm < 0) {
    output.println("kein Echo (ECHO/Spannungsteiler und gemeinsame Masse pruefen)");
  } else {
    output.print(distanceCm);
    output.println(" cm");
  }

  sonarMap_.recordSample(angleDeg, distanceCm);
}

void RadarSweep::finishSweep(Stream &output)
{
  sonarMap_.render(output, scanNumber_++);
  sonarMap_.clear();
  stepsSinceSample_ = 0;
  direction_ = -direction_;
}

void RadarSweep::update(Stream &output)
{
  const uint32_t nowUs = micros();
  if (nowUs - lastStepUs_ < RadarConfig::STEP_INTERVAL_US) {
    return;
  }

  lastStepUs_ = nowUs;
  motor_.step(direction_);
  positionSteps_ = static_cast<uint16_t>(static_cast<int>(positionSteps_) + direction_);
  ++stepsSinceSample_;

  const bool reachedEnd = positionSteps_ == 0 || positionSteps_ == RadarConfig::SWEEP_STEPS;
  if (stepsSinceSample_ >= stepsBetweenSamples() || reachedEnd) {
    sampleCurrentPosition(output);
    stepsSinceSample_ = 0;
  }

  if (reachedEnd) {
    finishSweep(output);
  }
}
