#include "WiFiStationCommunicator.h"
#include "wifi_com_config.h"

const String WiFiStationCommunicator::m_DYNAMIC_CONFIG_FILE = "wsc_config.conf";

String WiFiStationCommunicator::m_deviceId = DEVICE_ID;
String WiFiStationCommunicator::m_centralServerAddress = CENTRAL_SERVER_ADDR;
int WiFiStationCommunicator::m_centralServerPort = CENTRAL_SERVER_PORT;
String WiFiStationCommunicator::m_WiFiSSID = WIFI_SSID;
String WiFiStationCommunicator::m_WiFiPass = WIFI_PASS;
String WiFiStationCommunicator::m_accessPointSSID = ACCESS_POINT_SSID;
String WiFiStationCommunicator::m_accessPointPass = ACCESS_POINT_PASS;
// Convert from seconds to milli seconds
unsigned long WiFiStationCommunicator::m_ApWebserverRuntimeMaxMillis = ACCESSPOINT_WEBSERVER_RUNTIME * 1000L;
unsigned long WiFiStationCommunicator::m_ApWebserverStartMillis = 0;

ESP8266WebServer* WiFiStationCommunicator::m_pWebServer = nullptr;
bool WiFiStationCommunicator::m_wiFiConnected = false;
bool WiFiStationCommunicator::m_apWebserverRunning = false;

//static WiFiStationCommunicator::ESP8266WebServer *server;
String WiFiStationCommunicator::m_sendDataJson = "";
bool WiFiStationCommunicator::m_openAccessPointAndServer = true;
const char WiFiStationCommunicator::m_htmlIndex[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>ESP Config</title>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <body>
        <h1>Device: {DEVICE_ID}</h1>
        <p>The device must be restarted for the changes to take effect.</p>
        <h2>General</h2>
        <form action="/">
            New Device ID:<br><input type="text" name="devie_id" value="{DEVICE_ID}"><br>
            New Central Server Address:<br><input type="text" name="central_server_addr" value="{CENTRAL_SERVER_ADDR}"><br>
            New Central Server Port:<br><input type="text" name="central_server_port" value="{CENTRAL_SERVER_PORT}"><br>
            <input name="submit_general" type="submit" value="Submit">
        </form>
        <h2>WiFi</h2>
        <form action="/">
            New SSID:<br><input type="text" name="wifi_ssid" value="{WIFI_SSID}"><br>
            New Password:<br><input type="text" name="wifi_pass" value=""><br>
            <input name="submit_wifi" type="submit" value="Submit">
        </form>
        <h2>WiFi Access Point</h2>
        <form action="/">
            New SSID:<br><input type="text" name="ap_ssid" value="{AP_SSID}"><br>
            New Password:<br><input type="text" name="ap_pass" value=""><br>
            <input name="submit_ap" type="submit" value="Submit">
        </form>
    </body>
</html>
)rawliteral";

bool WiFiStationCommunicator::connectToWiFi() {
    if (VERBOSE) {
        Serial.println();
        Serial.print("WiFiStationCommunicator: Connect to WiFi Network: ");
        Serial.print(WIFI_SSID);
        Serial.print("...");
        Serial.println();
    }
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Check if connected in increasing interval
    int tryCnt = 1;
    bool wifiFailed = false;
    while (WiFi.status() != WL_CONNECTED && !wifiFailed) {
        // Waiting time is incresing after evry try: 1s, 2s, 3s, 4s, 5s
        delay(1000 * tryCnt);
        if (VERBOSE) {
            Serial.print(".");
        }
        // Stop trying after too many attempts
        if (tryCnt >= 5) {
            if (VERBOSE) {
                Serial.println();
                Serial.println("WiFiStationCommunicator: Try count exceeded.");
            }
            wifiFailed = true;
        }
        tryCnt++;
    }
    // Successfully Connected
    if (!wifiFailed) {
        if (VERBOSE) {
            Serial.println();
            Serial.println("WiFiStationCommunicator: Connected");
            Serial.print("WiFiStationCommunicator: Local IP: ");
            Serial.print(WiFi.localIP());
            Serial.println();
        }
        return true;
    }
    // Connection failed
    else {
        Serial.println("WiFiStationCommunicator: Unable to connect to WiFi Network.");
        return false;
    }
}

