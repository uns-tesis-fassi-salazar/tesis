#include <Utilidades.h>

// Delay no Bloqueante
bool lapTimer(ulong lapTime, ulong *lastLapTime) {
    ulong currentTime = millis();
    if (abs(currentTime - *lastLapTime) >= lapTime)
    {
        *lastLapTime = currentTime;
        return true;
    }
    else
    {
        return false;
    }
}

float truncToDec(float val, int cantDec) {
    double factor = pow(10, cantDec);
    return floorf(val * factor) / factor;
}

hw_timer_t *timerTimeout(int timeout, void (*resetCallback)(void)) {
    // timeout en ms
    hw_timer_t *timer = timerBegin(2, 80, true);      //timer 0, div 80
    timerAttachInterrupt(timer, resetCallback, true); //attach callback
    timerAlarmWrite(timer, timeout * 1000, true);     //set time in us
    timerAlarmEnable(timer);                          //enable interrupt
    return timer;
}

void diffBetweenTimePeriod(struct CustomTime start, struct CustomTime stop, struct CustomTime *diff) {
    if (stop.seconds > start.seconds)
    {
        --start.minutes;
        start.seconds += 60;
    }
    diff->seconds = start.seconds - stop.seconds;
    if (stop.minutes > start.minutes)
    {
        --start.hours;
        start.minutes += 60;
    }
    diff->minutes = start.minutes - stop.minutes;
    diff->hours = start.hours - stop.hours;
}

String tiempoToString() {
    struct tm timeinfo;
    char toRet[50];
    if (!getLocalTime(&timeinfo))
    {
        uploadLogs("No se pudo obtener el tiempo");
        return "";
    }
    sprintf(toRet, "%s %i de %s del %i %i:%i:%i", dias_semana[timeinfo.tm_wday], timeinfo.tm_mday, meses[timeinfo.tm_mon], timeinfo.tm_year + 1900, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    return String(toRet);
}

String tiempoCortoToString() {
    struct tm timeinfo;
    char toRet[26];
    if (!getLocalTime(&timeinfo))
    {
        uploadLogs("No se pudo obtener el tiempo");
        return "";
    }
    sprintf(toRet,"%i-%i-%i %i:%i:%i",timeinfo.tm_mday,timeinfo.tm_mon,timeinfo.tm_year + 1900,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
    strftime(toRet, 26, "%d-%m-%Y %H:%M:%S", &timeinfo);
    return String(toRet);
}

void printLocalTimeSpanish() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        uploadLogs("No se pudo obtener el tiempo");
        return;
    }
    Serial.printf("%s, %s %i %i %i:%i:%i", dias_semana[timeinfo.tm_wday], meses[timeinfo.tm_mon], timeinfo.tm_mday, timeinfo.tm_year + 1900, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.println();
}

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        uploadLogs("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int getUIntFromNVM(const char *key) {
    Preferences preferences;
    // Open Preferences with my-app namespace. Each application module, library, etc
    // has to use a namespace name to prevent key name collisions. We will open storage in
    // RW-mode (second parameter has to be false).
    // Note: Namespace name is limited to 15 chars.
    preferences.begin("as-app", true);

    // Get the value, if the key does not exist, return a default value of 0
    // Note: Key name is limited to 15 chars.
    unsigned int value = preferences.getUInt(key, 0);

    // Print the value to Serial Monitor
    // Serial.printf("Value: %u\n", value);

    // Close the Preferences
    preferences.end();

    return value;
}

void putUIntInNVM(const char *key, uint32_t value) {
    Preferences preferences;
    preferences.begin("as-app", false);
    preferences.putUInt(key, value);
    preferences.end();
}