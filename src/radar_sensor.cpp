#include "radar_sensor.h"

RadarSensor::RadarSensor(uint8_t trigPin, uint8_t echoPin, uint16_t maxDistanceCm)
    : trigPin_(trigPin), echoPin_(echoPin), maxDistanceCm_(maxDistanceCm)
{
}

void RadarSensor::begin()
{
  pinMode(trigPin_, OUTPUT);
  pinMode(echoPin_, INPUT);
  digitalWrite(trigPin_, LOW);
}

unsigned long RadarSensor::measureEchoDurationUs()
{
  digitalWrite(trigPin_, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin_, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_, LOW);

  return pulseInLong(echoPin_, HIGH, ECHO_TIMEOUT_US);
}

int RadarSensor::readDistanceCm()
{
  int distances[SAMPLE_COUNT];
  uint8_t validSamples = 0;

  for (uint8_t sample = 0; sample < SAMPLE_COUNT; ++sample) {
    const unsigned long echoDurationUs = measureEchoDurationUs();
    const int distanceCm = echoDurationUs / 58;

    if (echoDurationUs > 0 && distanceCm > 0 && distanceCm <= maxDistanceCm_) {
      distances[validSamples++] = distanceCm;
    }
    delay(INTER_SAMPLE_DELAY_MS);
  }

  if (validSamples == 0) {
    return -1;
  }

  for (uint8_t left = 0; left + 1 < validSamples; ++left) {
    for (uint8_t right = left + 1; right < validSamples; ++right) {
      if (distances[right] < distances[left]) {
        const int temporary = distances[left];
        distances[left] = distances[right];
        distances[right] = temporary;
      }
    }
  }

  return distances[validSamples / 2];
}
