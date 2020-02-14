#include <Arduino.h>
#include <FirebaseModule.h>
#include <DBStream.h>
#include <OTAUpdater.h>
#include <APWebServer.h>
#include <Sensores.h>
#include <Actuadores.h>
#include <IRModule.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Utils.h>

const char FIRMWARE_VERSION[] = ("__AuLaSSuStEnTaBlEs__ " __DATE__ " " __TIME__ "__");

// Funciones
bool hasWifiConfig();

String aulaKey = "";
String firmwareVersion = "";
int secondsToSleep = 5;
int seccodsBetweenUploads = 5;
ulong tiempoUltimaLecturaSensores = 0;
ulong currentTime = 0;
ulong nextTimeCheck = 1000*60;

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
    setUpFirebase();

    // Actualiza la versión del firmware en Firebase
    updateFirmwareVersion(FIRMWARE_VERSION);

    // Inicializacion del Stream de datos para los comandos que recibe el ESP32
    setUpStream(FIRMWARE_VERSION);

    // Inicializacion sensores
    setUpSensors();
    
    // Inicializacion actuadores
    setUpActuadores();

    Serial.println("*** Setup OK ***");
}

void loop() {
    listenButtonWiFiReset();

    if (WiFi.status() == WL_CONNECTED) {
        if (aulaKey != "") {
            if (lapTimer(seccodsBetweenUploads*1000, &tiempoUltimaLecturaSensores)) {
                loopSensors();
            }
            loopStream();
        } else {
            Serial.println("Esperando asignacion de aula...");
            delay(1000 * secondsToSleep);  // esperando configuracion...
            aulaKey = getAulaAsignada();
            checkFirmwareVersion(FIRMWARE_VERSION);
        }
    }
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