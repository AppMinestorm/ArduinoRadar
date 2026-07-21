#include "radar_web_server.h"

#include <WiFi.h>

#include "radar_config.h"
#include "web_page.h"

RadarWebServer::RadarWebServer(RadarSweep &sweep, SonarMap &sonarMap)
    : server_(RadarConfig::WEB_SERVER_PORT), sweep_(sweep), sonarMap_(sonarMap)
{
}

void RadarWebServer::begin(Stream &output)
{
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(RadarConfig::WIFI_AP_SSID, RadarConfig::WIFI_AP_PASSWORD)) {
    output.println("WEB: WLAN-Hotspot konnte nicht gestartet werden");
    return;
  }

  server_.on("/", HTTP_GET, [this]() { handleRoot(); });
  server_.on("/api/scan", HTTP_GET, [this]() { handleScanData(); });
  server_.onNotFound([this]() { handleRoot(); });
  server_.begin();

  output.println("WEB: WLAN-Hotspot bereit");
  output.print("WEB: SSID: ");
  output.println(RadarConfig::WIFI_AP_SSID);
  output.print("WEB: http://");
  output.println(WiFi.softAPIP());
}

void RadarWebServer::update()
{
  server_.handleClient();
}

void RadarWebServer::handleRoot()
{
  server_.send_P(200, "text/html; charset=utf-8", SONAR_WEB_PAGE);
}

void RadarWebServer::handleScanData()
{
  String response;
  response.reserve(512);
  response += "{\"scan\":";
  response += sweep_.scanNumber();
  response += ",\"angle\":";
  response += sweep_.currentAngleDeg();
  response += ",\"forward\":";
  response += (sweep_.isMovingForward() ? "true" : "false");
  response += ",\"echoes\":";
  response += sonarMap_.validSampleCount();
  response += ",\"closest\":";
  response += sonarMap_.closestDistanceCm();
  response += ",\"samples\":";
  sonarMap_.appendSamplesJson(response);
  response += '}';

  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json", response);
}
