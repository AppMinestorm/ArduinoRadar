#include "stepper_motor.h"

namespace {
// Vollschrittfolge fuer einen ULN2003 mit einem 28BYJ-48.
constexpr uint8_t COIL_PATTERNS[] = {0b0001, 0b0010, 0b0100, 0b1000};
constexpr uint8_t PATTERN_COUNT = sizeof(COIL_PATTERNS) / sizeof(COIL_PATTERNS[0]);
}

StepperMotor::StepperMotor(uint8_t in1Pin, uint8_t in2Pin, uint8_t in3Pin, uint8_t in4Pin)
    : pins_{in1Pin, in2Pin, in3Pin, in4Pin}
{
}

void StepperMotor::begin()
{
  for (const uint8_t pin : pins_) {
    pinMode(pin, OUTPUT);
  }
  release();
}

void StepperMotor::writeCoilPattern(uint8_t pattern)
{
  for (uint8_t index = 0; index < 4; ++index) {
    digitalWrite(pins_[index], (pattern & (1 << index)) ? HIGH : LOW);
  }
}

void StepperMotor::step(int8_t direction)
{
  if (direction > 0) {
    coilIndex_ = (coilIndex_ + 1) % PATTERN_COUNT;
  } else {
    coilIndex_ = (coilIndex_ + PATTERN_COUNT - 1) % PATTERN_COUNT;
  }

  writeCoilPattern(COIL_PATTERNS[coilIndex_]);
}

void StepperMotor::release()
{
  writeCoilPattern(0);
}
