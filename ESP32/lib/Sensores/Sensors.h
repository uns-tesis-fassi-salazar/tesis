#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Utils.h>

#include <BH1750.h>
#include <DHT.h>
#include <Wire.h>

void sensorsSetup();
void loopSensors();
void printSensors();

#endif