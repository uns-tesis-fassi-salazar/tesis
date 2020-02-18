#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <FirebaseModule.h>
#include <Sensores.h>
#include <Luces.h>
#include <AC.h>
#include <utilidades.h>

// struct aulaConfig {
//     int timeOutAulaVacia;
//     int intervaloLecturas;
//     int horaIniAuto;
//     int minIniAuto;
//     int horaFinAuto;
//     int minFinAuto;
//     bool hasConfig = false;
// };

bool readAulaConfig();
void checkAulaState();
void resetAulaConfig();
bool hasAulaConfig();
int getTimeoutAulaVacia();

#endif