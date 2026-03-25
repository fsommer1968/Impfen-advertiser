/**
 * @file impfen-advertiser-v2.ino
 * @brief Impfkampagne Advertiser - Version 2.0
 * 
 * @description
 * Verhält sich wie ein WLAN-Hotspot und liefert statischen Web-Content aus.
 * Per Zufallsgenerator wird eine Botschaft als SSID ausgewählt und für
 * 90 Sekunden bis 4 Minuten aufrechterhalten. Danach werden SSID und
 * WLAN-Kanal gewechselt.
 * 
 * @features
 * - Captive Portal Funktionalität
 * - Zufällige SSID-Rotation mit konfigurierbaren Botschaften
 * - Automatischer WLAN-Kanal-Wechsel
 * - NTP-Zeitsynchronisation mit RTC-Fallback
 * - Statistik-Logging mit Zeitstempel
 * - OTA (Over-The-Air) Update-Unterstützung
 * - Web-Interface für Administration
 * 
 * @hardware
 * - ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
 * - Mindestens 2MB Flash für Filesystem
 * - Optional: DS3231 RTC für präzise Zeitstempel
 * 
 * @author Frank Sommer
 * @version 2.0.0
 * @date 2026-03-25
 * @license MIT
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "RTClib.h"
#include <TimeLib.h>

// Konfigurationsdatei einbinden
#include "config.h"

// ============================================================================
// KONSTANTEN UND DEFINES
// ============================================================================
#define NTP_PACKET_SIZE 48              // NTP-Paketgröße in Bytes
#define NTP_TIMEOUT_MS 350              // Timeout für NTP-Antwort
#define MIN_VALID_NTP_TIME 10           // Minimaler gültiger NTP-Zeitwert
#define NTP_EPOCH_OFFSET 2208988800UL   // Offset zwischen NTP und Unix Epoch
#define WIFI_MODE_SWITCH_DELAY 200      // Verzögerung beim WLAN-Modus-Wechsel
#define MICROSECONDS_PER_SECOND 1000000 // Mikrosekunden pro Sekunde

// ============================================================================
// GLOBALE OBJEKTE
// ============================================================================
IPAddress apIP(AP_IP_OCTET1, AP_IP_OCTET2, AP_IP_OCTET3, AP_IP_OCTET4);
DNSServer dnsServer;
ESP8266WebServer webserver(HTTP_PORT);
RTC_DS3231 dsrtc;
WiFiUDP Udp;

// ============================================================================
// GLOBALE VARIABLEN
// ============================================================================
// Zeitverwaltung
IPAddress timeServer;
byte packetBuffer[NTP_PACKET_SIZE];
unsigned long looptime = 0;
unsigned long startmillis = 0;

// WLAN-Status
uint32_t rnd_ssid = 0;
uint32_t rnd_chnl = 1;
bool hasRTC = false;
bool isLocalWiFiConnected = false;

// SSID-Botschaften (aus config.h kopiert für Laufzeit-Zugriff)
char ssid_txt[MAX_BOTSCHAFT][32];

// ============================================================================
// HILFSFUNKTIONEN - ZEITVERWALTUNG
// ============================================================================

/**
 * @brief Prüft ob Sommerzeit in der EU gilt
 * 
 * Berechnet anhand von Jahr, Monat, Tag und Stunde ob Sommerzeit (MESZ/CEST)
 * oder Normalzeit (MEZ/CET) gilt. Berücksichtigt die EU-Regelung:
 * - Sommerzeit: Letzter Sonntag im März, 2:00 Uhr
 * - Normalzeit: Letzter Sonntag im Oktober, 3:00 Uhr
 * 
 * @param t_year Jahr (z.B. 2026)
 * @param t_month Monat (1-12)
 * @param t_day Tag (1-31)
 * @param t_hour Stunde (0-23)
 * @param tzHours Zeitzone in Stunden (1 für MEZ)
 * @return true wenn Sommerzeit gilt, false sonst
 */
bool summertime_EU(const int& t_year, const int& t_month, const int& t_day, 
                   const int& t_hour, const int& tzHours) {
    // Januar, Februar, November, Dezember: keine Sommerzeit
    if ((t_month < 3) || (t_month > 10)) return false;
    
    // April bis September: immer Sommerzeit
    if ((t_month > 3) && (t_month < 10)) return true;
    
    // März und Oktober: komplexe Berechnung für Umstellungstag
    if (((t_month == 3) && ((t_hour + 24 * t_day) >= (1 + tzHours + 24 * (31 - (5 * t_year / 4 + 4) % 7)))) ||
        ((t_month == 10) && ((t_hour + 24 * t_day) < (1 + tzHours + 24 * (31 - (5 * t_year / 4 + 1) % 7))))) {
        return true;
    }
    
    return false;
}