bool WiFiStationCommunicator::writeDynamicConfig(String deviceId, String centralServerAddr, int centralServerPort, 
                       String wiFiSSID, String wiFiPass, String apSSID, String apPass) {
    File file = LittleFS.open(m_DYNAMIC_CONFIG_FILE, "w");
    if(!file){
        Serial.println("WiFiStationCommunicator: Failed to open file '" + m_DYNAMIC_CONFIG_FILE + "' for writing");
        return false;
    }
    // Attention: Important that every entry ends with new line (including last one!)
    //            Else loading of config is not possible correctly later
    file.print(deviceId + "\n");
    file.print(centralServerAddr + "\n");
    file.print(String(centralServerPort) + "\n");
    file.print(wiFiSSID + "\n");
    file.print(wiFiPass + "\n");
    file.print(apSSID + "\n");
    file.print(apPass + "\n");
    file.close();
    return true;
}

void WiFiStationCommunicator::loadDynamicConfig() {
    if (VERBOSE) {
        Serial.println("WiFiStationCommunicator: Load dynamic config file..");
    }
    File file = LittleFS.open(m_DYNAMIC_CONFIG_FILE, "r");
    // Nothing to override when no file exist (so hard coded config is used)
    if(!file){
        if (VERBOSE) {
            Serial.println("WiFiStationCommunicator: No dynamic config file available.");
        }
        // Keep config of config.h when no file is available
        return;
    }
    String configStr = file.readString();
    file.close();
    // Parse dynamic config file line by line
    // Every line has stored a dynamic config entry
    bool loaded = false;
    int configIndex = 0;
    do {
        int index = configStr.indexOf("\n");
        if (index > 0) {
            // Get current line
            String config_entry = configStr.substring(0, index);
            // Remove already proceeded lines from string
            configStr = configStr.substring(index + 1, configStr.length() - 1);
            // Override hard coded config when loaded entry is not empty
            // Attention: It is important to check if the entry is empty within the if block, so the else clause at 
            //            the end is only reached when the last line is reached
            // Config: Device id
            if (configIndex == 0) {
                if (config_entry != "") {
                    m_deviceId = config_entry;
                }
            }
            // Config: Central Server Address
            else if (configIndex == 1) {
                if (config_entry != "") {
                    m_centralServerAddress = config_entry;
                }
            }
            // Config: Central Server Port
            else if (configIndex == 2) {
                if (config_entry != "") {
                    m_centralServerPort = config_entry.toInt();
                }
            }
            // Config: WiFi SSD
            else if (configIndex == 3) {
                if (config_entry != "") {
                    m_WiFiSSID = config_entry;
                }
            }
            // Config: WiFi Password
            else if (configIndex == 4) {
                if (config_entry != "") {
                    m_WiFiPass = config_entry;
                }
            }
            // Config: AP SSID
            else if (configIndex == 5) {
                if (config_entry != "") {
                    m_accessPointSSID = config_entry;
                }
            }
            // Config: AP Password
            else if (configIndex == 6) {
                if (config_entry != "") {
                    m_accessPointPass = config_entry;
                }
            }
            else {
                loaded = true;
            }
        }
        else {
            loaded = true;
        }
        configIndex++;
    } while(!loaded);
    if (VERBOSE) {
        Serial.println("WiFiStationCommunicator: loaded.");
    }
}

bool WiFiStationCommunicator::openAccessPoint() {
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: Open access point with SSID: ");
        Serial.print(m_accessPointSSID);
        Serial.print("...");
    }
    bool result = WiFi.softAP(m_accessPointSSID, m_accessPointPass);
    if (!result) {
        Serial.print("WiFiStationCommunicator: Error, can not open Access Point '");
        Serial.print(m_accessPointSSID);
        Serial.print("'");
        Serial.println();
        return false;
    }
    IPAddress IP = WiFi.softAPIP();
    if (VERBOSE) {
        Serial.println("WiFiStationCommunicator: Opened.");
        Serial.print("WiFiStationCommunicator: IP: ");
        Serial.println(IP);
    }
    return true;
}

bool WiFiStationCommunicator::closeAccessPoint() {
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: Close WiFi Access Point...");
    }
    bool result = WiFi.softAPdisconnect(true);
    if (!result) {
        Serial.print("WiFiStationCommunicator: Failed to close WiFi Access Point.");
    }
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: closed.");
    }
    return true;
}

