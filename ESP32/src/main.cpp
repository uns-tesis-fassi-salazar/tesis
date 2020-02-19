#include <Arduino.h>
#include <FirebaseModule.h>
#include <DBStream.h>
#include <OTAUpdater.h>
#include <APWebServer.h>
#include <Sensores.h>
#include <Actuadores.h>
#include <Automata.h>
#include <IRModule.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Utilidades.h>
#include "time.h"

// Constante para la versión del programa
const char FIRMWARE_VERSION[] = ("__AuLaSSuStEnTaBlEs__ " __DATE__ " " __TIME__ "__");
// Constantes para el protocolo NTP
const char *ntpServer = "pool.ntp.org";
const long GMTOffset_sec = 3600*(-3);   //UTC -3: Buenos Aires
const int daylightOffset_sec = 0;

// Funciones
bool hasWifiConfig();
void printLocalTime();

String aulaKey = "";
String firmwareVersion = "";
int segundosADormir = 5;
int tiempoEntreLecturas = 30;      // 30 segundos
int tiempoEntreChequeos = 60;      // 1 minutos
ulong tiempoUltimaLecturaSensores = 0;
ulong tiempoUltimoChequeo = 0;
bool blink = false;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }

    setUpWiFiResetButton();
    setUpRecordCommandButton();

    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);

    if (!hasWifiConfig()) {
        APWebServerSetup();
        while (!GetWifiConnection()) { }
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
    IPAddress IP = WiFi.localIP();
    if ((WiFi.status() != WL_CONNECTED) && (WiFi.status() == WL_CONNECT_FAILED)) {
        Serial.println("No se pudo conectar al wifi");
    } else {
        Serial.println("");
        Serial.print("STA IP address: ");
        Serial.println(IP);
    }

    Serial.println("MAC Address: " + WiFi.macAddress());

    // init and get the time
    configTime(GMTOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTimeSpanish();

    // Inicializacion Firebase
    setUpFirebase(FIRMWARE_VERSION);

    // Forzar recargar configuracion del aula
    resetAulaConfig();
    
    // Inicializacion sensores
    setUpSensors();

    // Inicializacion del Stream de datos para los comandos que recibe el ESP32
    setUpStream(FIRMWARE_VERSION);

    // Inicializacion actuadores
    setUpActuadores();

    String iniDet;
    iniDet = "MAC Address: " + WiFi.macAddress();
    iniDet += "\nSTA IP address: " + IP;
    iniDet += "\nVirmware Version: " + String(FIRMWARE_VERSION);
    iniDet += "\nUpdate: Prueba Final - Toma 5";
    iniDet += "\n*** ESP32 Setup OK ***";
    uploadLogs(iniDet);
    Serial.println("*** ESP32 Setup OK ***");
}

void loop() {

    listenWiFiResetButton();
    listenRecordCommandButton();

    if (WiFi.status() == WL_CONNECTED) {
        if (aulaKey != "") {
            if (lapTimer(tiempoEntreLecturas*1000, &tiempoUltimaLecturaSensores)) {
                loopSensors();
            }
            if (lapTimer(tiempoEntreChequeos*1000, &tiempoUltimoChequeo)) {
                checkAulaState();
            }
            loopStream();
        } else {
            uploadLogs("Esperando asignacion de aula...");
            // printLocalTimeSpanish();
            delay(1000 * segundosADormir);  // esperando configuracion...
            aulaKey = getAulaAsignada();
            uploadLogs("Aula obtenida: " + aulaKey+".");
            uploadLogs("Buscando configuracion del aula...");
            resetAulaConfig();
            checkAulaState();
            // checkFirmwareVersion(FIRMWARE_VERSION);
        }
    }
    yield();
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