/**
 * @brief Sendet ein NTP-Anfrage-Paket an den Zeitserver
 * 
 * Erstellt und sendet ein standardkonformes NTP-Paket (RFC 5905) an den
 * angegebenen Zeitserver auf Port 123.
 * 
 * @param address IP-Adresse des NTP-Servers
 */
void sendNTPpacket(IPAddress &address) {
    // Buffer mit Nullen initialisieren
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    
    // NTP-Request-Header aufbauen (siehe RFC 5905)
    packetBuffer[0] = 0b11100011;   // LI=3, Version=4, Mode=3 (Client)
    packetBuffer[1] = 0;            // Stratum
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision
    
    // Root Delay und Root Dispersion (8 Bytes Nullen)
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    
    // NTP-Paket senden
    Udp.beginPacket(address, 123);
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

/**
 * @brief Holt die aktuelle Zeit von NTP-Server oder RTC
 * 
 * Versucht zunächst die Zeit vom NTP-Server zu holen. Bei Fehler oder wenn
 * kein WLAN verfügbar ist, wird die Zeit von der RTC gelesen (falls vorhanden).
 * Die Funktion aktualisiert auch die RTC mit der NTP-Zeit wenn beide verfügbar sind.
 * 
 * @return timeval Struktur mit Sekunden und Mikrosekunden seit Unix Epoch
 *         Bei Fehler: tv_sec = 0, tv_usec = 0
 */
timeval getNtpTime() {
    DateTime dsnow;
    timeval tv_temp;
    tmElements_t tmp_time;
    uint32_t usec = 0;
    unsigned long secsSince1900 = 0;
    
    // NTP-Abfrage nur wenn lokales WLAN verbunden ist
    if (isLocalWiFiConnected) {
        // NTP-Server-Adresse auflösen
        if (!WiFi.hostByName(NTP_SERVER, timeServer)) {
            timeServer = WiFi.gatewayIP();
            if (DEBUG_MODE) {
                Serial.println(F("WARN: NTP-Server nicht gefunden, verwende Gateway"));
            }
        }
        
        // Alte Pakete verwerfen
        while (Udp.parsePacket() > 0);
        
        // NTP-Anfrage senden
        sendNTPpacket(timeServer);
        
        // Auf Antwort warten
        uint32_t beginWait = millis();
        while ((millis() - beginWait < NTP_TIMEOUT_MS)) {
            int sizeudp = Udp.parsePacket();
            yield();
            
            if (sizeudp >= NTP_PACKET_SIZE) {
                Udp.read(packetBuffer, NTP_PACKET_SIZE);
                
                // Zeitstempel aus Bytes 40-43 extrahieren
                secsSince1900 = (unsigned long)packetBuffer[40] << 24;
                secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
                secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
                secsSince1900 |= (unsigned long)packetBuffer[43];
                
                // Bruchteil der Sekunde aus Bytes 44-47 extrahieren
                uint32_t frac = (uint32_t)packetBuffer[44] << 24
                              | (uint32_t)packetBuffer[45] << 16
                              | (uint32_t)packetBuffer[46] << 8
                              | (uint32_t)packetBuffer[47] << 0;
                usec = ((uint64_t)frac * MICROSECONDS_PER_SECOND) >> 32;
                
                break;
            }
        }
        
        // NTP-Antwort verarbeiten
        if (secsSince1900 < MIN_VALID_NTP_TIME) {
            // NTP fehlgeschlagen, versuche RTC
            if (DEBUG_MODE) {
                Serial.println(F("WARN: NTP-Antwort ungültig"));
            }
            
            if (hasRTC) {
                dsnow = dsrtc.now();
                tmp_time.Year = dsnow.year() - 1970;
                tmp_time.Month = dsnow.month();
                tmp_time.Day = dsnow.day();
                tmp_time.Hour = dsnow.hour();
                tmp_time.Minute = dsnow.minute();
                tmp_time.Second = dsnow.second();
                
                secsSince1900 = makeTime(tmp_time) + NTP_EPOCH_OFFSET;
                usec = MICROSECONDS_PER_SECOND / 2; // Halbe Sekunde annehmen
                
                if (DEBUG_MODE) {
                    Serial.println(F("INFO: Zeit von RTC gelesen"));
                }
            } else {
                if (DEBUG_MODE) {
                    Serial.println(F("ERROR: Keine Zeitquelle verfügbar"));
                }
            }
        } else {
            // NTP erfolgreich, RTC aktualisieren falls vorhanden
            if (hasRTC) {
                breakTime((secsSince1900 - NTP_EPOCH_OFFSET), tmp_time);
                tmp_time.Year = tmp_time.Year - 30; // Korrektur für RTC-Chip
                dsrtc.adjust(DateTime(tmp_time.Year, tmp_time.Month, tmp_time.Day,
                                     tmp_time.Hour, tmp_time.Minute, tmp_time.Second));
                
                if (VERBOSE_LOGGING) {
                    Serial.println(F("INFO: RTC mit NTP-Zeit aktualisiert"));
                }
            }
        }
    } else {
        // Kein WLAN, nur RTC verwenden
        if (hasRTC) {
            dsnow = dsrtc.now();
            tmp_time.Year = dsnow.year() - 1970;
            tmp_time.Month = dsnow.month();
            tmp_time.Day = dsnow.day();
            tmp_time.Hour = dsnow.hour();
            tmp_time.Minute = dsnow.minute();
            tmp_time.Second = dsnow.second();
            
            secsSince1900 = makeTime(tmp_time) + NTP_EPOCH_OFFSET;
            usec = MICROSECONDS_PER_SECOND / 2;
            
            if (VERBOSE_LOGGING) {
                Serial.println(F("INFO: Zeit von RTC (kein WLAN)"));
            }
        } else {
            if (DEBUG_MODE) {
                Serial.println(F("ERROR: Keine Zeit verfügbar (kein WLAN, kein RTC)"));
            }
        }
    }
    
    // Rückgabewert vorbereiten
    if (secsSince1900 > MIN_VALID_NTP_TIME) {
        tv_temp.tv_sec = uint32_t(secsSince1900 - NTP_EPOCH_OFFSET);
        tv_temp.tv_usec = usec;
    } else {
        tv_temp.tv_sec = 0;
        tv_temp.tv_usec = 0;
    }
    
    return tv_temp;
}

/**
 * @brief Formatiert einen Zeitstempel als String
 * 
 * @param tformat Format: 0=Standard (DD.MM.YYYY HH:MM:SS), 
 *                        1=ISO8601 mit Zeitzone, 
 *                        2=Standard
 * @param stempel Ausgabe-Buffer (mindestens 32 Bytes)
 * @param zeitstempel_sec Unix-Zeitstempel in Sekunden
 */
void uhrzeitstempel(const byte& tformat, char* stempel, const time_t& zeitstempel_sec) {
    if ((tformat == 0) || (tformat == 2)) {
        sprintf_P(stempel, PSTR("%02d.%02d.%04d %02d:%02d:%02d"),
                  day(zeitstempel_sec), month(zeitstempel_sec), year(zeitstempel_sec),
                  hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
    }
    
    if (tformat == 1) {
        if (summertime_EU(year(zeitstempel_sec), month(zeitstempel_sec),
                         day(zeitstempel_sec), hour(zeitstempel_sec), TIMEZONE_OFFSET)) {
            sprintf_P(stempel, PSTR("%04d-%02d-%02dT%02d:%02d:%02d+02:00"),
                     year(zeitstempel_sec), month(zeitstempel_sec), day(zeitstempel_sec),
                     hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
        } else {
            sprintf_P(stempel, PSTR("%04d-%02d-%02dT%02d:%02d:%02d+01:00"),
                     year(zeitstempel_sec), month(zeitstempel_sec), day(zeitstempel_sec),
                     hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
        }
    }
}

// ============================================================================
// HILFSFUNKTIONEN - WEBSERVER
// ============================================================================

/**
 * @brief Bestimmt den Content-Type basierend auf der Dateiendung
 * 
 * @param filename Dateiname mit Endung
 * @return String mit MIME-Type
 */
String getContentType(String filename) {
    if (filename.endsWith(".htm")) return F("text/html");
    else if (filename.endsWith(".html")) return F("text/html");
    else if (filename.endsWith(".css")) return F("text/css");
    else if (filename.endsWith(".js")) return F("application/javascript");
    else if (filename.endsWith(".png")) return F("image/png");
    else if (filename.endsWith(".gif")) return F("image/gif");
    else if (filename.endsWith(".jpg")) return F("image/jpeg");
    else if (filename.endsWith(".ico")) return F("image/x-icon");
    else if (filename.endsWith(".xml")) return F("text/xml");
    else if (filename.endsWith(".pdf")) return F("application/x-pdf");
    else if (filename.endsWith(".zip")) return F("application/x-zip");
    else if (filename.endsWith(".gz")) return F("application/x-gzip");
    return F("text/plain");
}

/**
 * @brief Sammelt Informationen über die aktuelle HTTP-Anfrage
 * 
 * Erstellt einen Log-String mit Zeitstempel, IP-Adresse, User-Agent und URI
 * 
 * @return String mit formatierten Request-Informationen
 */
String sammle_requestinfos() {
    String requestdate, requestagent, requestip;
    time_t tv = getNtpTime().tv_sec;
    
    // Zeitstempel formatieren
    if (tv > MIN_VALID_NTP_TIME) {
        char tmp_timestring[32];
        if (summertime_EU(year(tv), month(tv), day(tv), hour(tv), TIMEZONE_OFFSET)) {
            uhrzeitstempel(2, tmp_timestring, tv + 7200); // MESZ: +2 Stunden
        } else {
            uhrzeitstempel(2, tmp_timestring, tv + 3600); // MEZ: +1 Stunde
        }
        requestdate = String(tmp_timestring);
    } else {
        // Fallback: aktuelle SSID als Zeitstempel
        requestdate = ssid_txt[rnd_ssid];
    }
    
    // IP-Adresse des Clients
    requestip = webserver.client().remoteIP().toString();
    
    // User-Agent auslesen
    if (webserver.hasHeader("User-Agent")) {
        requestagent = webserver.header("User-Agent");
    } else {
        requestagent = F("UNKNOWN");
    }
    
    // Log-String zusammenbauen
    String requestresult = requestdate + LOG_SEPARATOR + 
                          requestip + LOG_SEPARATOR + 
                          requestagent + LOG_SEPARATOR + 
                          webserver.uri();
    
    return requestresult;
}

/**
 * @brief Sendet die Standard-HTML-Antwort
 * 
 * Wird aufgerufen wenn keine passende Datei im Filesystem gefunden wurde
 */
void notFound() {
    webserver.send(200, F("text/html"), F(DEFAULT_HTML_RESPONSE));
}

/**
 * @brief Handler für unbekannte URLs
 * 
 * Versucht die angeforderte Datei aus dem Filesystem zu laden.
 * Falls nicht vorhanden, wird die Standard-HTML-Seite ausgeliefert.
 * Alle Anfragen werden geloggt.
 */
void handleUnknown() {
    String filename = webserver.uri();
    
    if (VERBOSE_LOGGING) {
        Serial.print(F("INFO: Anfrage für: "));
        Serial.println(filename);
    }
    
    File pageFile = LittleFS.open(filename, "r");
    
    // Datei ausliefern (außer Statistik-Datei und Root)
    if ((pageFile) && (filename.indexOf("requests.txt") == -1) && (filename != "/")) {
        String contentTyp = getContentType(filename);
        size_t sent = webserver.streamFile(pageFile, contentTyp);
        pageFile.close();
        
        if (VERBOSE_LOGGING) {
            Serial.print(F("INFO: Datei gesendet, Bytes: "));
            Serial.println(sent);
        }
    } else {
        // Standard-HTML ausliefern
        notFound();
    }
    
    // Request loggen
    File logfile = LittleFS.open(LOG_FILE, "a");
    if (logfile) {
        String requestlog = sammle_requestinfos();
        logfile.println(requestlog);
        logfile.close();
    } else if (DEBUG_MODE) {
        Serial.println(F("ERROR: Konnte Log-Datei nicht öffnen"));
    }
}

// ============================================================================
// HILFSFUNKTIONEN - NETZWERK
// ============================================================================

/**
 * @brief Aktiviert die Verbindung zum lokalen WLAN
 * 
 * Versucht eine Verbindung zum konfigurierten lokalen WLAN herzustellen.
 * Wird für Administration und OTA-Updates verwendet.
 * 
 * @return true wenn Verbindung erfolgreich, false sonst
 */
void aktiviere_localwlan() {
    #ifdef WLAN_LOCAL_POWER
    WiFi.setOutputPower(WLAN_LOCAL_POWER);
    #endif
    
    if (DEBUG_MODE) {
        Serial.print(F("INFO: Verbinde mit lokalem WLAN: "));
        Serial.println(LOCAL_SSID);
    }
    
    if (!WiFi.mode(WIFI_STA) ||
        !WiFi.begin(LOCAL_SSID, LOCAL_PASSWORD) ||
        (WiFi.waitForConnectResult(WIFI_CONNECT_TIMEOUT) != WL_CONNECTED)) {
        
        WiFi.mode(WIFI_OFF);
        isLocalWiFiConnected = false;
        
        if (DEBUG_MODE) {
            Serial.println(F("WARN: Lokale WLAN-Verbindung fehlgeschlagen"));
        }
    } else {
        isLocalWiFiConnected = true;
        
        Serial.println(F("INFO: Lokale WLAN-Verbindung erfolgreich"));
        Serial.print(F("INFO: IP-Adresse: "));
        Serial.println(WiFi.localIP());
    }
}

/**
 * @brief Aktiviert den Access Point Modus
 * 
 * Startet den ESP8266 als Access Point mit der zufällig ausgewählten SSID.
 * Wird nur aktiviert wenn keine lokale WLAN-Verbindung besteht.
 */
void aktiviere_apmodus() {
    if (!isLocalWiFiConnected) {
        WiFi.mode(WIFI_AP);
        
        #ifdef WLAN_AP_POWER
        WiFi.setOutputPower(WLAN_AP_POWER);
        #endif
        
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid_txt[rnd_ssid], NULL, rnd_chnl);
        
        #if ENABLE_CAPTIVE_PORTAL
        dnsServer.start(DNS_PORT, "*", apIP);
        #endif
        
        Serial.println(F("INFO: Access Point gestartet"));
        Serial.print(F("INFO: SSID: "));
        Serial.println(ssid_txt[rnd_ssid]);
        Serial.print(F("INFO: Kanal: "));
        Serial.println(rnd_chnl);
        Serial.print(F("INFO: IP-Adresse: "));
        Serial.println(WiFi.softAPIP());
    }
}

/**
 * @brief Initialisiert den NTP-Port
 * 
 * Startet den UDP-Port für NTP-Anfragen (nur im lokalen WLAN-Modus)
 */
void aktiviere_ntpport() {
    if (isLocalWiFiConnected) {
        Udp.begin(NTP_PORT);
        
        if (VERBOSE_LOGGING) {
            Serial.print(F("INFO: NTP-Port gestartet: "));
            Serial.println(NTP_PORT);
        }
    }
}

/**
 * @brief Sucht und initialisiert die RTC (Real Time Clock)
 * 
 * Versucht eine DS3231 RTC am I2C-Bus zu finden und zu konfigurieren.
 * Bei Power-Loss wird die RTC mit der Compile-Zeit initialisiert.
 */
void echtzeituhr_finden() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    Serial.println(F("INFO: Suche RTC..."));
    
    if (!dsrtc.begin()) {
        Serial.println(F("WARN: RTC nicht gefunden"));
        hasRTC = false;
    } else {
        Serial.println(F("INFO: RTC gefunden"));
        hasRTC = true;
        
        // Power-Loss-Status prüfen
        if (dsrtc.lostPower()) {
            Serial.println(F("WARN: RTC hatte Stromausfall, setze Zeit"));
            dsrtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            
            if (dsrtc.lostPower()) {
                Serial.println(F("ERROR: Konnte Power-Loss-Flag nicht löschen"));
            }
        } else {
            Serial.println(F("INFO: RTC-Zeit ist gültig"));
        }
        
        // RTC konfigurieren
        dsrtc.disable32K();
        dsrtc.clearAlarm(1);
        dsrtc.clearAlarm(2);
        dsrtc.writeSqwPinMode(DS3231_OFF);
        dsrtc.disableAlarm(1);
        dsrtc.disableAlarm(2);
    }
}

/**
 * @brief Konfiguriert OTA (Over-The-Air) Updates
 * 
 * Richtet die Callbacks für OTA-Updates ein. Ermöglicht Firmware-Updates
 * über WLAN ohne USB-Verbindung.
 */
void config_ota() {
    #if ENABLE_OTA
    
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
        Serial.println("INFO: OTA Update gestartet: " + type);
    });
    
    ArduinoOTA.onEnd([]() {
        Serial.println(F("\nINFO: OTA Update abgeschlossen"));
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        if (DEBUG_MODE) {
            Serial.printf("INFO: OTA Progress: %u%%\r", (progress / (total / 100)));
        }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("ERROR: OTA Fehler[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
        else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    });
    
    ArduinoOTA.begin();
    Serial.println(F("INFO: OTA aktiviert"));
    
    #endif
}

// ============================================================================
// SETUP
// ============================================================================

/**
 * @brief Initialisierung beim Start
 * 
 * Wird einmal beim Booten ausgeführt. Initialisiert alle Komponenten:
 * - Serielle Schnittstelle
 * - Filesystem
 * - RTC (falls vorhanden)
 * - Webserver mit allen Routen
 * - OTA-Updates
 */
void setup() {
    // WLAN beim Boot aktivieren
    enableWiFiAtBootTime();
    WiFi.persistent(false);
    delay(2);
    
    // Serielle Schnittstelle
    Serial.begin(SERIAL_BAUD);
    Serial.println();
    Serial.println(F("========================================"));
    Serial.print(F("INFO: "));
    Serial.print(F(FIRMWARE_NAME));
    Serial.print(F(" v"));
    Serial.println(F(FIRMWARE_VERSION));
    Serial.println(F("========================================"));
    
    // Zufallsgenerator initialisieren
    randomSeed(analogRead(0));
    
    // SSID-Botschaften aus PROGMEM kopieren
    for (int i = 0; i < MAX_BOTSCHAFT; i++) {
        strcpy_P(ssid_txt[i], SSID_MESSAGES[i]);
    }
    
    // Filesystem initialisieren
    Serial.println(F("INFO: Initialisiere Filesystem..."));
    if (!LittleFS.begin()) {
        Serial.println(F("ERROR: Filesystem-Initialisierung fehlgeschlagen"));
        return;
    }
    Serial.println(F("INFO: Filesystem bereit"));
    
    // RTC suchen und konfigurieren
    #if USE_RTC
    echtzeituhr_finden();
    #endif
    
    // Webserver-Routen einrichten
    Serial.println(F("INFO: Konfiguriere Webserver..."));
    
    // Favicon
    webserver.on(F("/favicon.ico"), []() {
        webserver.send(200, F("text/html"),
            F("<!DOCTYPE html>\r\n<html><head>\r\n"
              "<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAS0lEQVR42s2SMQ4AIAjE+P+ncSYdasgNXMJgcyIIlVKPIKdvioAXyWBeJmVpqRZKWtj9QWAKZyWll50b8IcL9JUeQF50n28ckyb0ADG8RLwp05YBAAAAAElFTkSuQmCC' type='image/x-png' />\r\n"
              "<title>Impfen Advertiser</title>\r\n</head></html>"));
    });
    
    // Statistik anzeigen
    webserver.on(F("/statistik"), []() {
        if (!webserver.authenticate(AUTH_USER, AUTH_PASSWORD)) {
            return webserver.requestAuthentication(DIGEST_AUTH, WWW_REALM, F("Authentifizierung fehlgeschlagen"));
        }
        
        // Aktuellen Request auch loggen
        File logfile = LittleFS.open(LOG_FILE, "a");
        if (logfile) {
            String requestlog = sammle_requestinfos();
            logfile.println(requestlog);
            logfile.close();
        }
        
        // Statistik-Datei ausliefern
        File pageFile = LittleFS.open(LOG_FILE, "r");
        if (pageFile) {
            String contentTyp = getContentType(LOG_FILE);
            size_t sent = webserver.streamFile(pageFile, contentTyp);
            pageFile.close();
        } else {
            webserver.send(404, F("text/plain"), F("Keine Statistik vorhanden"));
        }
    });
    
    // Statistik löschen
    webserver.on(F("/purge"), []() {
        if (!webserver.authenticate(AUTH_USER, AUTH_PASSWORD)) {
            return webserver.requestAuthentication(DIGEST_AUTH, WWW_REALM, F("Authentifizierung fehlgeschlagen"));
        }
        
        File logfile = LittleFS.open(LOG_FILE, "w");
        if (logfile) {
            String requestlog = sammle_requestinfos();
            logfile.println(requestlog);
            logfile.close();
            webserver.send(200, F("text/plain"), F("Statistik gelöscht"));
            Serial.println(F("INFO: Statistik wurde gelöscht"));
        } else {
            webserver.send(500, F("text/plain"), F("Fehler beim Löschen"));
        }
    });
    
    // Status-Seite
    webserver.on(F("/status"), []() {
        String status = F("<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Status</title></head><body>");
        status += F("<h1>Impfen-Advertiser Status</h1>");
        status += F("<p><b>Version:</b> ");
        status += F(FIRMWARE_VERSION);
        status += F("</p><p><b>Aktuelle SSID:</b> ");
        status += ssid_txt[rnd_ssid];
        status += F("</p><p><b>Kanal:</b> ");
        status += String(rnd_chnl);
        status += F("</p><p><b>Modus:</b> ");
        status += isLocalWiFiConnected ? F("Lokal verbunden") : F("Access Point");
        status += F("</p><p><b>RTC:</b> ");
        status += hasRTC ? F("Vorhanden") : F("Nicht vorhanden");
        status += F("</p><p><b>Uptime:</b> ");
        status += String(millis() / 1000);
        status += F(" Sekunden</p></body></html>");
        webserver.send(200, F("text/html"), status);
    });
    
    // Alle anderen Anfragen
    webserver.onNotFound(handleUnknown);
    
    // Header sammeln
    const char* headers[] = {"User-Agent"};
    webserver.collectHeaders(headers, 1);
    
    // Webserver starten
    webserver.begin();
    Serial.println(F("INFO: Webserver gestartet"));
    
    // OTA konfigurieren
    config_ota();
    
    Serial.println(F("INFO: Setup abgeschlossen"));
    Serial.println(F("========================================"));
}

// ============================================================================
// MAIN LOOP
// ============================================================================

/**
 * @brief Hauptschleife
 * 
 * Wird kontinuierlich ausgeführt. Verwaltet:
 * - SSID-Rotation nach Ablauf der Zeit
 * - WLAN-Kanal-Wechsel
 * - OTA-Updates
 * - DNS-Server (Captive Portal)
 * - Webserver-Anfragen
 */
void loop() {
    // Prüfen ob SSID gewechselt werden soll
    if ((startmillis == 0) || (abs((long)(millis() - startmillis)) > looptime * 1000)) {
        
        // Neue zufällige Werte generieren
        rnd_ssid = random(1, MAX_BOTSCHAFT - 1);
        rnd_chnl = random(MIN_WIFI_CHANNEL, MAX_WIFI_CHANNEL + 1);
        looptime = random(MIN_LOOP_TIME_SEC, MAX_LOOP_TIME_SEC);
        
        Serial.println(F("\n========================================"));
        Serial.println(F("INFO: SSID-Wechsel"));
        Serial.print(F("INFO: Neue SSID: "));
        Serial.println(ssid_txt[rnd_ssid]);
        Serial.print(F("INFO: Neuer Kanal: "));
        Serial.println(rnd_chnl);
        Serial.print(F("INFO: Dauer: "));
        Serial.print(looptime);
        Serial.println(F(" Sekunden"));
        Serial.println(F("========================================\n"));
        
        // WLAN neu konfigurieren
        WiFi.mode(WIFI_OFF);
        delay(WIFI_MODE_SWITCH_DELAY);
        
        // Lokales WLAN versuchen
        aktiviere_localwlan();
        
        // NTP initialisieren
        aktiviere_ntpport();
        
        // AP-Modus aktivieren (falls nicht lokal verbunden)
        aktiviere_apmodus();
        
        // Timer zurücksetzen
        startmillis = millis();
        
        // OTA-Passwort setzen wenn im AP-Modus
        if (!isLocalWiFiConnected) {
            ArduinoOTA.setPassword(OTA_PASSWORD);
        }
    }
    
    // OTA-Updates verarbeiten
    #if ENABLE_OTA
    ArduinoOTA.handle();
    #endif
    
    // DNS-Server verarbeiten (nur im AP-Modus)
    #if ENABLE_CAPTIVE_PORTAL
    if (!isLocalWiFiConnected) {
        dnsServer.processNextRequest();
    }
    #endif
    
    // Webserver-Anfragen verarbeiten
    webserver.handleClient();
}

// Made with Bob
