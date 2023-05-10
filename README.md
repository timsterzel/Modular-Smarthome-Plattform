# Modular-Smarthome-Plattform
This project is the practical part of my bachelor thesis "Development of an open and modular platform for smart home devices". 
## Development of an open and modular platform for smart home devices
  
The work consists of two main components. The smarthome devices, which act as clients, and a central instance that receives and processes data.
This runs a central application that can be accessed by the devices. The communication between the central application and the smarthome devices can take place via different communication protocols and can be extended with modules.
  
The central application is optimized for use on Linux systems (especially the Raspberry Pi), but should theoretically run on any device where Python3 and the libraries used are available.
  
In this work, a module for communication via WiFi has already been developed. Since the central application is modular, it is possible to develop and integrate further communication modules. The communication is one-way from the smarthome device (client) to the central application (server). This design decision allows the code on the Smarthome device to be kept very lean and to be executed in a resource-saving way on the hardware. This leaves the end user with many resources while maintaining a high level of abstraction to develop their own applications on the microcontroller.

The project consists of two different application parts, each with its own README file which contains instructions for compiling and starting the respective application:
  - Central Device: [project/CentralDevice/README.md](project/CentralDevice/README.md)
  - Smarthome Device: [project/SmarthomeDevice/ESP8266_8285/README.md](project/SmarthomeDevice/ESP8266_8285/README.md)
  
## Entwicklung einer offenen und modularen Plattform für Smarthome Geräte 
Die Arbeit besteht aus zwei Hauptkomponenten. Den Smarthome Geräten, welche als Clients fungieren und einer zentralen Instanz, die Daten empfängt und verarbeitet. Auf dieser läuft eine für die Geräte erreichbare zentrale Anwendung. Die Kommunikation zwischen zentraler Anwendung und Smarthome Geräten kann über verschiedene Kommunikationsprotokolle geschehen und mit Modulen erweitert werden.
  
Die zentrale Anwendung ist für die Verwendung unter Linux Systemen optimiert (insbesondere dem Raspberry Pi), sollte aber theoretisch auf allen Geräten lauffähig sein, auf denen Python3 und die verwendeten Bibliotheken verfügbar sind.
  
In dieser Arbeit wurde bereits ein Modul zur Kommunikation über WiFi entwickelt. Da die zentrale Anwendung modular aufgebaut ist, besteht die Möglichkeit, weitere Kommunikationsmodule zu entwickeln und einzubinden. Die Kommunikation geschieht einseitig von Smarthome Gerät (Client) zu zentraler Anwendung (Server). Diese Designentscheidung erlaubt es, den Code auf dem Smarthome Gerät sehr schlank zu halten und Ressourcen sparend auf der Hardware auszuführen. Dies lässt dem Endanwender viele Ressourcen bei gleichzeitiger hoher Abstraktion, um eigene Anwendungen auf dem Mikrocontroller zu entwickeln.

Das Projekt besteht aus zwei verschiedenen Anwendungsteilen mit jeweils einer eigenen README Datei welche Anweisungen zur Kompilierung und zum Start der jeweiligen Anwendung enthält:
  - Zentrale Instanz: [project/CentralDevice/README.md](project/CentralDevice/README.md)
  - Smarthome Gerät: [project/SmarthomeDevice/ESP8266_8285/README.md](project/SmarthomeDevice/ESP8266_8285/README.md)