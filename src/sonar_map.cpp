#include "sonar_map.h"

#include <string.h>

SonarMap::SonarMap(uint16_t maxDistanceCm) : maxDistanceCm_(maxDistanceCm)
{
}

void SonarMap::clear()
{
  memset(cells_, 0, sizeof(cells_));
  memset(distanceByAngle_, 0, sizeof(distanceByAngle_));
  validSampleCount_ = 0;
  closestDistanceCm_ = 0;
}

void SonarMap::addHeat(int row, int column, uint8_t heat)
{
  if (row < 0 || row >= RadarConfig::SONAR_RANGE_BINS || column < 0 ||
      column >= RadarConfig::SONAR_ANGLE_BINS) {
    return;
  }

  if (heat > cells_[row][column]) {
    cells_[row][column] = heat;
  }
}

void SonarMap::recordSample(uint8_t angleDeg, int distanceCm)
{
  if (distanceCm < 1 || distanceCm > maxDistanceCm_) {
    return;
  }

  ++validSampleCount_;
  if (closestDistanceCm_ == 0 || distanceCm < closestDistanceCm_) {
    closestDistanceCm_ = distanceCm;
  }

  const int column = map(angleDeg, 0, RadarConfig::SWEEP_DEGREES, 0,
                         RadarConfig::SONAR_ANGLE_BINS - 1);
  const int row = map(distanceCm, 0, maxDistanceCm_, RadarConfig::SONAR_RANGE_BINS - 1, 0);
  distanceByAngle_[column] = distanceCm;

  // Ein kleiner, weich auslaufender Fleck macht einzelne Messungen gut lesbar.
  for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
    for (int columnOffset = -2; columnOffset <= 2; ++columnOffset) {
      const uint8_t distanceFromCenter = abs(rowOffset) + abs(columnOffset);
      const int heat = MAX_HEAT - distanceFromCenter;
      if (heat > 0) {
        addHeat(row + rowOffset, column + columnOffset, heat);
      }
    }
  }
}

char SonarMap::symbolForHeat(uint8_t heat) const
{
  if (heat >= 5) return '@';
  if (heat == 4) return '#';
  if (heat == 3) return 'O';
  if (heat == 2) return 'o';
  if (heat == 1) return '.';
  return ' ';
}

char SonarMap::displaySymbol(uint8_t row, uint8_t column) const
{
  const char contact = symbolForHeat(cells_[row][column]);
  if (contact != ' ') {
    return contact;
  }

  // Reichweitenringe alle 50 cm und Radiallinien alle 15 Grad.
  const bool rangeRing = row % 5 == 0;
  const bool radialLine = column % 10 == 0;
  if (rangeRing && radialLine) return '+';
  if (rangeRing) return '-';
  if (radialLine) return '|';
  return ' ';
}

void SonarMap::printRangeLabel(Stream &output, uint8_t row) const
{
  const int distanceCm = map(row, 0, RadarConfig::SONAR_RANGE_BINS - 1, maxDistanceCm_, 0);

  if (row % 5 == 0 || row == RadarConfig::SONAR_RANGE_BINS - 1) {
    if (distanceCm < 10) output.print("  ");
    else if (distanceCm < 100) output.print(" ");
    output.print(distanceCm);
    output.print(" |");
  } else {
    output.print("     |");
  }
}

void SonarMap::render(Stream &output, uint16_t scanNumber) const
{
  output.println();
  output.println("    .=============================================================.");
  output.println("    |                 U-BOOT SONAR // AKTIV                      |");
  output.print("    | Scan ");
  output.print(scanNumber);
  output.print("  | Sektor: 000-");
  output.print(RadarConfig::SWEEP_DEGREES);
  output.print(" DEG | Echos: ");
  output.print(validSampleCount_);
  output.println(" |");
  output.print("    | Naechster Kontakt: ");
  if (closestDistanceCm_ == 0) {
    output.print("keiner");
  } else {
    output.print(closestDistanceCm_);
    output.print(" cm");
  }
  output.println("                                      |");
  output.println("    '============================================================='");
  output.println("       0 DEG     15        30        45        60        75      90");
  output.println(" 300cm +-------------------------------------------------------------+");

  for (uint8_t row = 0; row < RadarConfig::SONAR_RANGE_BINS; ++row) {
    printRangeLabel(output, row);
    for (uint8_t column = 0; column < RadarConfig::SONAR_ANGLE_BINS; ++column) {
      output.print(displaySymbol(row, column));
    }
    output.println('|');
  }

  output.println("       +-------------------------------------------------------------+");
  output.println("        BUG -> Suchsektor 0 bis 90 Grad");
  output.println("        @ Kontaktzentrum  # / O / o / . Echo-Staerke");
}

void SonarMap::appendSamplesJson(String &output) const
{
  output += '[';
  for (uint8_t column = 0; column < RadarConfig::SONAR_ANGLE_BINS; ++column) {
    if (column > 0) {
      output += ',';
    }
    output += distanceByAngle_[column];
  }
  output += ']';
}

uint8_t SonarMap::validSampleCount() const
{
  return validSampleCount_;
}

uint16_t SonarMap::closestDistanceCm() const
{
  return closestDistanceCm_;
}
