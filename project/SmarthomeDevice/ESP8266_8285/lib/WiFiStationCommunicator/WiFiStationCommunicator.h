#ifndef WIFI_STATION_COMMUNICATOR_H
#define WIFI_STATION_COMMUNICATOR_H

#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// https://github.com/arduino-libraries/ArduinoHttpClient
#include <ArduinoHttpClient.h>

class WiFiStationCommunicator {
    private:
        static const String m_DYNAMIC_CONFIG_FILE;
        // General Config 
        static String m_deviceId;
        static String m_centralServerAddress;
        static int m_centralServerPort;
        // WiFi Config
        static String m_WiFiSSID;
        static String m_WiFiPass;
        // Access Point Config
        static String m_accessPointSSID;
        static String m_accessPointPass;
        // Addional Config
        // Max runtime of webserver before closing in milliseconds
        static unsigned long m_ApWebserverRuntimeMaxMillis;
        // When the Access Point and webserver was started (in milliseconds since microcontroller was started)
        static unsigned long m_ApWebserverStartMillis;

        // Webserver running on ESP for setting settings
        // Used a pointer, because Webserver is not always running and as pointer the storage can allocate and freed dynamically
        static ESP8266WebServer *m_pWebServer;
        static bool m_wiFiConnected;
        static bool m_apWebserverRunning;

        // Stores data as json string. Json string is created on the fly while adding data and later used to send it to the central station
        static String m_sendDataJson;
        // Open Access Point if no WiFi Connection could be established if true on init
        // Can be modified before calling setup() with function settingOpenApIfNoWifiAvailable()
        static bool m_openAccessPointAndServer;
        // PROGMEM: Store data at flash storage instead of SRAM
        // https://www.arduino.cc/reference/de/language/variables/utilities/progmem/
        static const char m_htmlIndex[] PROGMEM;
    
    private:
        static bool connectToWiFi();

        // Stores new config data (overrides the hard coded config data) on mikrocontroller storage
        static bool writeDynamicConfig(String deviceId, String centralServerAddr, int centralServerPort, 
                               String wiFiSSID, String wiFiPass, String apSSID, String apPass);
        // Loads the dynamic confog data and override the hard coded config
        static void loadDynamicConfig();

        // Opens an wifi access point users can connect to 
        static bool openAccessPoint();
        static bool closeAccessPoint();
        // Start the webserver, so users can connect
        static void startWebserver();
        // Stop webserver and free resources
        static void stopWebserver();
        // Handles the incoming requests of the server (Standart page/ index and submitted data)
        static void webserverHandleRootRequest();

        // Finalize the string for sending (make it to valid json string)
        static void finalizeData();

    public:
        // Do basic initializations, Start necesarry connections, servers etc
        static void setup();
        // Has to be called frequently, so repeating tasks can be done
        static void loop();

        // When called before init, it can be defined if a access point and webserver should be startet, when no wifi is available
        // A Webserver will be started after, so it is possible to connect and add new wifi credentials and change basic settings 
        static void settingOpenAccessPointAndWebserverIfNoWiFi(bool open);
        static bool isWiFiConnected();
        // True if Access Point and Webserver are running
        static bool isAPWebserverActive();

        // Reset the data stored in json string. The device is always send to central station
        // So it is added to json string already after reset 
        static void resetData();
        static void addDataString(String key, String value);
        static void addDataBoolean(String key, bool value);
        // Functions for adding data for sending later. Data is stored as json so the added data are stored ins string in json format
        // General/Generic way to create json line
        // Note: Template methods can not be declared in source file
        template<typename ValueType>
        static void addDataNumber(String key, ValueType value) {
            m_sendDataJson += "\"" + key + "\":" + String(value) + ",";
        }
        // Sends the data to the central station
        static bool sendDataToStation();
};

#endif // WIFI_STATION_COMMUNICATOR_H
