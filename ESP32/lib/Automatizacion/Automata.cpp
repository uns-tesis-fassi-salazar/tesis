#include <Automata.h>

void apagarAutomatico();

aulaConfig aulaCnf;
bool flagExterior = false;
extern String aulaKey;
extern int tiempoEntreLecturas;

bool readAulaConfig() {
    if (aulaKey != "") {
        if (!getAulaConfig(&aulaCnf,aulaKey)) {
            uploadLogs("No se pudieron leer las configuraciones del aula.");
            return false;
        }
        if (aulaCnf.horaIniAuto > aulaCnf.horaFinAuto) {
            flagExterior = true;
        } else {
            if ((aulaCnf.horaIniAuto == aulaCnf.horaFinAuto) && (aulaCnf.minIniAuto > aulaCnf.minFinAuto)) {
                flagExterior = true;
            }
        }
        setMovementTimeout(aulaCnf.timeOutAulaVacia);
        setSensorHall(aulaCnf.umbralSensorHall, aulaCnf.zeroSensorHall);
        tiempoEntreLecturas = aulaCnf.intervaloLecturas;
        return true;
    }
    return false;
}

void checkAulaState() {
    if (!aulaCnf.hasConfig) {
        readAulaConfig();
    } else if (emptyRoomState()) {
        struct tm timeinfo;
        if(getLocalTime(&timeinfo)) {
            if (flagExterior) {
                if ((timeinfo.tm_hour > aulaCnf.horaIniAuto) || (timeinfo.tm_hour < aulaCnf.horaFinAuto)) {
                    apagarAutomatico();
                } else {
                    if (timeinfo.tm_hour == aulaCnf.horaIniAuto) {
                        if (timeinfo.tm_min >= aulaCnf.minIniAuto) {
                            apagarAutomatico();
                        }
                    } else {
                        if (timeinfo.tm_hour == aulaCnf.horaFinAuto) {
                           if (timeinfo.tm_min <= aulaCnf.minFinAuto) {
                               apagarAutomatico();
                           }
                        }
                    }
                }
            } else {
                if ((timeinfo.tm_hour > aulaCnf.horaIniAuto) && (timeinfo.tm_hour < aulaCnf.horaFinAuto)) {
                    apagarAutomatico();
                } else {
                    if (timeinfo.tm_hour == aulaCnf.horaIniAuto) {
                        if (timeinfo.tm_min >= aulaCnf.minIniAuto) {
                            apagarAutomatico();
                        }
                    } else {
                        if (timeinfo.tm_hour == aulaCnf.horaFinAuto) {
                           if (timeinfo.tm_min <= aulaCnf.minFinAuto) {
                               apagarAutomatico();
                           }
                        }
                    }
                }
            }
        }
    }
}

void resetAulaConfig() {
    aulaCnf.hasConfig = false;
    flagExterior = false;
}

bool hasAulaConfig() {
    return aulaCnf.hasConfig;
}

int getTimeoutAulaVacia() {
    return aulaCnf.timeOutAulaVacia;
}

void apagarAutomatico() {
    uploadLogs("Automata: apagando luces y AC...");
    if (hasCurrentFlow()) {
        toggleLuces();
    }
    delay(2000);
    turnOffAC();
}