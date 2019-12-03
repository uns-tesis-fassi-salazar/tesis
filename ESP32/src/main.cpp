#include <APWebServer.h>
#include <FirebaseModule.h>
#include <Led.h>

#include <WiFi.h>

#include <BH1750.h>
#include <DHT.h>
#include <Wire.h>

// #define WIFI_SSID "DIEGO"
// #define WIFI_PASSWORD "MontyPython"

#define SENSOR_HUMEDAD "Humedad"
#define SENSOR_TEMPERATURA "Temperatura"
#define SENSOR_LUX "Luminocidad"
#define SENSOR_MOV "Movimiento"
#define AULA_ASIGNADA "AulaAsignada"
#define ACTUADOR_LED "Led"

// Funciones
void setupInet();
void setupSensors();
int lookupActiveMode();
void printTittle();
void doActions();
void printFirebaseResult(FirebaseData &data);
bool lapTimer(ulong lapTime, ulong *lastLapTime);

// Variables
BH1750 lightMeter;
DHT dht;

FirebaseJson jsonSensorData;

String ip;
String nodoMac;

float luxValue, tempValue, humidityValue;
int movementValue;
int luxPortSDA = GPIO_NUM_22;
int luxPortCSL = GPIO_NUM_21;
int dhtPort = GPIO_NUM_5;
// int ledPort = GPIO_NUM_32;
int movementSensorPort = GPIO_NUM_25;
// int turnOnOffLed = 0;

boolean activeMode = 0;
int secondsToSleep = 4;
int seccodsBetweenReads = 0;

ulong tiempoUltimaLecturaDHT = 0;

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

    if (nodoMac != "CC:50:E3:B6:29:48") setupSensors();

    Serial.println("*** Setup OK ***");
}

void loop() {
    APWebServerLoop();

    if (WiFi.status() == WL_CONNECTED) {
        if (activeMode) {
            // delay(500);
            if (nodoMac != "CC:50:E3:B6:29:48") {
                // delay(dht.getMinimumSamplingPeriod());

                luxValue = lightMeter.readLightLevel();
                lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
                // delay(200);

                if (lapTimer(dht.getMinimumSamplingPeriod(), &tiempoUltimaLecturaDHT)) {
                    tempValue = dht.getTemperature();
                    humidityValue = dht.getHumidity();
                }
                // delay(200);

                movementValue = digitalRead(movementSensorPort);

                // readActuador(ACTUADOR_LED,&turnOnOffLed);
            } else {
                // turnOnOffLed = 0;
                tempValue = temperatureRead();
                humidityValue = 0;
                luxValue = 0;
                movementValue = 0;
            }

            if (!isnan(tempValue) && !isnan(humidityValue)) {
                printTittle();

                jsonSensorData.clear();
                jsonSensorData.add("Luminocidad", (double)luxValue);
                jsonSensorData.add("Temperatura", (double)tempValue);
                jsonSensorData.add("Humedad", (double)humidityValue);
                jsonSensorData.add("Movimiento", (double)movementValue);
                if (uploadData(jsonSensorData)) {
                    // Serial.println("Lux OK");
                } else {
                    Serial.println("JSON NOT OK");
                }
            }

            // doActions();

            // delay(1000 * seccodsBetweenReads);

        } else {
            Serial.println("Esperando asignacion de aula...");
            delay(1000 * secondsToSleep);  // esperando configuracion...
            activeMode = lookupActiveMode();
        }
    }
}

void setupInet() {
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    ip = WiFi.localIP().toString();
    Serial.println(ip);
    Serial.println();
    Serial.print("MAC Address: ");
    nodoMac = WiFi.macAddress();
    Serial.println(nodoMac);
}

void setupSensors() {
    // Inicializacion sensor Luz
    Wire.begin(luxPortSDA, luxPortCSL);
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);

    // Inicializacion Sensor Humedad y Tempreratura
    pinMode(dhtPort, PULLUP);
    dht.setup(dhtPort);

    // Inicializacion Sensor Movimiento
    pinMode(movementSensorPort, PULLUP);

    // Inicializacion actuadores
    setUpLed();
}

int lookupActiveMode() {
    int salida;
    readData(AULA_ASIGNADA, &salida);
    return salida;
}

void printTittle() {
    Serial.print("\nLuminocidad\tHumedad\t\tTemperatura\tLedOnOff\tMovement\n");
    Serial.print(luxValue);
    Serial.print(" lux");
    Serial.print("\t");
    Serial.print(humidityValue);
    Serial.print(" %");
    Serial.print("\t \t");
    Serial.print(tempValue);
    Serial.print(" C°");
    Serial.print("\t");
    Serial.print(ledValue());
    Serial.print("\t\t");
    Serial.print(movementValue);
    Serial.print("\n");
}

// Delay no Bloqueante
bool lapTimer(ulong lapTime, ulong *lastLapTime) {
    ulong currentTime = millis();
    if (abs(currentTime - *lastLapTime) >= lapTime) {
        *lastLapTime = currentTime;
        return true;
    } else {
        return false;
    }
}