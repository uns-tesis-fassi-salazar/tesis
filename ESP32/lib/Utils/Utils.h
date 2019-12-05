#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

bool lapTimer(ulong lapTime, ulong *lastLapTime);
float truncToDec(float val, int cantDec);
hw_timer_t *timerTimeout(int timeout, void (*resetCallback)(void));

#endif