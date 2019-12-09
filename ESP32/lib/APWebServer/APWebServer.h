#ifndef APWEBSERVER_H
#define APWEBSERVER_H

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Webpage.h>

void setupWiFiResetButton();
void APWebServerSetup();
bool GetWifiConnection();
void listenButtonWiFiReset();

#endif