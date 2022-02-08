//Impfkampagne Advertiser
// Verhaelt sich wie ein Hotspot, kann aber Content zum Thema Impfen (oder jeden beliebigen anderen statischen Web Content ausliefern)
// Per Zufallsgenerator gesteuert wird eine aus einer Liste von (derzeit 16) Botschaften zum Thema Corona und Impfen eine SSID erzeugt und
// fuer 90 Sekunden bis 4 Minuten aufrecht erhalten. Danach wird die SSID und der WLAN Kanal gewechselt

static const char authUser[] PROGMEM = "SETZEMICHUSERNAME";                 // Userid fuer Abruf der Statistikdaten
static const char authPassword[] PROGMEM = "SETZEMICHPASSWORT";   //Passwort fuer Abruf der Statistikdaten
static const char otaPassword[] PROGMEM = "PASSWORTOTA"; // Passwort fuer OTA geschuetzten Update
static const char localssid[]  = "LOKALESSID";   //SSID lokales, eigenes WLAN  - erleichtert dministration und ggf. OTA Update
static const char localpassword[]  = "PASSWORTLOKALESSID"; // Passwort //lokales, eigenes WLAN, entweder arbeitet das Modul als Client im eigenen WLAN
                                                        // oder als Hotspot mit einer zufällig ausgewählten SSID wie oben beschrieben
                                                        // Wenn das eigenen WLAN nicht gefunden werden kann, wird der AP Modus aktiviert 
                                                      
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "RTClib.h"   //DS3231 RTC
#include <TimeLib.h>

#define WLAN_LOCALPOWER 10.5    // lokaler Test
#define WLAN_APPOWER 20.5    // lokaler Test
#define MAX_BOTSCHAFT 17    // Anzahl der Botschaften im Array ssid_txt
#define MIN_LOOPTIME 90     // so lange bleibt die SSID mindestens aktiv
#define MAX_LOOPTIME 240    // so lange bliebt die SSID maximal aktiv
#define NTP_TIMESERVER "de.pool.ntp.org" // NTP Server, lokal oder de.pool.ntp.org
#define I2C_SDA  D6       // I2C Pins fuer DS3231 RTC
#define I2C_SCL  D7       // dito

IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webserver(80);
RTC_DS3231 dsrtc;
IPAddress timeServer;
  
static const char www_realm[] PROGMEM = "Impfen jetzt";
static const char authFailResponse[] PROGMEM = "Fehler";


String responseHTML = (""
                      "<!DOCTYPE html><html lang='de'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>Impfen toetet NICHT</title></head><body>"
                      "<h1><p style=\"text-align: center\">Hallo Impfgegner!</p></h1><p style=\"text-align: center\">"
                      "Glaub nicht jeden Scheiss in Telegram, sondern lass Dich impfen!</p><p style=\"text-align: center\">"
                      "<a href=\"http:./Impfung.html\">Beitrag aus Wikipedia zum Thema Impfung</a></p></body></html>");

char ssid_txt[MAX_BOTSCHAFT][32] =                       
                      {{"Test"},
                      {"Nicht Impfen tötet!"},
                      {"Impfen. What else?"},
                      {"Impfgegner sind nicht schlau"},
                      {"Lass-Dich-Impfen"},
                      {"Impfen bringt Freiheit"},
                      {"Impfen schützt auch die Kultur"},
                      {"Nazis unterwandern Impfgegner!"},
                      {"Impfen lohnt sich"},
                      {"Dann geh doch zum Impfen!"},
                      {"see. think. impf."},
                      {"#ZusammenGegenCorona"},
                      {"Impfen ist wichtig!"},
                      {"Wir lieben Impfen"},
                      {"Impfen hilft"},
                      {"Come impf and find out"},
                      {"Nicht Impfen macht Pharma reich"}};

const char* Headers[] = {"User-Agent"};
const byte DNS_PORT = 53;

unsigned long looptime=0;
unsigned long startmillis=0;
uint32_t rnd_ssid;
uint32_t rnd_chnl;
boolean has_rtc=false;
boolean wifilocal = false;

