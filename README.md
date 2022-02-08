Impfkampagne Advertiser
Verhaelt sich wie ein Hotspot, kann aber Content zum Thema Impfen (oder jeden beliebigen anderen statischen Web Content ausliefern)
Per Zufallsgenerator gesteuert wird eine aus einer Liste von (derzeit 16) Botschaften zum Thema Corona und Impfen eine SSID erzeugt und
fuer 90 Sekunden bis 4 Minuten aufrecht erhalten. Danach wird die SSID und der WLAN Kanal gewechselt

Funktioniert mit ESP8266 und optional einer DS3231 RTC, benötigt fuer einen sinnvollen Betrieb ESP8266 Konfiguration mit mindestens 2MB Filesystem

Optionen des We3bservers:

<IP-Adresse>/statistik   :
Statistik ueber die Clients die eine Verbindung aufgebaut haben, wenn eine DS3231 Uhr verbaut ist, mit Zeitstempel. An Informationen ueber die Clients
steht nur der User-Agent, die angeforderte Webseite und evtl. die lokale IP-Adresse des Clients zur Verfuerung. Andere Informationen insbesondere 
personenbezogene Informationen stehen gar nicht zur verfuegung bzw. werden nicht erfasst. 
Auszug aus einem Logfile:
------------------
05.02.2022 14:58:30 --- Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36 --- /generate_204_443b2f4a-a59d-437b-8e33-2930cc1511d5
05.02.2022 14:58:35 --- Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36 --- /generate_204
05.02.2022 14:58:39 --- Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36 --- /generate_204_f51be4b9-1f88-4579-a4dd-039b81bb9384
05.02.2022 14:58:40 --- Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36 --- /generate_204
05.02.2022 14:59:36 --- Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36 --- /generate_204
05.02.2022 15:15:16 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
05.02.2022 15:15:50 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
05.02.2022 15:16:11 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
05.02.2022 15:17:38 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
05.02.2022 15:54:14 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 16:07:01 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 16:07:58 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 16:11:03 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 16:38:28 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 16:38:48 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 17:49:57 --- Dalvik/2.1.0 (Linux; U; Android 8.0.0; RNE-L21 Build/HUAWEIRNE-L21) --- /
05.02.2022 17:53:47 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
05.02.2022 17:53:51 --- Mozilla/5.0 (iPhone; CPU iPhone OS 15_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 --- /hotspot-detect.html
05.02.2022 17:53:51 --- CaptiveNetworkSupport-418 wispr --- /hotspot-detect.html
---------------

<IP-Adresse>/purge  :
Das Statistikfile wird komplett geloescht

<IP-Adresse>/BELIEBIGEANDEREURL  :
Es wird die im Sketch definierte Standars HTML Seite ausgeliefert, die evtl. auf eine weitere HTML Seite vewrlinkt, die dann aber im lokalen Filesystem 
des ESP abgelegt sein muss


