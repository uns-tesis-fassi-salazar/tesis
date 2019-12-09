#include <Arduino.h>
#include <APWebServer.h>
#include <FirebaseModule.h>
#include <Sensors.h>
#include <AC.h>
#include <Led.h>
#include <Utils.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define AULA_ASIGNADA "AulaAsignada"
#define ACTUADOR_LED "Led"

// Funciones
int lookupActiveMode();
void printFirebaseResult(FirebaseData &data);

String ip;
String nodoMac;

boolean activeMode = 0;
int secondsToSleep = 4;
int seccodsBetweenReads = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }

    setupWiFiResetButton();

    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_STA,&wifi_config);
    
    // wifi_config_t wifi_config_aux;
    // memset(&wifi_config_aux, 165, sizeof(wifi_config_t));
    // Serial.println(wifi_config.sta.ssid[0] == 0);
    // for (size_t i = 0; i < 32; i++)
    // {
    //     Serial.println(int(wifi_config.sta.ssid[i]));
    // }
    // if (memcmp(&wifi_config_aux.sta,&wifi_config.sta,sizeof(wifi_sta_config_t)) == 0) {
    //     Serial.println("Config Vacia");
    //     Serial.println(int(wifi_config.sta.ssid[0]));
    //     uint8_t aux = wifi_config.sta.ssid[0] & 0b10100101;
    //     Serial.println(aux);
    // } else {
    //     Serial.println("Start WiFi Info: ");
    //     Serial.println((char *)wifi_config.sta.ssid);
    //     Serial.println((char *)wifi_config.sta.password);
    //     Serial.println("End WiFi Info.");
    // }

    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

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

    Serial.println("");
    
    // WL_CONNECT_FAILED
    if ((WiFi.status() != WL_CONNECTED) && (WiFi.status() == WL_CONNECT_FAILED)) {
        Serial.println("No se pudo conectar al wifi");
        APWebServerSetup();
        while (!GetWifiConnection()) { }
    } else {
        Serial.println("");
        IPAddress IP = WiFi.localIP();
        Serial.print("STA IP address: ");
        Serial.println(IP);
    }

    Serial.print("MAC Address: ");
    nodoMac = WiFi.macAddress();
    Serial.println(nodoMac);

    // Inicializacion Firebase
    setupFirebase(nodoMac);

    // Inicializacion sensores
    sensorsSetup();

    // Inicializacion actuadores
    setUpLed();
    setUpAC();

    Serial.println("*** Setup OK ***");
}

void loop() {
    listenButtonWiFiReset();

    if (WiFi.status() == WL_CONNECTED) {
        if (activeMode) {
            loopSensors();
            // delay(1000 * seccodsBetweenReads);
        } else {
            Serial.println("Esperando asignacion de aula...");
            delay(1000 * secondsToSleep);  // esperando configuracion...
            activeMode = lookupActiveMode();
        }
    }
}

int lookupActiveMode() {
    int salida;
    readData(AULA_ASIGNADA, &salida);
    return salida;
}