# Documentation
The ```WiFiStationCommunicator``` Class implements methods to make communication with a WebServer via WiFi in the same network easy.  

## Preparation
The following files have to be placed inside your Arduino sketch root:
- ```WiFiStationCommunicator.h```
- ```WiFiStationCommunicator.cpp```
- ```wifi_com_config.h```
And the config file has to be configured correctly.

## How to use
### General
To implement the class to your project:  
```
#include "WiFiStationCommunicator.h"
```

Inside the ```setup()``` function of your project you have to call the setup method of the class and can then check if the WiFi connection could be etablished:  
```
void setup() {
    [...]
    WiFiStationCommunicator::setup();
    if (!WiFiStationCommunicator::isWiFiConnected()) {
        Serial.print("No WiFi connection.");
    }
    [...]
}
```
If no WiFi connection could be etablished a WiFi Access Point is started automatically.  
  
At the beginning of the ```loop()``` function you have to call the loop method of the WiFiCommunicator class: 
```
void loop() {
    WiFiStationCommunicator::loop();
    [...]
}
```
### Send data
Before adding new data you have to call reset method:  
```
WiFiStationCommunicator::resetData();
```
You can add data for sending with the following methods:  
```
void addDataString(String key, String value);
void addDataBoolean(String key, bool value);
// Compatible with any primitive number type (int, float, double etc)
void addDataNumber(String key, ValueType value);
```
To send data:  
```
WiFiStationCommunicator::sendDataToStation();
```
Example:
```
WiFiStationCommunicator::resetData();
WiFiStationCommunicator::addDataString("Notice", "Send from Smart Device);
WiFiStationCommunicator::addDataBoolean("celcius", true);
WiFiStationCommunicator::addDataNumber("temperature", 30.6);
WiFiStationCommunicator::sendDataToStation();
```
### Deep Sleep
Before going into deep sleep mode, it is recommended to check if the webserver (and so also wifi access point) is active. Because else the Webserver and the WiFi Access Point would be not reachable anymore.
```
if (!WiFiStationCommunicator::isAPWebserverActive()) {
    // Going into deep sleep is fine
}
```

### Addional Configuration
To deactivate the start of a WiFi Access Point and Webserver when no WiFi connection is available you can call the ```settingOpenAccessPointAndWebserverIfNoWiFi()``` Method before calling the classes init method:
```
WiFiStationCommunicator::settingOpenAccessPointAndWebserverIfNoWiFi(false);
```
