#ifndef FIREBASEMODULE_H
#define FIREBASEMODULE_H

#include <FirebaseESP32.h>
#include <Automata.h>
#include <utilidades.h>

#define AULAS "aulas/"
#define NODOS "nodos/"
#define ACTUADORES "actuadores/"
#define COMANDOS_IR "comandosIR/"
#define COMANDO_IR "comandoIR/"
#define LED "led/"
#define SENSORES "sensores/"
#define SENSOR_HUMEDAD "humedad/"
#define SENSOR_TEMPERATURA "temperatura/"
#define SENSOR_LUX "luminocidad/"
#define SENSOR_MOV "movimiento/"

#define COMANDO "comando"
#define AULA_ASIGNADA "aulaAsignada/"
#define DB_FIRMWARE_VERSION "firmwareVersion/"
#define FIRMWARE "firmware/"
#define URL "url/"
#define LOGS "logs/"

#define TIEMPO_VACIA "tiempoVacia/"

void setUpFirebase(const char *fVersion);
String getAulaAsignada();
bool getAulaConfig(struct aulaConfig *aulaCnf, String aulaKey);
String getFirmwareVersion();
bool updateFirmwareVersion(const char *fVersion);
String getFirmwareURL(String new_ver);
bool readData(String path, int *value);
bool readActuador(String path, int *value);
String createKeyCommand();
String getKeyCommand(String pathKey);
bool deleteKeyCommand(String commandKey);
bool uploadKeyCommand(String pathKey, String commandKey);
bool uploadData(FirebaseJson &jsonData);
bool uploadData(int value,String sensorId);
bool uploadData(float value,String sensorId);
bool getBlobData(String pathKey, std::vector<uint8_t> * data);
bool uploadBlobData(String pathKey, uint8_t * data, int length);
bool setCommandValue(String pathKey,String value);

bool uploadLogs(String log);

void setStreamCallback(FirebaseData &fbDataStream,String dbPath,StreamEventCallback eventCallBack, StreamTimeoutCallback timeoutCallback);
void removeStreamCallback(FirebaseData &fbDataStream);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);

// void printFirebaseResult(FirebaseData &data);

#endif