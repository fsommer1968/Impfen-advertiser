# Migration von Version 1.0 zu Version 2.0

Dieser Leitfaden hilft Ihnen beim Upgrade von Impfen-Advertiser V1 auf V2.

## 📋 Übersicht der Änderungen

### Hauptunterschiede

| Aspekt | Version 1.0 | Version 2.0 |
|--------|-------------|-------------|
| Konfiguration | Im Hauptcode | Separate `config.h` |
| Dokumentation | Minimal | Vollständig |
| Fehlerbehandlung | Basic | Erweitert |
| Logging | Einfach | Mit Debug-Modi |
| Code-Struktur | Monolithisch | Modular |
| Namenskonvention | Gemischt | Einheitlich |

## 🔄 Migrations-Schritte

### Schritt 1: Backup erstellen

**Wichtig:** Sichern Sie Ihre aktuelle Installation!

```bash
# Backup des alten Codes
cp impfen-Advertiser.ino impfen-Advertiser.ino.backup

# Backup der Statistik-Datei (falls vorhanden)
# Über /statistik herunterladen und lokal speichern
```

### Schritt 2: Alte Konfiguration dokumentieren

Notieren Sie Ihre aktuellen Einstellungen aus dem alten Code:

```cpp
// Aus impfen-Advertiser.ino (Zeilen 6-11)
static const char authUser[] = "???";
static const char authPassword[] = "???";
static const char otaPassword[] = "???";
static const char localssid[] = "???";
static const char localpassword[] = "???";
```

### Schritt 3: Neue Version installieren

1. **Dateien herunterladen:**
   - `impfen-advertiser-v2.ino`
   - `config.h.example`

2. **Konfiguration erstellen:**
   ```bash
   cp config.h.example config.h
   ```

3. **Einstellungen übertragen:**
   Öffnen Sie `config.h` und tragen Sie Ihre Werte ein:

   ```cpp
   // Authentifizierung (aus altem authUser/authPassword)
   #define AUTH_USER "IhrAlterUsername"
   #define AUTH_PASSWORD "IhrAltesPasswort"
   #define OTA_PASSWORD "IhrAltesOTAPasswort"
   
   // Lokales WLAN (aus altem localssid/localpassword)
   #define LOCAL_SSID "IhreAlteSSID"
   #define LOCAL_PASSWORD "IhrAltesWLANPasswort"
   ```

### Schritt 4: SSID-Botschaften anpassen

**Alt (V1):**
```cpp
char ssid_txt[MAX_BOTSCHAFT][32] = {
    {"Test"},
    {"Nicht Impfen tötet!"},
    // ...
};
```

**Neu (V2):**
```cpp
// In config.h
const char SSID_MESSAGES[MAX_BOTSCHAFT][32] PROGMEM = {
    "Test",
    "Nicht Impfen tötet!",
    // ...
};
```

**Änderungen:**
- Keine geschweiften Klammern mehr um einzelne Strings
- `PROGMEM` für Speicheroptimierung
- In `config.h` statt Hauptdatei

### Schritt 5: Neue Features konfigurieren

V2 bietet neue Konfigurationsoptionen:

```cpp
// Debug-Modus (neu in V2)
#define DEBUG_MODE true
#define VERBOSE_LOGGING true

// Erweiterte WLAN-Einstellungen
#define WIFI_CONNECT_TIMEOUT 10000
#define MIN_WIFI_CHANNEL 1
#define MAX_WIFI_CHANNEL 13

// Features aktivieren/deaktivieren
#define ENABLE_CAPTIVE_PORTAL true
#define ENABLE_OTA true
#define USE_RTC true
```

### Schritt 6: Code hochladen

1. **Arduino IDE öffnen**
2. **Board-Einstellungen prüfen:**
   - Board: NodeMCU 1.0 (oder Ihr Modell)
   - Flash Size: 4MB (FS:2MB OTA:~1019KB)
   - Upload Speed: 115200

3. **Kompilieren und hochladen:**
   - Sketch → Hochladen
   - Seriellen Monitor öffnen (74880 Baud)

4. **Boot-Meldungen prüfen:**
   ```
   ========================================
   INFO: Impfen-Advertiser v2.0.0
   ========================================
   INFO: Initialisiere Filesystem...
   INFO: Filesystem bereit
   ...
   ```

### Schritt 7: Funktionalität testen

1. **Status-Seite aufrufen:**
   ```
   http://172.217.28.1/status
   ```
   Oder im lokalen Modus:
   ```
   http://<ESP-IP>/status
   ```

2. **Statistik prüfen:**
   ```
   http://172.217.28.1/statistik
   ```
   (Mit Ihren Zugangsdaten)

3. **SSID-Wechsel beobachten:**
   - Seriellen Monitor beobachten
   - Nach 90-240 Sekunden sollte SSID wechseln

## 🔍 Verhaltensunterschiede

### Logging-Format

**V1:**
```
05.02.2022 14:58:30 --- Mozilla/5.0... --- /generate_204
```

**V2:**
```
05.02.2022 14:58:30 ## 192.168.4.2 ## Mozilla/5.0... ## /generate_204
```

**Änderung:** Trennzeichen von `---` zu `##` und zusätzliche IP-Adresse

### Serielle Ausgabe

**V1:** Minimal
```
AP Name: Impfen schützt!
IP-Adresse: 172.217.28.1
```

**V2:** Detailliert
```
========================================
INFO: SSID-Wechsel
INFO: Neue SSID: Impfen schützt!
INFO: Neuer Kanal: 6
INFO: Dauer: 157 Sekunden
========================================
```

