#include <Arduino.h>
#include <APWebServer.h>
#include <FirebaseModule.h>
#include <Sensors.h>
#include <IRModule.h>
#include <AC.h>
#include <Led.h>
#include <utilidades.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include <otaUpdater.h>

#define AULA_ASIGNADA "AulaAsignada"
#define ACTUADOR_LED "Led"

// Funciones
int lookupActiveMode();
bool hasWifiConfig();

boolean activeMode = 0;
int secondsToSleep = 4;
int seccodsBetweenUploads = 5*1000;
ulong tiempoUltimaLecturaSensores = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }

    setupWiFiResetButton();

    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);

    if (!hasWifiConfig()) {
        APWebServerSetup();
        while (!GetWifiConnection()) { }
    } else {
        Serial.println("Config establecida");
    }

    WiFi.begin();
    WiFi.setAutoReconnect(false);

    int timeout = 0;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && timeout <= 5) {
        delay(1000);
        timeout++;
        Serial.print(".");
    }

    // WL_CONNECT_FAILED
    if ((WiFi.status() != WL_CONNECTED) && (WiFi.status() == WL_CONNECT_FAILED)) {
        Serial.println("No se pudo conectar al wifi");
    } else {
        Serial.println("");
        IPAddress IP = WiFi.localIP();
        Serial.print("STA IP address: ");
        Serial.println(IP);
    }

    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Inicializacion Firebase
    setupFirebase(WiFi.macAddress());

    // Inicializacion sensores
    sensorsSetup();

    // Inicializacion actuadores
    setupIRReceiver();

    setUpLed();
    setUpAC();

    Serial.println("*** Setup OK ***");
}

void loop() {
    listenButtonWiFiReset();

    if (WiFi.status() == WL_CONNECTED) {
        if (activeMode) {
            if (lapTimer(seccodsBetweenUploads, &tiempoUltimaLecturaSensores)) {
                loopSensors();
            }
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

bool hasWifiConfig() {
    bool hasWifiConfig = true;
    wifi_config_t wifi_config;
    esp_err_t err;
    // Muy importante pedir la configuracion del wifi luego de habilitar la interfaz STA
    err = esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

    if (err == ESP_OK) {
        Serial.println("ESP_OK");
    } else {
        Serial.printf("ESP_ERR, code: %i", err);
    }

    wifi_config_t wifi_config_aux;
    memset(&wifi_config_aux, 0, sizeof(wifi_config_t));

    if (memcmp(wifi_config.sta.ssid, wifi_config_aux.sta.ssid, sizeof(uint8_t)*32) == 0) {
        Serial.println("Config Vacia");
        hasWifiConfig = false;
    } else {
        Serial.println("Config establecida");
    }

    return hasWifiConfig;
}