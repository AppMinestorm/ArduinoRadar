#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "radar_sweep.h"
#include "sonar_map.h"

class RadarWebServer {
public:
  RadarWebServer(RadarSweep &sweep, SonarMap &sonarMap);

  void begin(Stream &output);
  void update();

private:
  WebServer server_;
  RadarSweep &sweep_;
  SonarMap &sonarMap_;

  void handleRoot();
  void handleScanData();
};
