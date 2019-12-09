#include <APWebServer.h>
#include <Arduino.h>
#include <esp_wifi.h>

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }
    setupWiFiResetButton();

    

    Serial.println("Get config antes de intentar conectar al wifi");
    wifi_config_t wifi_config;
    esp_err_t err;
    // Muy importante pedir la configuracion del wifi luego de habilitar la interfaz STA
    err = esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

    if (err == ESP_OK) {
        Serial.println("ESP_OK");
    } else {
        Serial.printf("ESP_ERR: %i", err);
    }

    for (size_t i = 0; i < 32; i++) {
        Serial.printf("%c",wifi_config.sta.ssid[i]);
        /* code */
    }

    Serial.println("");


    wifi_config_t wifi_config_aux;
    memset(&wifi_config_aux, 0, sizeof(wifi_config_t));

    if (memcmp(wifi_config.sta.ssid, wifi_config_aux.sta.ssid, sizeof(uint8_t)*32) == 0) {
        Serial.println("Config Vacia");
        APWebServerSetup();
        while (!GetWifiConnection()) { }
    } else {
        Serial.println("Config establecida");
        WiFi.begin();
    }

    WiFi.setAutoReconnect(true);

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

    } else {
        Serial.println("");
        IPAddress IP = WiFi.localIP();
        Serial.print("STA IP address: ");
        Serial.println(IP);
    }

    Serial.println("Get config despues de intentar conectar al wifi");
    wifi_config_t wifi_config2;
    err = esp_wifi_get_config(WIFI_IF_STA, &wifi_config2);

    if (err == ESP_OK) {
        Serial.println("ESP_OK");
    } else {
        Serial.printf("ESP_ERR: %i", err);
    }

    for (size_t i = 0; i < 32; i++) {
        Serial.printf("%c",wifi_config.sta.ssid[i]);
        /* code */
    }

    Serial.println("");

    Serial.println("*** Setup OK ***");
}

void loop() {
    listenButtonWiFiReset();
    yield();
}