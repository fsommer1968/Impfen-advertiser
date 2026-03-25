# 🏥 Impfen-Advertiser Version 2.0

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/yourusername/impfen-advertiser)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP8266-orange.svg)](https://www.espressif.com/)

Ein ESP8266-basiertes Projekt, das sich als WLAN-Hotspot verhält und statischen Web-Content ausliefert. Durch zufällige SSID-Rotation werden Botschaften zum Thema Impfen (oder beliebige andere Inhalte) verbreitet.

## 📋 Inhaltsverzeichnis

- [Features](#-features)
- [Hardware-Anforderungen](#-hardware-anforderungen)
- [Installation](#-installation)
- [Konfiguration](#-konfiguration)
- [Verwendung](#-verwendung)
- [API-Endpunkte](#-api-endpunkte)
- [Funktionsweise](#-funktionsweise)
- [Troubleshooting](#-troubleshooting)
- [Changelog](#-changelog)
- [Lizenz](#-lizenz)

## ✨ Features

### Kernfunktionen
- 🔄 **Automatische SSID-Rotation**: Wechselt alle 90-240 Sekunden die SSID
- 📡 **Captive Portal**: Leitet alle DNS-Anfragen auf den Webserver um
- 🌐 **Dual-Mode**: Arbeitet als Access Point oder verbindet sich mit lokalem WLAN
- 📊 **Statistik-Logging**: Erfasst Verbindungen mit Zeitstempel
- 🔐 **Authentifizierung**: Geschützter Zugriff auf Admin-Funktionen
- 🔧 **OTA-Updates**: Firmware-Updates über WLAN ohne USB-Kabel

### Technische Features
- ⏰ **NTP-Zeitsynchronisation**: Automatische Zeitabfrage von NTP-Servern
- 🕐 **RTC-Unterstützung**: Optional DS3231 für präzise Zeitstempel
- 📁 **Filesystem**: Statische Dateien aus LittleFS ausliefern
- 🔀 **Kanal-Wechsel**: Automatischer WLAN-Kanal-Wechsel (1-13)
- 📝 **Umfangreiche Dokumentation**: Inline-Kommentare und Doxygen-Style

### Version 2.0 Verbesserungen
- ✅ Vollständig dokumentierter Code
- ✅ Modularisierte Funktionen
- ✅ Konfiguration in separater Datei
- ✅ Verbesserte Fehlerbehandlung
- ✅ Einheitliche Namenskonventionen
- ✅ Debug- und Logging-Modi
- ✅ Status-Seite für Monitoring

## 🔧 Hardware-Anforderungen

### Minimal-Konfiguration
- **ESP8266** (z.B. NodeMCU, Wemos D1 Mini, ESP-12E)
- **Flash-Speicher**: Mindestens 2MB (für Filesystem)
- **USB-Kabel**: Zur Programmierung
- **Stromversorgung**: 5V via USB oder 3.3V direkt

### Empfohlene Konfiguration
- **ESP8266** mit 4MB Flash
- **DS3231 RTC-Modul** (für präzise Zeitstempel ohne WLAN)
- **Externe WLAN-Antenne** (für bessere Reichweite)
- **Gehäuse** (für Outdoor-Einsatz)

### Optionale Hardware

#### DS3231 RTC-Modul
Für präzise Zeitstempel auch ohne WLAN-Verbindung.

**Verkabelung:**
```
ESP8266          DS3231
--------         ------
D6 (GPIO12)  →   SDA
D7 (GPIO13)  →   SCL
3.3V         →   VCC
GND          →   GND
```

**Hinweis:** Die I2C-Pins können in `config.h` angepasst werden.

## 📥 Installation

### 1. Arduino IDE vorbereiten

#### ESP8266 Board installieren
1. Arduino IDE öffnen
2. **Datei** → **Voreinstellungen**
3. Zusätzliche Boardverwalter-URLs hinzufügen:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. **Werkzeuge** → **Board** → **Boardverwalter**
5. "ESP8266" suchen und installieren

#### Erforderliche Bibliotheken
Über **Sketch** → **Bibliothek einbinden** → **Bibliotheken verwalten**:

- `ESP8266WiFi` (in ESP8266 Core enthalten)
- `DNSServer` (in ESP8266 Core enthalten)
- `ESP8266WebServer` (in ESP8266 Core enthalten)
- `LittleFS` (in ESP8266 Core enthalten)
- `ArduinoOTA` (in ESP8266 Core enthalten)
- `Wire` (in ESP8266 Core enthalten)
- `RTClib` von Adafruit (für DS3231 RTC)
- `TimeLib` von Paul Stoffregen

### 2. Projekt herunterladen

```bash
git clone https://github.com/yourusername/impfen-advertiser.git
cd impfen-advertiser
```

### 3. Konfiguration erstellen

```bash
cp config.h.example config.h
```

Öffnen Sie `config.h` und passen Sie die Werte an:

```cpp
// Authentifizierung
#define AUTH_USER "admin"
#define AUTH_PASSWORD "IhrSicheresPasswort"
#define OTA_PASSWORD "IhrOTAPasswort"

// Lokales WLAN
#define LOCAL_SSID "IhrWLANName"
#define LOCAL_PASSWORD "IhrWLANPasswort"
```

### 4. Board-Einstellungen

**Werkzeuge** → Board-Einstellungen:
- **Board**: "NodeMCU 1.0 (ESP-12E Module)" oder entsprechend
- **Flash Size**: "4MB (FS:2MB OTA:~1019KB)"
- **CPU Frequency**: "80 MHz"
- **Upload Speed**: "115200"
- **Port**: Entsprechenden COM-Port auswählen

### 5. Hochladen

1. **Sketch** → **Hochladen**
2. Warten bis "Hochladen abgeschlossen" erscheint
3. **Werkzeuge** → **Serieller Monitor** öffnen (74880 Baud)

### 6. Filesystem hochladen (optional)

Für statische HTML-Dateien:

1. **Werkzeuge** → **ESP8266 Sketch Data Upload**
2. Dateien im `data/` Ordner werden hochgeladen

## ⚙️ Konfiguration

### Wichtige Parameter in config.h

#### Authentifizierung
```cpp
#define AUTH_USER "admin"              // Benutzername für /statistik
#define AUTH_PASSWORD "changeme123"    // Passwort für Admin-Bereich
#define OTA_PASSWORD "otapassword123"  // Passwort für OTA-Updates
```

#### WLAN-Einstellungen
```cpp
#define LOCAL_SSID "MeinWLAN"          // Lokales WLAN für Administration
#define LOCAL_PASSWORD "passwort"      // Passwort des lokalen WLANs
#define WLAN_LOCAL_POWER 10.5f         // Sendeleistung lokal (dBm)
#define WLAN_AP_POWER 20.5f            // Sendeleistung AP-Modus (dBm)
```

#### Timing
```cpp
#define MIN_LOOP_TIME_SEC 90           // Min. Zeit pro SSID (Sekunden)
#define MAX_LOOP_TIME_SEC 240          // Max. Zeit pro SSID (Sekunden)
```

#### SSID-Botschaften
```cpp
const char SSID_MESSAGES[MAX_BOTSCHAFT][32] PROGMEM = {
    "Ihre Botschaft 1",
    "Ihre Botschaft 2",
    // ... bis zu 17 Botschaften
};
```

**Wichtig:** `MAX_BOTSCHAFT` muss mit der Anzahl der Botschaften übereinstimmen!

#### Debug-Modus
```cpp
#define DEBUG_MODE true                // Debug-Ausgaben aktivieren
#define VERBOSE_LOGGING true           // Detaillierte Logs
```

## 🚀 Verwendung

### Erster Start

1. ESP8266 mit Strom versorgen
2. Seriellen Monitor öffnen (74880 Baud)
3. Bootmeldungen beobachten:
   ```
   ========================================
   INFO: Impfen-Advertiser v2.0.0
   ========================================
   INFO: Initialisiere Filesystem...
   INFO: Filesystem bereit
   INFO: Suche RTC...
   INFO: RTC gefunden
   INFO: Konfiguriere Webserver...
   INFO: Webserver gestartet
   INFO: OTA aktiviert
   INFO: Setup abgeschlossen
   ========================================
   ```

### Betriebsmodi

#### Access Point Modus (Standard)
- Wird aktiviert wenn lokales WLAN nicht erreichbar ist
- ESP8266 erstellt eigenen Hotspot mit zufälliger SSID
- IP-Adresse: `172.217.28.1`
- Captive Portal leitet alle Anfragen auf Webserver um

#### Lokaler Modus
- Verbindet sich mit konfiguriertem WLAN
- Ermöglicht Administration und OTA-Updates
- IP-Adresse wird per DHCP bezogen

### SSID-Rotation

Das System wechselt automatisch:
- **SSID**: Zufällige Auswahl aus konfigurierten Botschaften
- **Kanal**: Zufällig zwischen 1-13 (Europa)
- **Dauer**: Zufällig zwischen 90-240 Sekunden

Beispiel-Ausgabe:
```
========================================
INFO: SSID-Wechsel
INFO: Neue SSID: Impfen schützt!
INFO: Neuer Kanal: 6
INFO: Dauer: 157 Sekunden
========================================
```

## 🌐 API-Endpunkte

### Öffentliche Endpunkte

#### `GET /`
Standard-HTML-Seite mit Impf-Botschaft

**Response:** HTML-Seite

#### `GET /status`
Zeigt aktuellen System-Status

**Response:** HTML mit:
- Firmware-Version
- Aktuelle SSID
- WLAN-Kanal
- Betriebsmodus
- RTC-Status
- Uptime

**Beispiel:**
```
http://172.217.28.1/status
```

#### `GET /<dateiname>`
Liefert Dateien aus dem Filesystem

**Unterstützte Formate:**
- HTML, CSS, JavaScript
- Bilder (PNG, JPG, GIF)
- PDF, ZIP

**Beispiel:**
```
http://172.217.28.1/impfung.html
```

### Geschützte Endpunkte

Erfordern HTTP Digest Authentication mit konfigurierten Zugangsdaten.

#### `GET /statistik`
Zeigt gesammelte Zugriffs-Statistiken

**Authentifizierung:** Erforderlich

**Response:** Text-Datei mit Log-Einträgen

**Format:**
```
Datum/Zeit ## IP-Adresse ## User-Agent ## URI
05.02.2022 14:58:30 ## 192.168.4.2 ## Mozilla/5.0... ## /
```

**Beispiel:**
```
http://172.217.28.1/statistik
```

#### `GET /purge`
Löscht die Statistik-Datei

**Authentifizierung:** Erforderlich

**Response:** `Statistik gelöscht`

**Beispiel:**
```
http://172.217.28.1/purge
```

## 🔍 Funktionsweise

### Ablauf beim Start

1. **Initialisierung**
   - Serielle Schnittstelle starten
   - Filesystem mounten
   - RTC suchen und konfigurieren
   - Webserver-Routen einrichten

2. **WLAN-Konfiguration**
   - Versuch lokales WLAN zu verbinden
   - Bei Erfolg: NTP-Zeit synchronisieren
   - Bei Fehler: Access Point starten

3. **Hauptschleife**
   - Timer überwachen
   - Bei Ablauf: Neue SSID und Kanal wählen
   - OTA-Updates verarbeiten
   - DNS-Anfragen umleiten (AP-Modus)
   - HTTP-Anfragen bearbeiten

### Zeitverwaltung

```
┌─────────────────────────────────────┐
│  Zeitquelle-Hierarchie              │
├─────────────────────────────────────┤
│  1. NTP-Server (wenn WLAN verfügbar)│
│  2. DS3231 RTC (wenn vorhanden)     │
│  3. Keine Zeit verfügbar            │
└─────────────────────────────────────┘
```

**NTP-Synchronisation:**
- Erfolgt nur im lokalen WLAN-Modus
- Aktualisiert automatisch die RTC
- Berücksichtigt Sommerzeit (MESZ/MEZ)

**RTC-Fallback:**
- Wird verwendet wenn NTP nicht verfügbar
- Behält Zeit auch bei Stromausfall
- Wird bei NTP-Erfolg aktualisiert

### Captive Portal

Im Access Point Modus:
1. DNS-Server fängt alle Anfragen ab
2. Leitet auf ESP8266-IP um (`172.217.28.1`)
3. Webserver liefert Content aus
4. Anfrage wird geloggt

### Logging

Jede HTTP-Anfrage wird protokolliert:
```
Zeitstempel ## IP-Adresse ## User-Agent ## URI
```

**Gespeicherte Informationen:**
- Datum/Zeit (wenn verfügbar)
- Lokale IP des Clients
- Browser/Gerät (User-Agent)
- Angeforderte URL

**Datenschutz:**
- Keine personenbezogenen Daten
- Keine MAC-Adressen
- Keine Tracking-Cookies

## 🐛 Troubleshooting

### Problem: ESP8266 bootet nicht

**Symptome:**
- Keine serielle Ausgabe
- LED blinkt nicht

**Lösungen:**
1. USB-Kabel prüfen (Daten-Kabel, nicht nur Ladekabel)
2. Anderen USB-Port versuchen
3. Treiber installieren (CH340/CP2102)
4. Flash-Button beim Booten gedrückt halten

### Problem: Lokales WLAN wird nicht gefunden

**Symptome:**
```
WARN: Lokale WLAN-Verbindung fehlgeschlagen
```

**Lösungen:**
1. SSID und Passwort in `config.h` prüfen
2. WLAN-Reichweite prüfen
3. 2.4 GHz WLAN verwenden (nicht 5 GHz!)
4. WLAN-Kanal im Router ändern (1, 6 oder 11)

### Problem: Filesystem-Fehler

**Symptome:**
```
ERROR: Filesystem-Initialisierung fehlgeschlagen
```

**Lösungen:**
1. Flash-Größe in Arduino IDE prüfen
2. "Flash Size" mit Filesystem wählen (z.B. "4MB (FS:2MB)")
3. Filesystem formatieren: `LittleFS.format()`
4. Sketch neu hochladen

### Problem: RTC nicht gefunden

**Symptome:**
```
WARN: RTC nicht gefunden
```

**Lösungen:**
1. Verkabelung prüfen (SDA, SCL, VCC, GND)
2. I2C-Scanner verwenden
3. Pull-Up-Widerstände prüfen (4.7kΩ)
4. Andere I2C-Pins in `config.h` definieren

### Problem: OTA-Update schlägt fehl

**Symptome:**
- Update startet nicht
- Verbindung bricht ab

**Lösungen:**
1. Im lokalen WLAN-Modus sein
2. OTA-Passwort prüfen
3. Firewall-Einstellungen prüfen
4. Genug freien Flash-Speicher haben
5. Stabile Stromversorgung sicherstellen

### Problem: Webserver antwortet nicht

**Symptome:**
- Keine Verbindung möglich
- Timeout-Fehler

**Lösungen:**
1. IP-Adresse prüfen (172.217.28.1 im AP-Modus)
2. Mit richtigem SSID verbunden sein
3. Firewall auf Client deaktivieren
4. Serielle Ausgabe auf Fehler prüfen

### Debug-Tipps

**Serielle Ausgabe aktivieren:**
```cpp
#define DEBUG_MODE true
#define VERBOSE_LOGGING true
```

**Nützliche Serial-Befehle:**
```cpp
Serial.println(WiFi.localIP());      // IP-Adresse anzeigen
Serial.println(WiFi.SSID());         // Verbundenes WLAN
Serial.println(WiFi.RSSI());         // Signalstärke
Serial.println(ESP.getFreeHeap());   // Freier RAM
```

## 📊 Changelog

### Version 2.0.0 (2026-03-25)

#### Neu
- ✅ Vollständige Code-Dokumentation mit Doxygen-Style
- ✅ Separate Konfigurationsdatei (`config.h`)
- ✅ Status-Endpunkt für Monitoring
- ✅ Verbesserte Fehlerbehandlung
- ✅ Debug- und Verbose-Modi
- ✅ Einheitliche Namenskonventionen
- ✅ Konstanten statt Magic Numbers

#### Verbessert
- ✅ Modularisierte Funktionen
- ✅ Bessere Logging-Ausgaben
- ✅ Optimierte NTP-Zeitsynchronisation
- ✅ Robustere RTC-Initialisierung
- ✅ Klarere Struktur und Kommentare

#### Behoben
- ✅ Inkonsistente Variablennamen
- ✅ Fehlende Fehlerbehandlung
- ✅ Auskommentierter Code entfernt
- ✅ Speicherlecks behoben

### Version 1.0.0 (Original)
- Basis-Funktionalität
- SSID-Rotation
- Captive Portal
- NTP und RTC-Unterstützung
- Statistik-Logging

## 📄 Lizenz

MIT License

Copyright (c) 2026 Frank Sommer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## ⚠️ Haftungsausschluss

Diese Software dient ausschließlich zu **Bildungs- und Informationszwecken**.

**Wichtige Hinweise:**
- Der Betreiber ist für Inhalte und Verwendung selbst verantwortlich
- Beachten Sie lokale Gesetze bezüglich WLAN-Betrieb
- Captive Portals können in manchen Ländern reguliert sein
- Keine Haftung für Schäden oder Missbrauch
- Verwenden Sie das Projekt verantwortungsvoll

## 🔒 Datenschutz

**Gesammelte Daten:**
- Zeitstempel (wenn verfügbar)
- Lokale IP-Adresse (nicht öffentlich)
- User-Agent (Browser/Gerät-Information)
- Angeforderte URL

**NICHT gesammelt:**
- Personenbezogene Daten
- MAC-Adressen
- Standortdaten
- Cookies oder Tracking-IDs

**Speicherung:**
- Lokal auf dem ESP8266
- Keine Cloud-Übertragung
- Manuelles Löschen möglich (`/purge`)

## 🤝 Beitragen

Beiträge sind willkommen! Bitte:

1. Fork das Repository
2. Erstelle einen Feature-Branch (`git checkout -b feature/AmazingFeature`)
3. Commit deine Änderungen (`git commit -m 'Add some AmazingFeature'`)
4. Push zum Branch (`git push origin feature/AmazingFeature`)
5. Öffne einen Pull Request

## 📞 Support

Bei Fragen oder Problemen:

- **Issues**: [GitHub Issues](https://github.com/yourusername/impfen-advertiser/issues)
- **Diskussionen**: [GitHub Discussions](https://github.com/yourusername/impfen-advertiser/discussions)
- **E-Mail**: your.email@example.com

## 🙏 Danksagungen

- ESP8266 Community
- Adafruit für RTClib
- Arduino Team
- Alle Contributor

---

**Hinweis:** Dieses Projekt wurde zu Bildungszwecken erstellt. Verwenden Sie es verantwortungsvoll und im Einklang mit lokalen Gesetzen.