/*-------- NTP code ----------*/
const int timeZone = 1;     // Central European Time
WiFiUDP Udp;
uint32_t ntpport = 8888;  // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

boolean summertime_EU(const int& t_year, const int& t_month, const int& t_day, const int& t_hour, const int& tzHours)
{
  if ((t_month<3)||(t_month>10)) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if ((t_month>3)&&(t_month<10)) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (((t_month==3)&&((t_hour+24*t_day)>=(1+tzHours+24*(31-(5*t_year/4+4)%7))))||((t_month==10)&&((t_hour+24*t_day)<(1+tzHours+24*(31-(5*t_year/4+1)%7)))))
     {
        return true;
     } else {
       return false;
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

timeval getNtpTime() {
  DateTime dsnow;
  const char* NTPserver = NTP_TIMESERVER;
  timeval  tv_temp;
  tmElements_t tmp_time;
  uint32_t usec=0;
  unsigned long secsSince1900=0;
  
  if (wifilocal==true) {  // nur wenn das wlan da ist
    if (!WiFi.hostByName(NTPserver, timeServer)) {
      timeServer = WiFi.gatewayIP(); 
    }
   while (Udp.parsePacket() > 0) ; // discard any previously received packets   
   sendNTPpacket(timeServer);  
   uint32_t beginWait = millis();
   while ((millis() - beginWait < 350)) {
      int sizeudp = Udp.parsePacket();    
      yield();
      if (sizeudp >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer       
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];      
      uint32_t frac  = (uint32_t) packetBuffer[44] << 24
               | (uint32_t) packetBuffer[45] << 16
               | (uint32_t) packetBuffer[46] <<  8
               | (uint32_t) packetBuffer[47] <<  0;
           usec = ((uint64_t) frac * 1000 *1000) >> 32;
      /*
      if ((usec/1000)<1000) {  // zur naechsten Sekunde warten
        //delay(uint32_t(1001-usec/1000)); //verzoegerung 
        secsSince1900+=1; //+1 weil zur naechsten sekunde gewartet wurde
      } //verzoegerung auf nae sekunde ende
      */     
      }
   }
     if (secsSince1900<10) {// NTP Paket nicht i.O  // standard
      if (has_rtc==true){ //NTP nicht i.O. aber RTC vorhanden
        //NTP Fehler, Uhrzeit aus RTC lesen
        dsnow = dsrtc.now();        
        tmp_time.Year=dsnow.year();
        tmp_time.Month=dsnow.month();
        tmp_time.Day=dsnow.day();
        tmp_time.Hour=dsnow.hour();
        tmp_time.Minute=dsnow.minute();
        tmp_time.Second=dsnow.second();                               
        //String tmp_cmd = F("NTP NOK,Zeit in RTC:");
        //char tmp_timestring[32];             
        //sprintf_P (tmp_timestring,PSTR("%02d.%02d.%04d %02d:%02d:%02d (%1d)"),dsnow.day(),dsnow.month(),dsnow.year(),dsnow.hour(),dsnow.minute(),dsnow.second(),dsnow.dayOfTheWeek());
        //tmp_cmd +=String(tmp_timestring);
        //Serial.println(tmp_cmd);
        tmp_time.Year=tmp_time.Year-1970;  // Korrektur fuer MakeTime
        secsSince1900= makeTime(tmp_time)+2208988800UL;  // convert time elements into time_t
        usec=0.5*(1000)*(1000);  // immer auf halber sekunden annehmen       
       } else { // NTP Paket nicht i.O. und kein RTC
        String tmp_cmd = F("NTP NOK:keine Zeit?,kein RTC");
        Serial.println(tmp_cmd);
      }// ende NTP Paket nicht i.O. und kein RTC
     } else { // NTP Paket i.O.
      if (has_rtc==true){  //NTP zeit OK, rtc vorhanden
        dsnow = dsrtc.now();
        (usec>(0.7*1000*1000)?breakTime((secsSince1900+1)-2208988800UL,tmp_time):breakTime((secsSince1900)-2208988800UL,tmp_time));  
        tmp_time.Year=tmp_time.Year-30; // Korrektur fuer RTC chip
        //String tmp_cmd = F("NTP OK,Zeit an RTC:");
        //char tmp_timestring[32];  
        //sprintf_P (tmp_timestring,PSTR("%02d.%02d.%04d %02d:%02d:%02d"),tmp_time.Day,tmp_time.Month,tmp_time.Year,tmp_time.Hour,tmp_time.Minute,tmp_time.Second);
        //tmp_cmd +=String(tmp_timestring);
        //Serial.println(tmp_cmd);    
        dsrtc.adjust(DateTime(tmp_time.Year,tmp_time.Month,tmp_time.Day,tmp_time.Hour,tmp_time.Minute,tmp_time.Second));
       } else {  // NTP Zeit OK, kein RTC
      }  // NTP Zeit OK, kein RTC
    } // NTP lokal jetzt vorhanden
   } else {  //wlan nicht da, ggf. RTC abfragen
       if (has_rtc==true){ //RTC vorhanden
            //NTP Fehler, Uhrzeit aus RTC lesen
           dsnow = dsrtc.now();       
           tmp_time.Year=dsnow.year();
           tmp_time.Month=dsnow.month();
           tmp_time.Day=dsnow.day();
           tmp_time.Hour=dsnow.hour();
           tmp_time.Minute=dsnow.minute();
           tmp_time.Second=dsnow.second();
           //String tmp_cmd = F("No NTP,Zeit in RTC:");
           //char tmp_timestring[32];             
           //sprintf_P (tmp_timestring,PSTR("%02d.%02d.%04d %02d:%02d:%02d (%1d)"),dsnow.day(), dsnow.month(), dsnow.year(), dsnow.hour(), dsnow.minute(), dsnow.second(),dsnow.dayOfTheWeek());
           //tmp_cmd +=String(tmp_timestring);
           //Serial.println(tmp_cmd);
           tmp_time.Year=tmp_time.Year-1970;  // Korrektur fuer MakeTime    
           secsSince1900= makeTime(tmp_time)+2208988800UL;  // convert time elements into time_t
           usec=0.5*(1000)*(1000);
       }
  }  //NTP gesperrt w/semaphore aber RTC vorhanden
 
  if (secsSince1900>10) {// NTP Paket i.O
      tv_temp.tv_sec=uint32_t(secsSince1900-2208988800UL);
      tv_temp.tv_usec=usec;
      return(tv_temp);
  }
 Serial.println("No NTP,no RTC!");
 tv_temp.tv_sec=0;
 tv_temp.tv_usec=0;
 return(tv_temp);  // semaphore blockiert, kein RTC fallback oder Fehler
}

void uhrzeitstempel (const byte& tformat, char* stempel, const time_t& zeitstempel_sec) {
    if ((tformat==0)||(tformat==2)) 
           sprintf_P (stempel,PSTR("%02d.%02d.%04d %02d:%02d:%02d"),day(zeitstempel_sec), month(zeitstempel_sec), year(zeitstempel_sec), hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
    if (tformat==1) {
       if (summertime_EU(year(zeitstempel_sec),month(zeitstempel_sec),day(zeitstempel_sec),hour(zeitstempel_sec),timeZone)) {
          sprintf_P (stempel,PSTR("%04d-%02d-%02dT%02d:%02d:%02d+02:00"), year(zeitstempel_sec), month(zeitstempel_sec), day(zeitstempel_sec), hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
         } else {
          sprintf_P (stempel,PSTR("%04d-%02d-%02dT%02d:%02d:%02d+01:00"), year(zeitstempel_sec), month(zeitstempel_sec), day(zeitstempel_sec), hour(zeitstempel_sec), minute(zeitstempel_sec), second(zeitstempel_sec));
       }
    }
    return;
}
// NTP Code Ende


// Sendet "Not Found"-Seite
void notFound()
{ 
  webserver.send(200, "text/html", responseHTML);
}


String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


void config_ota() {   // Damit ist ein OTA Update des Sketch oder Filesystem via WLAN  moeglich
  ArduinoOTA.onStart([]() {
    });
  ArduinoOTA.onEnd([]() {
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  });
  ArduinoOTA.onError([](ota_error_t error) {
  });
  ArduinoOTA.begin();
}


String sammle_requestinfos() {
  String requestdate, requestagent, requestip; 
  time_t tv=getNtpTime().tv_sec;   
  if (tv>10) {
     char tmp_timestring[32];
     (summertime_EU(year(tv),month(tv),day(tv),hour(tv),timeZone)==true?uhrzeitstempel(2,tmp_timestring,tv+7200):uhrzeitstempel(2,tmp_timestring,tv+3600));
     requestdate = String(tmp_timestring);
    } else {
      requestdate=ssid_txt[rnd_ssid];
  }
  requestip=webserver.client().remoteIP().toString();
  if (webserver.hasHeader("User-Agent")) {
     requestagent=webserver.header("User-Agent"); 
   } else {
     requestagent="UNKNOWN";
  }
  String requestresult= requestdate+" ## "+requestip+" ## "+requestagent+" ## "+webserver.uri();
  return(requestresult);
}

void handleUnknown() { 
  String filename = webserver.uri();   
  Serial.print("File to serve: ");
  Serial.println(filename);
  File pageFile = LittleFS.open(filename, "r");
  // Es wird versucht, die angegebene Datei aus dem LittleFS hochzuladen
  if ((pageFile)&&(filename.indexOf("requests.txt")==-1)&&(filename!="/")) { // Statistik gg. unerwuenschten Zugriff sichern
      String contentTyp = getContentType(filename);
      size_t sent = webserver.streamFile(pageFile, contentTyp);
      pageFile.close();    
    } else {
      //Standard HTML
      notFound(); 
  } 
  // Daten zur Anfrage (Datum wenn vorhanden, ansonsten genutzte SSID, lokale IP Adresse sofern vorhanden, User-Agent und URI) sichern
  File logfile = LittleFS.open("/requests.txt","a");
  String requestlog=sammle_requestinfos();
  logfile.println(requestlog);
  logfile.close();
}


void aktiviere_localwlan() {
 #ifdef WLAN_LOCALPOWER
   float wlpower=WLAN_LOCALPOWER;
   WiFi.setOutputPower(wlpower);
 #endif
 if (!WiFi.mode(WIFI_STA)
        || !WiFi.begin(localssid, localpassword)
        || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
      WiFi.mode(WIFI_OFF);
      Serial.println("Cannot connect local!");
      wifilocal=false;
    } else
      wifilocal=true;
      Serial.println("connect local succesful!");  
      Serial.print("IP-Adresse: ");
      Serial.println(WiFi.localIP());
}


void aktiviere_apmodus() {
 if (wifilocal==false) {
  WiFi.mode(WIFI_AP);
  #ifdef WLAN_APPOWER
   float wlpower=WLAN_APPOWER;
   WiFi.setOutputPower(wlpower);
  #endif
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid_txt[rnd_ssid],NULL,rnd_chnl);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.print("AP Name: ");
  Serial.println(ssid_txt[rnd_ssid]);
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.softAPIP()); 
 } 
}

void aktiviere_ntpport() {
 if (wifilocal==true) {
  Udp.begin(ntpport);
 }
}

void echtzeituhr_finden() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("Init RTC");
  if (!dsrtc.begin()) {
    Serial.println("Couldn't find RTC");
    has_rtc=false;
   } else {
    Serial.println("RTC found");
    has_rtc=true;
  }
  if (has_rtc) { 
   Serial.print("RTC Lost power status:");
   if (dsrtc.lostPower()) { //Power Fail, zeit setzen
     Serial.println("POWER FAILED");
     dsrtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     if (dsrtc.lostPower()) {
        Serial.println(" ERROR clearing POWER FAILED.");
     }
    } else {
       Serial.println("RTC POWER OK");
   }
   dsrtc.disable32K();
   dsrtc.clearAlarm(1);
   dsrtc.clearAlarm(2);
   dsrtc.writeSqwPinMode(DS3231_OFF);
   dsrtc.disableAlarm(2);
   dsrtc.disableAlarm(1);   
  }
}

