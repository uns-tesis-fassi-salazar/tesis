#ifndef FIREBASEMODULE_H
#define FIREBASEMODULE_H

#include <FirebaseESP32.h>

void setupFirebase(String nodoMac);
boolean readData(String path, int *value);
boolean readActuador(String path, int *value);
boolean uploadData(FirebaseJson &jsonData);
boolean uploadData(int value,String sensorId);
boolean uploadData(float value,String sensorId);

void printFirebaseResult(FirebaseData &data);

#endif