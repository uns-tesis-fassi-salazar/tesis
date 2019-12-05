#include <Arduino.h>
#include <APWebServer.h>
#include <FirebaseModule.h>
#include <Sensors.h>
#include <Led.h>
#include <Utils.h>

#include <WiFi.h>

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

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    // setupInet();

    APWebServerSetup();
    while (WiFi.status() != WL_CONNECTED) {
        APWebServerLoop();
    }

    Serial.print("MAC Address: ");
    nodoMac = WiFi.macAddress();
    Serial.println(nodoMac);

    setupFirebase(nodoMac);

    if (nodoMac != "CC:50:E3:B6:29:48") sensorsSetup();

    // Inicializacion actuadores
    setUpLed();

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