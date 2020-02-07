#ifndef FIREBASEMODULE_H
#define FIREBASEMODULE_H

#include <FirebaseESP32.h>

#define AULAS "aulas/"
#define NODOS "nodos/"
#define ACTUADORES "actuadores/"
#define COMANDOS_IR "comandosIR/"
#define LED "led/"
#define SENSORES "sensores/"
#define SENSOR_HUMEDAD "humedad/"
#define SENSOR_TEMPERATURA "temperatura/"
#define SENSOR_LUX "luminocidad/"
#define SENSOR_MOV "movimiento/"

#define COMANDO "comando/"
#define AULA_ASIGNADA "aulaAsignada/"
#define DB_FIRMWARE_VERSION "firmwareVersion/"
#define FIRMWARE "firmware/"
#define URL "url/"

#define TIEMPO_VACIA "tiempoVacia/"

void setUpFirebase();
String getAulaAsignada();
String getFirmwareVersion();
boolean updateFirmwareVersion(const char *fVersion);
String getFirmwareURL(String new_ver);
boolean readData(String path, int *value);
boolean readActuador(String path, int *value);
String createKeyCommand();
String getKeyCommand(String pathKey);
boolean uploadData(FirebaseJson &jsonData);
boolean uploadData(int value,String sensorId);
boolean uploadData(float value,String sensorId);
boolean uploadBlobData(String pathKey, uint8_t * data, int length);
void getBlobData(String pathKey, std::vector<uint8_t> * data);

void setStreamCallback(FirebaseData &fbDataStream,String dbPath,StreamEventCallback eventCallBack, StreamTimeoutCallback timeoutCallback);
void removeStreamCallback(FirebaseData &fbDataStream);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);

// void printFirebaseResult(FirebaseData &data);

#endif