#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

// softAP configs
const char* APssid = "FS";
const char* APpassword = "enterenter";
const int APchannel = 1;
const int APhidden = 0;
const int APmaxConnections = 1;

WebServer server(80);

void setupButton();
void debounceButton();
void changeToAPSTAMode();

//Check if header is present and correct
bool is_authentified() {
    Serial.println("Enter is_authentified");
    if (server.hasHeader("Cookie")) {
        Serial.print("Found cookie: ");
        String cookie = server.header("Cookie");
        Serial.println(cookie);
        if (cookie.indexOf("ESPSESSIONID=1") != -1) {
            Serial.println("Authentification Successful");
            return true;
        }
    }
    Serial.println("Authentification Failed");
    return false;
}

//login page, also called for disconnect
void handleLogin() {
    String msg;
    if (server.hasHeader("Cookie")) {
        Serial.print("Found cookie: ");
        String cookie = server.header("Cookie");
        Serial.println(cookie);
    }
    if (server.hasArg("DISCONNECT")) {
        Serial.println("Disconnection");
        server.sendHeader("Location", "/login");
        server.sendHeader("Cache-Control", "no-cache");
        server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
        server.send(301);
        return;
    }
    if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
        if (server.arg("USERNAME") == "admin" && server.arg("PASSWORD") == "admin") {
            server.sendHeader("Location", "/");
            server.sendHeader("Cache-Control", "no-cache");
            server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
            server.send(301);
            Serial.println("Log in Successful");
            return;
        }
        msg = "Wrong username/password! try again.";
        Serial.println("Log in Failed");
    }
    String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
    content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
    content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
    content += "You also can go <a href='/inline'>here</a></body></html>";
    server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
    Serial.println("Enter handleRoot");
    String header;
    if (!is_authentified()) {
        server.sendHeader("Location", "/login");
        server.sendHeader("Cache-Control", "no-cache");
        server.send(301);
        return;
    }
    String content = "<html><body><H2>hello, you successfully connected to esp8266!</H2><br>";
    if (server.hasHeader("User-Agent")) {
        content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
    }
    content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
    server.send(200, "text/html", content);
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

void setup(void) {
    Serial.begin(9600);
    setupButton();
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
    }
}

void loop(void) {
    if (WiFi.getMode() == WIFI_MODE_APSTA ) {
        server.handleClient();
    }
    debounceButton();
}

void changeToAPSTAMode() {
    if (WiFi.mode(WIFI_AP_STA)) {
        // Connect to Wi-Fi network with SSID and password
        Serial.print("Setting AP (Access Point)…");
        WiFi.softAP(APssid, APpassword, APchannel, APhidden, APmaxConnections);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        Serial.println("ESP en modo AP y STA");

        Serial.print("Setting webserver");
        server.
        server.on("/", handleRoot);
        server.on("/login", handleLogin);
        server.on("/inline", []() {
            server.send(200, "text/plain", "this works without need of authentification");
        });

        server.onNotFound(handleNotFound);
        //here the list of headers to be recorded
        const char* headerkeys[] = {"User-Agent", "Cookie"};
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
        //ask server to track these headers
        server.collectHeaders(headerkeys, headerkeyssize);
        server.begin();
        Serial.println("HTTP server started");
    } else {
        Serial.println("Ups! error en wifi.mode(WIFI_AP_STA)");
    }
}

// --------------------- BUTTON -----------------------
const int buttonPin = 25;

int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setupButton() {
    pinMode(buttonPin, PULLDOWN);
}

// TODO: button with interruption
void debounceButton() {
    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState) {
            buttonState = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonState == HIGH) {
                Serial.println("Button pressss!");
                changeToAPSTAMode();
            }
        }
    }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;
}