### Neue Endpunkte

V2 fügt hinzu:
- `/status` - System-Status anzeigen
- Verbesserte Fehlerseiten
- Bessere Authentifizierungs-Meldungen

## ⚠️ Breaking Changes

### 1. Konfigurationsdatei erforderlich

**Problem:** Code kompiliert nicht ohne `config.h`

**Lösung:**
```bash
cp config.h.example config.h
# Dann config.h anpassen
```

### 2. SSID-Array-Format geändert

**Problem:** Alte SSID-Definitionen funktionieren nicht

**Alt:**
```cpp
char ssid_txt[17][32] = {{"Test"}, {"Botschaft"}};
```

**Neu:**
```cpp
const char SSID_MESSAGES[17][32] PROGMEM = {"Test", "Botschaft"};
```

### 3. Variablennamen geändert

| Alt (V1) | Neu (V2) |
|----------|----------|
| `has_rtc` | `hasRTC` |
| `wifilocal` | `isLocalWiFiConnected` |
| `looptime` | `looptime` (unverändert) |

### 4. Konstanten statt Magic Numbers

**Alt:**
```cpp
if (secsSince1900 < 10)
```

**Neu:**
```cpp
if (secsSince1900 < MIN_VALID_NTP_TIME)
```

## 🐛 Häufige Migrationsprobleme

### Problem 1: Kompilierungsfehler "config.h not found"

**Fehler:**
```
fatal error: config.h: No such file or directory
```

**Lösung:**
```bash
cp config.h.example config.h
```

### Problem 2: SSID-Array-Fehler

**Fehler:**
```
error: invalid conversion from 'const char*' to 'char'
```

**Lösung:** SSID-Format in `config.h` anpassen (siehe Schritt 4)

### Problem 3: Alte Statistik nicht lesbar

**Problem:** Log-Format hat sich geändert

**Lösung:**
1. Alte Statistik über `/statistik` herunterladen
2. Lokal speichern
3. Nach Migration: `/purge` aufrufen
4. Neue Statistik wird im neuen Format angelegt

### Problem 4: OTA funktioniert nicht mehr

**Problem:** OTA-Passwort wird nicht erkannt

**Lösung:**
1. `OTA_PASSWORD` in `config.h` prüfen
2. Im lokalen WLAN-Modus sein
3. Firewall-Einstellungen prüfen

## 📊 Vergleich der Funktionen

### Beibehaltene Features
✅ SSID-Rotation  
✅ Captive Portal  
✅ NTP-Zeitsynchronisation  
✅ RTC-Unterstützung  
✅ Statistik-Logging  
✅ OTA-Updates  
✅ Dual-Mode (AP/Client)  

### Neue Features in V2
🆕 Status-Endpunkt  
🆕 Debug-Modi  
🆕 Verbose Logging  
🆕 Bessere Fehlerbehandlung  
🆕 Konfigurierbare Timeouts  
🆕 Erweiterte Dokumentation  

### Entfernte Features
❌ Keine (vollständig rückwärtskompatibel)

## 🔧 Optimierungen nach Migration

### 1. Debug-Modus deaktivieren

Für Produktivbetrieb:
```cpp
#define DEBUG_MODE false
#define VERBOSE_LOGGING false
```

### 2. Sendeleistung anpassen

Je nach Einsatzort:
```cpp
// Innenraum
#define WLAN_AP_POWER 15.0f

// Außenbereich
#define WLAN_AP_POWER 20.5f
```

### 3. Timing optimieren

```cpp
// Schnellerer Wechsel
#define MIN_LOOP_TIME_SEC 60
#define MAX_LOOP_TIME_SEC 120

// Langsamerer Wechsel
#define MIN_LOOP_TIME_SEC 180
#define MAX_LOOP_TIME_SEC 300
```

### 4. Log-Größe begrenzen

```cpp
// Maximale Log-Größe (0 = unbegrenzt)
#define MAX_LOG_SIZE 100000  // 100 KB
```

## 📝 Checkliste nach Migration

- [ ] Backup der alten Version erstellt
- [ ] `config.h` erstellt und angepasst
- [ ] SSID-Botschaften übertragen
- [ ] Code erfolgreich hochgeladen
- [ ] Serieller Monitor zeigt V2.0.0
- [ ] Status-Seite erreichbar
- [ ] Statistik funktioniert
- [ ] SSID-Wechsel funktioniert
- [ ] OTA-Updates getestet (optional)
- [ ] Alte Statistik gesichert

## 🆘 Rollback zu V1

Falls Probleme auftreten:

1. **Alten Code wiederherstellen:**
   ```bash
   cp impfen-Advertiser.ino.backup impfen-Advertiser.ino
   ```

2. **In Arduino IDE hochladen**

3. **Alte Statistik wiederherstellen** (falls gesichert)

## 📞 Support

Bei Migrationsproblemen:

- **GitHub Issues:** [Link zu Issues]
- **Dokumentation:** Siehe `DOKUMENTATION_V2_VORSCHLAEGE.md`
- **README:** Siehe `README_V2.md`

## ✅ Erfolgreiche Migration

Nach erfolgreicher Migration sollten Sie sehen:

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

========================================
INFO: SSID-Wechsel
INFO: Neue SSID: Ihre Botschaft
INFO: Neuer Kanal: 6
INFO: Dauer: 157 Sekunden
========================================
```

**Herzlichen Glückwunsch! Die Migration war erfolgreich! 🎉**