#include "WiFiStationCommunicator.h"
// I2C communication with BMP180 sensor
#include <Wire.h>
// Library for BMP180 sensor (Adafruit BMP085 Library)
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmpSensor;

void setup() {
    // Init serial output
    Serial.begin(9600);
    // Connect to WiFi and central station
    WiFiStationCommunicator::setup();
    if (!WiFiStationCommunicator::isWiFiConnected()) {
        Serial.print("No WiFi connection.");
    }
    // Connect to BMP180 sensor
    if (!bmpSensor.begin()) {
        Serial.print("No BMP180 sensor found.");
    }
}

void loop() {
    WiFiStationCommunicator::loop();
    if (!WiFiStationCommunicator::isAPWebserverActive() && 
         WiFiStationCommunicator::isWiFiConnected()) {
        // Read sensor data
        double temperature = bmpSensor.readTemperature();
        double pressure = bmpSensor.readPressure();
        // Send data to station
        WiFiStationCommunicator::resetData();
        WiFiStationCommunicator::addDataNumber("temperature", temperature);
        WiFiStationCommunicator::addDataNumber("pressure", pressure);
        WiFiStationCommunicator::sendDataToStation();
        // Go into deep sleep for 1 hour (Port D0 and RST has to be connected)
        ESP.deepSleep(60 * 60 * 1000000);
        //ESP.deepSleep(3 * 1000000);
    }
}