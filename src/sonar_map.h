#pragma once

#include <Arduino.h>

#include "radar_config.h"

class SonarMap {
public:
  explicit SonarMap(uint16_t maxDistanceCm);

  void clear();
  void recordSample(uint8_t angleDeg, int distanceCm);
  void render(Stream &output, uint16_t scanNumber) const;
  void appendSamplesJson(String &output) const;
  uint8_t validSampleCount() const;
  uint16_t closestDistanceCm() const;

private:
  static constexpr uint8_t MAX_HEAT = 5;

  uint16_t maxDistanceCm_;
  uint8_t cells_[RadarConfig::SONAR_RANGE_BINS][RadarConfig::SONAR_ANGLE_BINS] = {};
  uint16_t distanceByAngle_[RadarConfig::SONAR_ANGLE_BINS] = {};
  uint8_t validSampleCount_ = 0;
  uint16_t closestDistanceCm_ = 0;

  void addHeat(int row, int column, uint8_t heat);
  char symbolForHeat(uint8_t heat) const;
  char displaySymbol(uint8_t row, uint8_t column) const;
  void printRangeLabel(Stream &output, uint8_t row) const;
};
