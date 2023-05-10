# Smarthome Device
Die WiFi Kommunikationsklasse für den ESP8266 und ESP8285 ist der zweite Kernbestandteil der Arbeit. Diese befindet sich im Unterverzeichnis ```lib/```. Im Unterverzeichnis ```examples/``` sind Beispiele zu finden, welche die entwickelte Klasse verwenden.  
  
Die Lösung baut auf die Arduino Plattform auf. Die nachfolgenden Erläuterungen beziehen sich auf die Verwendung zusammen mit der [Arduino IDE](https://www.arduino.cc/en/software). Alternativ sollte aber auch eine Verwendung mit [PlatformIO](https://platformio.org/platformio-ide) (für Visual Studio Code) möglich sein.  
  
Zu Verwendung muss die IDE zunächst für die Verwendung des ESP Boards eingerichtet sein. Dazu ist es notwendig, das Arduino ESP8266 Addon zum Boardmanager hinzuzufügen. Weitere Details sind im [Github Repository](https://github.com/esp8266/Arduino) des Projekts zu finden. Danach lässt sich die IDE für das Flashen des ESP Chips mit verschiedenen vorkonfigurierten Profilen einrichten und es stehen einige ESP8266 spezifische Bibliotheken zur Verfügung.  
Eine ausführliche Anleitung mitsamt Einrichtung der IDE findet sich unter:  [https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
## WiFi Kommunikationsklasse
Zur Verwendung der ```WiFiStationCommunicator``` Klasse müssen neben den vorinstallierten Arduino Standard Bibliotheken noch folgende Libraries installiert werden (verfügbar im Bibliotheksverwalter):  
- [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient) (0.4.0) 

Die ESP8266/8285 spezifischen Bibliotheken stehen bereits durch das vorherige Hinzufügen des ESP8266 Addons zur Verfügung:
- [ESP8266WiFi](https://github.com/esp8266/Arduino)
- [ESP8266WebServer](https://github.com/esp8266/Arduino)

### Verwendung
Um die Kommunikationsklasse im eigenen Arduino Projekt verwenden zu können, müssen folgende Dateien im Hauptverzeichnis des Arduino Sketches liegen:  
- ```WiFiStationCommunicator.h```
- ```WiFiStationCommunicator.cpp```
- ```wifi_com_config.h```

Diese befinden sich im ```lib/``` Verzeichnis. Die ```WiFiStationCommunicator.h``` und ```WiFiStationCommunicator.cpp``` Dateien können entweder in das Sketch Verzeichnis kopiert- oder mittels eines Softlinks zum Verzeichnis hinzugefügt werden (ausgehend vom ```examples``` Verzeichnis):
```
$ ln -s ../../lib/WiFiStationCommunicator/WiFiStationCommunicator.h WiFiStationCommunicator.h
$ ln -s ../../lib/WiFiStationCommunicator/WiFiStationCommunicator.cpp WiFiStationCommunicator.cpp
```
Das Konfigurationstemplate sollte hingegen in ```wifi_com_config.h``` umbenannt und in das Sketch Verzeichnis kopiert werden.

### Konfiguration
```
// # General
#define DEVICE_ID ""
#define CENTRAL_SERVER_ADDR ""
#define CENTRAL_SERVER_PORT 6000
// # WIFI
#define WIFI_SSID ""
#define WIFI_PASS ""
// ACCESS POINT
#define ACCESS_POINT_SSID ""
#define ACCESS_POINT_PASS ""
// # Additional
#define VERBOSE true
// Runtime of Webserver and Access Point after no WiFi connection could be established in seconds
// Set to zero for no time limit
// Max: 65'535 seconds
#define WEBSERVER_RUNTIME 300
```
In der Konfigurationsdatei muss eine eindeutige Device ID eingetragen werden. Diese sollte nur aus Leerzeichen, Buchstaben (A-Z,a-z), Unter- und Bindestrichen und Zahlen (0-9) bestehen.  
Darüber hinaus muss die IP und der PORT eingetragen werden, über welche das WiFi Kommunikationsmodul der zentrale Anwendung erreichbar ist.  
  
Damit sich das Gerät mit dem WiFi verbinden kann, ist außerdem das Hinzufügen der WiFi Zugangsdaten notwendig.  
  
Es lässt sich außerdem ein WiFi Access Point einrichten. Dieser wird für eine bestimmte Zeit geöffnet wenn keine WiFi Verbindung hergestellt werden konnte. Nach dem Verbinden lassen sich dann in einen Webbrowser über die Adresse ```http://192.168.4.1/``` Anpassungen z.B. an den hinterlegten WiFi Zugangsdaten, vornehmen.

### Verwendung
Nachdem alle vorherigen Schritte durchgeführt wurden, lässt sich die WiFi Kommunikationsklasse über ein einfaches include in das eigene Projekt einbinden:  
```
#include "WiFiStationCommunicator.h"
```

### Dokumentation
Eine Dokumentation zur Verwendung der Klasse steht in einer separaten [DOCUMENTATION.md](lib/WiFiStationCommunicator/DOCUMENTATION.md) zur Verfügung.

## Beispiele
Zur Veranschaulichung, wie die Lösung verwendet werden kann, wurden Beispiele erarbeitet, welche sich im Unterverzeichnis ```examples/``` befinden. Der Aufbau der Schaltung und die verwendende Hardware wurden in der Ausarbeitung ausführlich beschrieben. Zur Verwendung des Barometer Beispiels ist darüber hinaus noch folgende Bibliothek notwendig:  
- [Adafruit BMP085 Library](https://github.com/adafruit/Adafruit-BMP085-Library) (1.2.1) (Verfügbar über Bibliotheksverwalter)
