#include "WiFiStationCommunicator.h"

void setup() {
    // Init serial output
    Serial.begin(9600);
    // Connect to WiFi and central station
    WiFiStationCommunicator::setup();
    if (!WiFiStationCommunicator::isWiFiConnected()) {
        Serial.print("No WiFi connection.");
    }
}

void loop() {
    WiFiStationCommunicator::loop();
    if (!WiFiStationCommunicator::isAPWebserverActive() && 
         WiFiStationCommunicator::isWiFiConnected()) {
        // Send data to station
        WiFiStationCommunicator::resetData();
        WiFiStationCommunicator::addDataBoolean("event", true);
        WiFiStationCommunicator::sendDataToStation();
        // Go into deep sleep until device is woken up/resetet by external event
        ESP.deepSleep(0);
    }
}