void WiFiStationCommunicator::startWebserver() {
    if (VERBOSE) {
        Serial.println("WiFiStationCommunicator: Start Webserver...");
    }
    // Delete old webserver if running
    if (m_pWebServer) {
        delete m_pWebServer;
        m_pWebServer = nullptr;
    }
    m_pWebServer = new ESP8266WebServer(80);
    // Add functions which is called when basic / request is incoming
    m_pWebServer->on("/", webserverHandleRootRequest);
    m_pWebServer->begin();
    if (VERBOSE) {
        Serial.println("WiFiStationCommunicator: Started.");
    }
}

void WiFiStationCommunicator::stopWebserver() {
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: Stop Webserver...");
    }
    if (m_pWebServer) {
        // Free resources (Webserver is stopped automatically when destructor is called on delete: https://github.com/esp8266/Arduino/issues/686)
        delete m_pWebServer;
        m_pWebServer = nullptr;
    }
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: stopped.");
    }
}

void WiFiStationCommunicator::webserverHandleRootRequest() {
    // Get all submits (when nothing was submittet the string is empty)
    String argSubmitGeneral = m_pWebServer->arg("submit_general");
    String argSubmitWiFi = m_pWebServer->arg("submit_wifi");
    String argSubmitAP = m_pWebServer->arg("submit_ap");
    bool dataChanged = false;
    // Override hard coded config if new config was set (only when new data is not empty)
    // General config was set
    if (argSubmitGeneral != "") {
        //Serial.println("submit_general: '" + argSubmitGeneral + "'");
        String argDeviceId = m_pWebServer->arg("devie_id");
        String argServerAddr = m_pWebServer->arg("central_server_addr");
        String argServerPort = m_pWebServer->arg("central_server_port");
        if (argDeviceId != "") {
            m_deviceId = argDeviceId;
        }
        if (argServerAddr != "") {
            m_centralServerAddress = argServerAddr;
        }
        if (argServerPort != "") {
            m_centralServerPort = argServerPort.toInt();
        }
        dataChanged = true;
    }
    // Wifi config was set
    else if (argSubmitWiFi != "") {
        //Serial.println("argSubmitWiFi: '" + argSubmitWiFi + "'");
        String argWiFiSSID = m_pWebServer->arg("wifi_ssid");
        String argWiFiPass = m_pWebServer->arg("wifi_pass");
        if (argWiFiSSID != "") {
            m_WiFiSSID = argWiFiSSID;
        }
        if (argWiFiPass != "") {
            m_WiFiPass = argWiFiPass;
        }
        dataChanged = true;
    }
    // WiFi Access Point config was set
    else if (argSubmitAP != "") {
        // Serial.println("argSubmitAP: '" + argSubmitAP + "'");
        String argApSSID = m_pWebServer->arg("ap_ssid");
        String argApPass = m_pWebServer->arg("ap_pass");
        if (argApSSID != "") {
            m_accessPointSSID = argApSSID;
        }
        if (argApPass != "") {
            m_accessPointPass = argApPass;
        }
        dataChanged = true;
    }
    // Write new settings to dynamic config file at storage after change
    if (dataChanged) {
        writeDynamicConfig(m_deviceId, m_centralServerAddress, m_centralServerPort, 
                          m_WiFiSSID, m_WiFiPass, m_accessPointSSID, m_accessPointPass);
    }
    // Show index page
    String html = m_htmlIndex;
    // Show current config on web page (replace placeholders with real data, don't show passwords!)
    html.replace("{DEVICE_ID}", m_deviceId);
    html.replace("{CENTRAL_SERVER_ADDR}", m_centralServerAddress);
    html.replace("{CENTRAL_SERVER_PORT}", String(m_centralServerPort));
    html.replace("{WIFI_SSID}", m_WiFiSSID);
    html.replace("{AP_SSID}", m_accessPointSSID);
    m_pWebServer->send(200, "text/html", html);
}

void WiFiStationCommunicator::finalizeData() {
    // No data existing so create a valid empty json string
    if (m_sendDataJson.length() < 1) {
        m_sendDataJson = "{}";
        return;
    }
    // If last char is '}' the String is already Valid json
    if (m_sendDataJson.charAt(m_sendDataJson.length() - 1) == '}') {
        return;
    }
    // Remove trailing comma and make data to valid json string with trailing '}' instead
    if (m_sendDataJson.charAt(m_sendDataJson.length() - 1) == ',') {
        m_sendDataJson.remove(m_sendDataJson.length() - 1);
        m_sendDataJson += "}";
        return;
    }
}

