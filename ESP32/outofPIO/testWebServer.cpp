#include <APWebServer.h>


void setup() {
    Serial.begin(9600);
    APWebServerSetup();
}

void loop() {
    listenButtonWiFiReset();
    APWebServerLoop();
}