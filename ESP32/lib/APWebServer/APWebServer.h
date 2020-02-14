#ifndef APWEBSERVER_H
#define APWEBSERVER_H

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Webpage.h>
#include <Led.h>

#define AP_BUTTON_PIN GPIO_NUM_2

void setUpWiFiResetButton();
void APWebServerSetup();
bool GetWifiConnection();
void listenWiFiResetButton();

#endif