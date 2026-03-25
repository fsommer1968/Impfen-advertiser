# Beitragen zum Impfen-Advertiser Projekt

Vielen Dank für Ihr Interesse, zum Impfen-Advertiser beizutragen! 🎉

## 📋 Inhaltsverzeichnis

- [Code of Conduct](#code-of-conduct)
- [Wie kann ich beitragen?](#wie-kann-ich-beitragen)
- [Entwicklungsumgebung einrichten](#entwicklungsumgebung-einrichten)
- [Pull Request Prozess](#pull-request-prozess)
- [Coding Standards](#coding-standards)
- [Commit-Nachrichten](#commit-nachrichten)
- [Bug Reports](#bug-reports)
- [Feature Requests](#feature-requests)

## Code of Conduct

Dieses Projekt folgt einem Code of Conduct. Durch Ihre Teilnahme verpflichten Sie sich, diesen einzuhalten. Bitte melden Sie inakzeptables Verhalten.

### Unsere Standards

- Respektvoller und inklusiver Umgang
- Konstruktive Kritik
- Fokus auf das Beste für die Community
- Empathie gegenüber anderen Community-Mitgliedern

## Wie kann ich beitragen?

### 🐛 Bugs melden

Bugs werden als [GitHub Issues](https://github.com/yourusername/impfen-advertiser/issues) verfolgt.

**Bevor Sie einen Bug melden:**
- Prüfen Sie, ob der Bug bereits gemeldet wurde
- Stellen Sie sicher, dass Sie die neueste Version verwenden
- Sammeln Sie relevante Informationen

**Guter Bug Report enthält:**
- Klare und beschreibende Überschrift
- Schritte zur Reproduktion
- Erwartetes Verhalten
- Tatsächliches Verhalten
- Screenshots (falls relevant)
- Umgebungsdetails (ESP8266-Modell, Arduino IDE Version, etc.)

**Beispiel:**
```markdown
**Beschreibung:**
SSID wechselt nicht nach konfigurierter Zeit

**Schritte zur Reproduktion:**
1. config.h mit MIN_LOOP_TIME_SEC=60 konfiguriert
2. Code hochgeladen
3. Nach 60 Sekunden keine Änderung

**Erwartetes Verhalten:**
SSID sollte nach 60 Sekunden wechseln

**Tatsächliches Verhalten:**
SSID bleibt unverändert

**Umgebung:**
- ESP8266: NodeMCU v1.0
- Arduino IDE: 1.8.19
- ESP8266 Core: 3.0.2
- Firmware Version: 2.0.0
```

### 💡 Feature Requests

Feature-Vorschläge sind willkommen!

**Guter Feature Request enthält:**
- Klare Beschreibung des Features
- Begründung (warum ist es nützlich?)
- Mögliche Implementierung (optional)
- Beispiele oder Mockups (optional)

### 🔧 Code beitragen

1. **Fork** das Repository
2. **Clone** Ihren Fork
3. **Branch** erstellen (`git checkout -b feature/AmazingFeature`)
4. **Änderungen** vornehmen
5. **Testen** Sie Ihre Änderungen
6. **Commit** (`git commit -m 'Add some AmazingFeature'`)
7. **Push** (`git push origin feature/AmazingFeature`)
8. **Pull Request** öffnen

### 📝 Dokumentation verbessern

Dokumentation ist genauso wichtig wie Code!

- Tippfehler korrigieren
- Erklärungen verbessern
- Beispiele hinzufügen
- Übersetzungen beitragen

## Entwicklungsumgebung einrichten

### Voraussetzungen

- Arduino IDE 1.8.x oder 2.x
- ESP8266 Board Support
- Git

### Setup

```bash
# Repository forken und clonen
git clone https://github.com/IhrUsername/impfen-advertiser.git
cd impfen-advertiser

# Konfiguration erstellen
cp config.h.example config.h

# config.h mit Ihren Testdaten anpassen
nano config.h
```

### Bibliotheken installieren

Über Arduino IDE Library Manager:
- RTClib (Adafruit)
- TimeLib (Paul Stoffregen)

### Testen

```bash
# Code kompilieren (ohne Upload)
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 impfen-advertiser-v2.ino

# Auf ESP8266 hochladen
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2 impfen-advertiser-v2.ino
```

## Pull Request Prozess

### Vor dem PR

- [ ] Code kompiliert ohne Fehler
- [ ] Auf echter Hardware getestet
- [ ] Dokumentation aktualisiert
- [ ] Commit-Nachrichten sind aussagekräftig
- [ ] Code folgt den Coding Standards

### PR Beschreibung

Gute PR-Beschreibung enthält:

```markdown
## Änderungen
- Feature X hinzugefügt
- Bug Y behoben
- Dokumentation Z aktualisiert

## Motivation
Warum sind diese Änderungen notwendig?

## Testing
Wie wurden die Änderungen getestet?

## Screenshots
(falls relevant)

## Checklist
- [x] Code kompiliert
- [x] Auf Hardware getestet
- [x] Dokumentation aktualisiert
- [x] Keine Breaking Changes (oder dokumentiert)
```

### Review-Prozess

1. Maintainer prüfen den PR
2. Feedback wird gegeben
3. Änderungen werden vorgenommen
4. Nach Approval wird gemerged

## Coding Standards

### Allgemein

- **Sprache**: Code in Englisch, Kommentare in Deutsch
- **Einrückung**: 4 Spaces (keine Tabs)
- **Zeilenlänge**: Max. 100 Zeichen
- **Encoding**: UTF-8

### Namenskonventionen

```cpp
// Konstanten: UPPER_CASE
#define MAX_CONNECTIONS 10
const int BUFFER_SIZE = 256;

// Variablen: camelCase
int connectionCount = 0;
bool isConnected = false;

// Funktionen: camelCase
void handleRequest() { }
String getFormattedTime() { }

// Klassen: PascalCase
class NetworkManager { };
class TimeManager { };
```

### Kommentare

```cpp
/**
 * @brief Kurze Beschreibung der Funktion
 * 
 * Detaillierte Beschreibung was die Funktion macht,
 * wie sie verwendet wird, etc.
 * 
 * @param param1 Beschreibung des Parameters
 * @param param2 Beschreibung des Parameters
 * @return Beschreibung des Rückgabewerts
 * 
 * @note Wichtige Hinweise
 * @warning Warnungen
 * @see Verwandte Funktionen
 */
int myFunction(int param1, String param2) {
    // Inline-Kommentar für komplexe Logik
    return result;
}
```

### Code-Struktur

```cpp
// 1. Includes
#include <ESP8266WiFi.h>

// 2. Defines und Konstanten
#define MAX_VALUE 100

// 3. Globale Variablen
int globalCounter = 0;

// 4. Funktionsdeklarationen (falls nötig)
void helperFunction();

// 5. Setup
void setup() {
    // Initialisierung
}

// 6. Loop
void loop() {
    // Hauptlogik
}

// 7. Hilfsfunktionen
void helperFunction() {
    // Implementation
}
```

### Fehlerbehandlung

```cpp
// Immer Fehler prüfen
if (!WiFi.begin(ssid, password)) {
    Serial.println(F("ERROR: WiFi-Initialisierung fehlgeschlagen"));
    return false;
}

// Debug-Ausgaben verwenden
if (DEBUG_MODE) {
    Serial.println(F("INFO: Verbindung hergestellt"));
}
```

### Speicheroptimierung

```cpp
// PROGMEM für konstante Strings
const char HTML_HEADER[] PROGMEM = "<!DOCTYPE html>";

// F() Makro für Flash-Strings
Serial.println(F("Nachricht"));

// String-Reserve für bekannte Größen
String response;
response.reserve(500);
```

## Commit-Nachrichten

### Format

```
<typ>: <kurze Beschreibung>

<detaillierte Beschreibung (optional)>

<footer (optional)>
```

### Typen

- `feat`: Neues Feature
- `fix`: Bug-Fix
- `docs`: Dokumentation
- `style`: Formatierung, keine Code-Änderung
- `refactor`: Code-Umstrukturierung
- `test`: Tests hinzufügen/ändern
- `chore`: Build-Prozess, Tools, etc.

### Beispiele

```bash
# Gute Commit-Nachrichten
git commit -m "feat: Status-Endpunkt hinzugefügt"
git commit -m "fix: SSID-Wechsel nach Timeout korrigiert"
git commit -m "docs: README mit Installationsanleitung erweitert"
git commit -m "refactor: NTP-Code in separate Funktion ausgelagert"

# Schlechte Commit-Nachrichten
git commit -m "update"
git commit -m "fix bug"
git commit -m "changes"
```

### Detaillierte Beschreibung

```bash
git commit -m "feat: OTA-Update-Unterstützung hinzugefügt

- ArduinoOTA-Bibliothek integriert
- Passwort-Schutz implementiert
- Callbacks für Progress-Anzeige
- Dokumentation in README ergänzt

Closes #42"
```

## Bug Reports

### Template

```markdown
**Beschreibung:**
Kurze Beschreibung des Problems

**Schritte zur Reproduktion:**
1. Schritt 1
2. Schritt 2
3. ...

**Erwartetes Verhalten:**
Was sollte passieren?

**Tatsächliches Verhalten:**
Was passiert stattdessen?

**Screenshots:**
(falls relevant)

**Umgebung:**
- ESP8266 Modell: NodeMCU v1.0
- Arduino IDE Version: 1.8.19
- ESP8266 Core Version: 3.0.2
- Firmware Version: 2.0.0
- Betriebssystem: Windows 10

**Zusätzlicher Kontext:**
Weitere relevante Informationen

**Serielle Ausgabe:**
```
Relevante Log-Ausgaben hier einfügen
```
```

## Feature Requests

### Template

```markdown
**Feature-Beschreibung:**
Klare Beschreibung des gewünschten Features

**Problem/Motivation:**
Welches Problem löst dieses Feature?

**Vorgeschlagene Lösung:**
Wie könnte das Feature implementiert werden?

**Alternativen:**
Welche Alternativen wurden in Betracht gezogen?

**Zusätzlicher Kontext:**
Screenshots, Mockups, Links, etc.
```

## Fragen?

- **GitHub Discussions**: Für allgemeine Fragen
- **GitHub Issues**: Für Bugs und Features
- **E-Mail**: your.email@example.com

## Danke!

Vielen Dank für Ihren Beitrag zum Impfen-Advertiser Projekt! 🙏

Jeder Beitrag, egal wie klein, wird geschätzt und hilft das Projekt zu verbessern.