void WiFiStationCommunicator::setup() {
    if (VERBOSE) {
        Serial.print("WiFiStationCommunicator: Device: ");
        Serial.println(m_deviceId);
    }
    // Load dynamic config if file system exists and is mounted
    if(LittleFS.begin()) {
        loadDynamicConfig();
    }
    else {
        Serial.println("WiFiStationCommunicator: An Error has occurred while mounting LittleFS. Can not load dynamic config if existing. Use hard coded config instead.");
    }
    m_wiFiConnected = connectToWiFi();
    // Open Access Point and Webserver if setting is set and no WiFi connection could be etablished
    if (!m_wiFiConnected && m_openAccessPointAndServer) {
        openAccessPoint();
        startWebserver();
        m_apWebserverRunning = true;
        // Store when access point and wevserver was started (time in milli seconds since microcontroller was started)
        // millis() = runtime since start (overflow after ~50 days)
        m_ApWebserverStartMillis = millis();
    }
    // startWebserver(); // DEBUG
}

void WiFiStationCommunicator::loop() {
    if (m_apWebserverRunning) {
        // Handle Webserver Client requests
        m_pWebServer->handleClient();
        // Check if Access Point and Webserver should be closed (if a max runtime is specified in config)
        if (m_ApWebserverRuntimeMaxMillis > 0) {
            // Get total runtime
            unsigned long runtime = millis() - m_ApWebserverStartMillis;
            // Stop if runtime exceeded it's max defined runtime
            if (runtime >= m_ApWebserverRuntimeMaxMillis) {
                if (VERBOSE) {
                    Serial.println("WiFiStationCommunicator: Max Access Point Webserver Runtime reached. Stop Access Point and Webserver.");
                    Serial.println("WiFiStationCommunicator: Stop Webserver...");
                }
                stopWebserver();
                closeAccessPoint();
                if (VERBOSE) {
                    Serial.println("WiFiStationCommunicator: Stopped.");
                }
            }
        }
    }
}

// A Webserver will be started after, so it is possible to connect and add new wifi credentials and change basic settings
void WiFiStationCommunicator::settingOpenAccessPointAndWebserverIfNoWiFi(bool open) {
    m_openAccessPointAndServer = open;
}

bool WiFiStationCommunicator::isWiFiConnected() {
    return m_wiFiConnected;
}

bool WiFiStationCommunicator::isAPWebserverActive() {
    return m_apWebserverRunning;
}

void WiFiStationCommunicator::resetData() {
    m_sendDataJson = "{\"device_id\":\"" + m_deviceId + "\",";
}

void WiFiStationCommunicator::addDataString(String key, String value) {
    // Escape double quotes: " => \"
    value.replace("\"", "\\\"");
    m_sendDataJson += "\"" + key + "\":\"" + value + "\",";
}

void WiFiStationCommunicator::addDataBoolean(String key, bool value) {
    String value_str = value ? "true" : "false";
    m_sendDataJson += "\"" + key + "\":" + value_str + ",";
}

bool WiFiStationCommunicator::sendDataToStation() {
    // Data has to be valid json string, so finalize to ensure it is
    finalizeData();
    if (VERBOSE) {
        Serial.println("\tWiFiStationCommunicator: Data to Send:");
        Serial.println(m_sendDataJson);
    }
    WiFiClient wiFiClient;
    // Send data via http
    HttpClient httpClient(wiFiClient, m_centralServerAddress, m_centralServerPort);
    // Data is send as json string, so set correct conrent type
    String contentType = "application/json";
    String postData = m_sendDataJson;
    // Send the data to central station
    httpClient.post("/receiver", contentType, postData);
    int responseCode = httpClient.responseStatusCode();
    if (responseCode > 0) {
        String response = httpClient.responseBody();
        if (VERBOSE) {
            Serial.print("WiFiStationCommunicator: Http Response: ");
            Serial.println(response);
        }
    }
    else {
        Serial.print("WiFiStationCommunicator: Error while POST request to: ");
        Serial.println(m_centralServerAddress);
        Serial.print("WiFiStationCommunicator: Error code: ");
        Serial.println(responseCode);
        return false;
    }
    return true;
}