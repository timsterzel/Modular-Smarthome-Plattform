# Zentrale Instanz
Eine der Kernbestandteil der Arbeit ist die Zentrale Anwendung. Diese befindet sich im Unterverzeichnis ```CentralApplication/```. Im Unterverzeichnis ```CustomScripts/``` befinden sich Beispiele für Endanwender Skripte. Die folgenden Anweisungen beziehen sich auf den Einsatz auf Linux Systemen. Eine Ausführung auf anderen Systemen mit Python Interpreter sollte aber theoretisch möglich sein.

## Zentrale Anwendung
Einer der zwei Kernbestandteile der Arbeit. Befindet sich im Unterverzeichnis ```CentralApplication/```.
### Vorrausetzungen
Zum Ausführen der zentralen Anwendung wird Python3 in mindestens der Version 3.7 benötigt. Getestet wurde die Anwendung unter folgenden Systemen:  
- Python 3.7.3 unter Raspbian GNU/Linux 10  
- python 3.8.10 unter Ubuntu 20.04.4 LTS  
- Python 3.9.7 unter Ubuntu 21.10  
  
Es werden keine zusätzlichen Bibliotheken benötigt.  
  
Die Anwendung legt eine SQLite Datenbank im Homeverzeichnis des Systems unter ```~/.CentralApplication``` an, in welcher eingehende Daten der Smarthome Geräte gespeichert werden. Es sind deshalb Schreibrechte auf das Homeverzeichnis erforderlich.  

### Konfiguration
Um das Programm auszuführen, ist es notwendig, das Template für die Konfigurationsdateien vorher umzubenennen. Das Template der Hauptkonfiguration findet sich unter ```/config/config.py.example``` und muss zu ```/config/config.py``` umbenannt werden:  
```
$ cp config/config.py.example config/config.py
```
Innerhalb der Datei können diverse Anpassunugen gemacht werden. Das Template eignet sich bereits zum Testen der Anwendung. Die REST API wird standardmäßig unter localhost und dem Port 6363 gestartet, damit diese Anwendungen auf dem System zur Verfügung steht.  
  
Das im Rahmen der Arbeit entwickelte WiFi Kommunikationsmodul wird bereits eingebunden:  
```
COMMUNICATION_MODULES_TO_LOAD = [ "esp_wifi_communicator" ]
```
Die Anwendung bindet das Modul dann automatisch ein. Das Einbinden mehrerer Module ist möglich, indem man Komma separiert einen weiteren Eintrag hinzufügt:
```
COMMUNICATION_MODULES_TO_LOAD = [ "esp_wifi_communicator", "future_communicator" ]
```
Zum jetzigen Zeitpunkt steht nur das *esp_wifi_communicator* Modul zur Verfügung.  
  
Da das Kommunikationsmodul eingebunden wurde, muss dieses auch konfiguriert werden. Dazu muss zunächst wieder das Template umbenannt werden:  
```
$ cp modules/communication/esp_wifi_communicator/config/config.py.example modules/communication/esp_wifi_communicator/config/config.py
```
Danach ist es wichtig, beim Eintrag *HOSTNAME* die IP der zentralen Instanz im Netzwerk anzugeben. Nur so kann intern der Webserver gestartet werden und Nachrichten von Smarthome Geräten empfangen werden. Wird hier keine oder eine falsche IP-Adresse angegeben, bricht das Programm mit einer Exception ab. Zum Testen eignet sich auch der Eintrag ```localhost```. Dies würde im produktiv Einsatz aber keinen Sinn machen. Falls notwendig, kann auch der Port angepasst werden.

### Ausführung
Die Anwendung wird nach der Konfiguration mit folgendem Befehl gestartet:  
```
$ python3 central_app.py 
```

### Dokumentation
Eine Dokumentation der API und zum Einbinden und entwickeln eigener Kommunikationsmodule findet sich in einer separaten [DOCUMENTATION.md](CentralApplication/DOCUMENTATION.md).

## Custom Skripte
Custom Skripte sind kein Kernbestandteil der Arbeit. Sie wurden entwickelt, um die Verwendung der API und die Funktionsweise der Events zu veranschaulichen. Auffindbar im Unterverzeichnis ```CustomScripts/```.   
  
Bei den Custom Skripten handelt es sich um Bash Skripte, welche verschiedene Systemwerkzeuge und Anwendungen verwenden. Zur Ausführung muss die ```mail``` Funktion des Systems eingerichtet sein (z.B. mittels ```msmtp```) und die Kommandozeilen Anwendung ```jq``` zum parsen von JSON Strings muss installiert sein. Außerdem muss die jeweilige ```config.sh``` angepasst werden.