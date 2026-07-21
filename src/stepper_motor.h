#pragma once

#include <Arduino.h>

class StepperMotor {
public:
  StepperMotor(uint8_t in1Pin, uint8_t in2Pin, uint8_t in3Pin, uint8_t in4Pin);

  void begin();
  void step(int8_t direction);
  void release();

private:
  uint8_t pins_[4];
  uint8_t coilIndex_ = 0;

  void writeCoilPattern(uint8_t pattern);
};
