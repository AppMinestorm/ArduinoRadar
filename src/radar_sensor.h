#pragma once

#include <Arduino.h>

class RadarSensor {
public:
  RadarSensor(uint8_t trigPin, uint8_t echoPin, uint16_t maxDistanceCm);

  void begin();
  int readDistanceCm();

private:
  // Ein weiterer Versuch und eine groessere Pause zwischen den Triggern
  // verbessern die Chance auf schwache Echos im Fernbereich.
  static constexpr uint8_t SAMPLE_COUNT = 4;
  static constexpr uint32_t ECHO_TIMEOUT_US = 30000;
  static constexpr uint8_t INTER_SAMPLE_DELAY_MS = 35;

  uint8_t trigPin_;
  uint8_t echoPin_;
  uint16_t maxDistanceCm_;

  unsigned long measureEchoDurationUs();
};