void setup() {  //setup
  enableWiFiAtBootTime(); 
  WiFi.persistent(false);
  delay(2);
  Serial.begin(74880);
  randomSeed(analogRead(0));

  // Initialize LittleFS
  if(!LittleFS.begin()){
    return;
  }

  // Wenn vorhanden, DS3231 RTC finden und konfigurieren
  echtzeituhr_finden();
  
  // Webserver einrichten
  webserver.on(F("/favicon.ico"), [] () {
     webserver.send(200,F("text/html"),F("<!DOCTYPE html>\r\n<html><head>\r\n<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAS0lEQVR42s2SMQ4AIAjE+P+ncSYdasgNXMJgcyIIlVKPIKdvioAXyWBeJmVpqRZKWtj9QWAKZyWll50b8IcL9JUeQF50n28ckyb0ADG8RLwp05YBAAAAAElFTkSuQmCC' type='image/x-png' />\r\n<title>Impfen Advertiser</title>\r\n</head></html>"));
  });
  webserver.on(F("/statistik"), []() {  //Requestfile==Statistik anzeigen
    if (!webserver.authenticate(authUser,authPassword)) {
      return webserver.requestAuthentication(DIGEST_AUTH,www_realm,authFailResponse);
    }
    File logfile = LittleFS.open("/requests.txt","a");
    String requestlog=sammle_requestinfos();
    logfile.println(requestlog);
    logfile.close();
    File pageFile = LittleFS.open("/requests.txt", "r");
    String contentTyp = getContentType(filename);
    size_t sent = webserver.streamFile(pageFile, contentTyp);
    pageFile.close();

  });
  webserver.on(F("/purge"), []() {     // Requestfile==Statistik loeschen
    if (!webserver.authenticate(authUser,authPassword)) {
      return webserver.requestAuthentication(DIGEST_AUTH,www_realm,authFailResponse);
    }
    File logfile = LittleFS.open("/requests.txt", "w");
    String requestlog=sammle_requestinfos();
    logfile.println(requestlog);
    logfile.close();
    webserver.send(200, "text/plain", "Logfile purged");
  });
  webserver.onNotFound(handleUnknown); //entweder lokal vorhandene Datei oder Standarddatei ausliefern
  
  webserver.collectHeaders(Headers, sizeof(Headers)/sizeof(Headers[0]));
  webserver.begin();
  config_ota();
}


