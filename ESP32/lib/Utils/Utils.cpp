#include <Utils.h>

// Delay no Bloqueante
bool lapTimer(ulong lapTime, ulong *lastLapTime) {
    ulong currentTime = millis();
    if (abs(currentTime - *lastLapTime) >= lapTime) {
        *lastLapTime = currentTime;
        return true;
    } else {
        return false;
    }
}

float truncToDec(float val, int cantDec) {
    double factor = pow(10, cantDec);
    return floorf(val * factor) / factor;
}

hw_timer_t *timerTimeout(int timeout, void (*resetCallback)(void)) {
    // timeout en ms
    hw_timer_t *timer = timerBegin(2, 80, true);       //timer 0, div 80
    timerAttachInterrupt(timer, resetCallback, true);  //attach callback
    timerAlarmWrite(timer, timeout * 1000, true);  //set time in us
    timerAlarmEnable(timer);                           //enable interrupt
    return timer;
}