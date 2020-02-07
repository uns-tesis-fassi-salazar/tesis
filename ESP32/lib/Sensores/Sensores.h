#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <FirebaseModule.h>
#include <Utils.h>
#include <BH1750.h>
#include <DHT.h>
#include <Wire.h>

void setUpSensors();
void loopSensors();
void printSensors();

#endif