#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <FirebaseModule.h>
#include <Automata.h>
#include <BH1750.h>
#include <DHT.h>
#include <ACS712.h>
#include <Wire.h>
#include <Utilidades.h>

#define MOVEMENT_PIN GPIO_NUM_5
#define LUX_SDA_PIN GPIO_NUM_21
#define LUX_CSL_PIN GPIO_NUM_22
#define DHT_PIN GPIO_NUM_32

void setUpSensors();
void loopSensors();
void printSensors();
bool emptyRoomState();
bool hasCurrentFlow();
void setMovementTimeout(int timeout);
void setSensorHall(float umbralHall, int zeroSensorHall);

#endif