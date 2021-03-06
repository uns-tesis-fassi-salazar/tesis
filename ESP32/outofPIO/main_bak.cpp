#include <Arduino.h>
#include <APWebServer.h>
#include <FirebaseModule.h>
#include <Sensors.h>
#include <Led.h>
#include <utilidades.h>

#include <WiFi.h>

#define AULA_ASIGNADA "AulaAsignada"
#define ACTUADOR_LED "Led"

// Funciones
int lookupActiveMode();
void printFirebaseResult(FirebaseData &data);

String ip;
String nodoMac;

bool activeMode = 0;
int secondsToSleep = 4;
int seccodsBetweenReads = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    APWebServerSetup();
    while (WiFi.status() != WL_CONNECTED) {
        APWebServerLoop();
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
            Serial.print("Hall: ");
            Serial.println(hallRead());
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