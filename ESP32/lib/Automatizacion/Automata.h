#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <FirebaseModule.h>
#include <Sensores.h>
#include <Luces.h>
#include <IRModule.h>
#include <Utilidades.h>

struct aulaConfig {
    int timeOutAulaVacia;
    int intervaloLecturas;
    int horaIniAuto;
    int minIniAuto;
    int horaFinAuto;
    int minFinAuto;
    int zeroSensorHall = 1810;
    float umbralSensorHall = 2.5;
    bool hasConfig = false;
};

bool readAulaConfig();
void checkAulaState();
void resetAulaConfig();
bool hasAulaConfig();
int getTimeoutAulaVacia();

#endif