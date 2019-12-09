#ifndef FIREBASEMODULE_H
#define FIREBASEMODULE_H

#include <FirebaseESP32.h>

void setupFirebase(String nodoMac);
boolean readData(String path, int *value);
boolean readActuador(String path, int *value);
boolean uploadData(FirebaseJson &jsonData);
boolean uploadData(int value,String sensorId);
boolean uploadData(float value,String sensorId);
boolean uploadBlobData(String pathKey, uint8_t * data, int length);


void setStreamToActuador(FirebaseData &fbDataStream,String actuadorId,StreamEventCallback eventCallBack, StreamTimeoutCallback timeoutCallback);

void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);

void printFirebaseResult(FirebaseData &data);

#endif