void loop() {
 if((startmillis==0)||((abs((long)(millis()-startmillis))>looptime*1000))) {  // Nach Ablauf der Zeit andere Botschaft schalten
  rnd_ssid = random(1,MAX_BOTSCHAFT-1); // per Zufallsgenerator festlegen welche Botschaft als SSID aktiv wird
  rnd_chnl = random(1,13); // per Zufallsgenerator festlegen auf welchem Kanal diese SSID aktiv wird
  looptime = random(MIN_LOOPTIME,MAX_LOOPTIME); // per Zufallsgenerator festlegen wie lange diese SSID aktiv bleibt
  
  WiFi.mode(WIFI_OFF);
  delay(200);   // zur Sicherheit ein bschen warten
  //lokales WLAN versuchen zu aktivieren
  aktiviere_localwlan();

  // NTP initialisieren
  aktiviere_ntpport();

  //AP modus aktivieren, sofern nicht schon lokal verbunden
  aktiviere_apmodus();

  startmillis=millis();
  if (wifilocal==false)   // wenn OTA im AP Modus aktiv ist, zur Sicherheit das Passwort setzen um Manipulationen zu verhindern
   ArduinoOTA.setPassword(otaPassword);
 }
 ArduinoOTA.handle(); 
 if (wifilocal==false)     // Nur wenn nicht lokal verbunden ist das Capture Portal aktiv  
   dnsServer.processNextRequest();
 webserver.handleClient();
}
