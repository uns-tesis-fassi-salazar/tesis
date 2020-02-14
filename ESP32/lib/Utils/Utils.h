#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <FirebaseModule.h>
#include <Utils.h>

const char dias_semana[7][10] = {"Domingo","Lunes","Martes","Miercoles","Jueves","Viernes","Sabado"};
const char meses[12][11] = {"Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre"};

struct CustomTime {
    int seconds;
    int minutes;
    int hours;
};

void differenceBetweenTimePeriod(struct TIME t1, struct TIME t2, struct TIME *diff);

bool lapTimer(ulong lapTime, ulong *lastLapTime);
float truncToDec(float val, int cantDec);
hw_timer_t *timerTimeout(int timeout, void (*resetCallback)(void));
void printLocalTime();
void printLocalTimeSpanish();
String tiempoToString();
String tiempoCortoToString();

#endif