#ifndef APWEBSERVER_H
#define APWEBSERVER_H

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

void APWebServerSetup();
void APWebServerLoop();
void listenButtonWiFiReset();

#endif