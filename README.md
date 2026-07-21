# ESP32-S3 Sonar-Radar

Kleine, stationaere Sonar-Radarstation mit einem Freenove ESP32-S3 WROOM, HC-SR04 und einem 28BYJ-48-Schrittmotor mit ULN2003-Treiber. Der Kopf tastet einen Bereich von 0 bis 90 Grad ab und stellt Echos sowohl im seriellen Monitor als auch auf einer lokalen Weboberflaeche dar.

## Funktionen

- 90-Grad-Scan mit Rueckfahrt zur sicheren Startposition
- Mehrfachmessung des HC-SR04 fuer robustere Fernbereichs-Echos
- ASCII-Sonarkarte im seriellen Monitor (115200 Baud)
- Lokaler WLAN-Hotspot mit animierter Sonar-Weboberflaeche
- Keine externe Internetverbindung erforderlich

## Verkabelung

| Bauteil | Anschluss am ESP32-S3 |
| --- | --- |
| HC-SR04 VCC | 5 V |
| HC-SR04 GND | GND |
| HC-SR04 TRIG | GPIO 8 |
| HC-SR04 ECHO | GPIO 9 ueber Spannungsteiler |
| ULN2003 IN1 | GPIO 14 |
| ULN2003 IN2 | GPIO 13 |
| ULN2003 IN3 | GPIO 12 |
| ULN2003 IN4 | GPIO 11 |
| ULN2003 GND | Gemeinsame Masse mit ESP32-S3 |
| ULN2003 5 V | Externe, stabile 5-V-Motorversorgung |

### Echo-Spannungsteiler

Der Echo-Ausgang des HC-SR04 liefert meist 5 V und darf nicht direkt an einen ESP32-GPIO angeschlossen werden.

```text
HC-SR04 ECHO --- 1 kOhm --- GPIO 9
                              |
                            2 kOhm
                              |
                             GND
```

> Motor nie am 3V3-Pin betreiben. ESP32-GND und Motorversorgungs-GND muessen verbunden sein. Zwei unabhängige 5-V-Quellen nicht parallel zusammenschalten.

## Starten

1. Den Kopf vor dem Einschalten manuell auf eine sichere Startposition stellen.
2. Projekt in VS Code mit PlatformIO oeffnen und auf das Board hochladen.
3. Den seriellen Monitor bei **115200 Baud** oeffnen. Dort erscheinen Boot-, Mess- und WLAN-Informationen.
4. Mit Handy oder PC das WLAN **`ESP32-Sonar`** verwenden. Passwort: **`sonar-radar`**.
5. Im Browser `http://192.168.4.1` aufrufen. Eine moegliche Warnung "Kein Internet" ist bei diesem lokalen Hotspot normal.

WLAN-Name und Passwort stehen in [`src/radar_config.h`](src/radar_config.h) und sollten fuer den eigenen Aufbau geaendert werden.

## Projektstruktur

```text
src/
  main.cpp              Initialisierung und Hauptschleife
  radar_config.h        Pins, Scan- und WLAN-Konfiguration
  radar_sensor.*        HC-SR04-Messung und Filterung
  stepper_motor.*       ULN2003-/28BYJ-48-Ansteuerung
  radar_sweep.*         Scanablauf und Winkelposition
  sonar_map.*           Sonarkarte und Messdaten fuer die Web-API
  radar_web_server.*    WLAN-Hotspot und HTTP-Endpunkte
  web_page.h            Eingebettete Sonar-Weboberflaeche
```

## Hinweise zur Reichweite

Die effektive HC-SR04-Reichweite haengt stark von Material, Groesse und Ausrichtung des Objekts ab. Glatte oder schräge Flaechen reflektieren Ultraschall oft am Sensor vorbei. Die Firmware misst pro Winkel mehrfach; das verbessert die Erkennung schwacher Echos, verlaengert aber die Dauer eines Scans.
