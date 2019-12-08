#include <APWebServer.h>
#include <Led.h>

void setupButton();
void changeToAPSTAMode();
void changeToSTAMode();
void webserverSetup();
void webserverBegin();
void webserverStop();
bool connectWiFi(const String &ssid, const String &passwd, int timeout);
void handleRoot();
void handleConfig();
void handleConnectWiFi();
void handleNotFound();

// STA configs
String STAssid;
String STApassword;

// softAP configs
String APssid = "Aulas Sustentables (" + WiFi.macAddress().substring(9) + ")";
const char *APpassword = "aulassustentables";
const int APchannel = 1;
const int APhidden = 0;
const int APmaxConnections = 2;

WebServer server(80);

bool canConnect = false;

// ------------------ Webserver -------------------

void webserverSetup() {
    Serial.println("Setting webserver");
    server.on("/", handleRoot);
    server.on("/config", HTTP_GET, handleConfig);
    server.on("/connect", HTTP_POST, handleConnectWiFi);
    server.onNotFound(handleNotFound);
}

void webserverBegin() {
    server.begin();
    Serial.println("HTTP server started");
}

void webserverStop() {
    server.stop();
    Serial.println("webserver detenido");
}

void handleConfig() {
    server.send(200, "text/html", webpageHTML);
    return;
}

void handleConnectWiFi() {
    int codigoRespuesta = 400;  // Bad request
    if (server.hasArg("ssid") && server.hasArg("password")) {
        if (connectWiFi(server.arg("ssid"), server.arg("password"), 8)) {
            codigoRespuesta = 200;
            canConnect = true;
        } else {
            codigoRespuesta = 401;  // Unauthorized
        }
    }
    server.send(codigoRespuesta);
    return;
}

//root page can be accessed only if authentification is ok
void handleRoot() {
    server.sendHeader("Location", "/config");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
}

//no need authentification
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

// ----------------- Station mode ------------------

void changeToSTAMode() {
    // Disable AP mode and only enable STA mode.
    if (WiFi.enableAP(false) && WiFi.enableSTA(true)) {
        Serial.println("WIFI mode: STATION");
    } else {
        Serial.println("ERROR: changeToSTAMode()");
    }
}

bool connectWiFi(const String &ssid, const String &passwd, int timeout) {
    char ssidAux[ssid.length() + 1];
    char passwdAux[passwd.length() + 1];

    ssid.toCharArray(ssidAux, sizeof(ssidAux));
    passwd.toCharArray(passwdAux, sizeof(passwdAux));

    // Connect to Wi-Fi network with SSID and password
    WiFi.begin(ssidAux, passwdAux);

    int tryCount = 0;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && tryCount <= timeout) {
        delay(1000);
        tryCount++;
        Serial.print(".");
    }
    Serial.println("");

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("TIMEOUT, no se pudo conectar al wifi");
        return false;
    } else {
        Serial.println("");
        IPAddress IP = WiFi.localIP();
        Serial.println("Connection stablish!");
        Serial.print("STA IP address: ");
        Serial.println(IP);
        WiFi.setAutoReconnect(true);
        // printNetworkInfo();
        return true;
    }
}

// ----------------- Access Point and Station mode ------------------

void changeToAPSTAMode() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Disconnecting from wifi…");
        WiFi.setAutoReconnect(false);
        WiFi.disconnect(false, false);  // disconnect station without turn off wifi and erase config.
    }
    if (WiFi.enableAP(true) && WiFi.enableSTA(true)) {
        Serial.println("WIFI mode: AP - STA");
        // Enable AP and STA mode. Sta is disconnected from current wifi.
        Serial.println("Setting AP (Access Point)");
        bool softAP = WiFi.softAP(APssid.c_str(), APpassword, APchannel, APhidden, APmaxConnections);
        if (!softAP) {
            Serial.println("ERROR: softAP()");
        } else {
            IPAddress IP = WiFi.softAPIP();
            Serial.print("AP IP address: ");
            Serial.println(IP);
            // printNetworkInfo();
            Serial.println("ESP en modo AP y STA");
            webserverBegin();
        }
    } else {
        Serial.println("ERROR: changeToAPSTAMode()");
    }
}

// --------------------- BUTTON -----------------------

#define BUTTON_PIN GPIO_NUM_2

struct Button {
    const uint8_t PIN;
    bool state;
    bool pressed;
    ulong pressTime;
};

Button button = {BUTTON_PIN, false, false, 0};

void IRAM_ATTR buttonInterrupt() {
    button.pressTime = millis();
    button.pressed = true;
}

void setupButton() {
    pinMode(button.PIN, PULLUP);
    attachInterrupt(button.PIN, buttonInterrupt, FALLING);
}

void buttonListener() {
    if (button.pressed) {
        if (digitalRead(button.PIN) == LOW) {
            ulong readTime;
            readTime = millis();
            if ((readTime - button.pressTime) > 1000) {
                Serial.println("Button pressss!");
                button.pressed = false;
                blinkLed();
                WiFi.disconnect(true, true);
                ESP.restart();
            }
        } else {
            button.pressed = false;
        }
    }
}

void APWebServerSetup(void) {
    setupButton();
    webserverSetup();

    WiFi.mode(WIFI_STA);
    WiFi.begin();

    Serial.println("");

    int timeout = 0;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && timeout <= 5) {
        delay(1000);
        timeout++;
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No se pudo conectar al wifi");
        changeToAPSTAMode();

    } else {
        Serial.println("");
        IPAddress IP = WiFi.localIP();
        Serial.print("STA IP address: ");
        Serial.println(IP);
        // printNetworkInfo();
    }
}

void APWebServerLoop(void) {
    buttonListener();
    if (WiFi.getMode() == WIFI_MODE_APSTA) {
        server.handleClient();
        if (canConnect) {
            if (!server.client() || !server.client().connected()) {
                changeToSTAMode();
                webserverStop();
                canConnect = false;
            }
        }
    }
}

void listenButtonWiFiReset() {
    buttonListener();
}