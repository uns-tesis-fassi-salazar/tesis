#include <Automata.h>

aulaConfig aulaCnf;
extern String aulaKey;

bool readAulaConfig() {
    if (aulaKey != "") {
        if (!getAulaConfig(&aulaCnf,aulaKey)) {
            uploadLogs("No se pudieron leer las configuraciones del aula.");
            return false;
        }
        if (aulaCnf.horaIniAuto > aulaCnf.horaFinAuto) {
            aulaCnf.horaFinAuto += 23;
            aulaCnf.minFinAuto += 59;
        } else {
            if ((aulaCnf.horaIniAuto == aulaCnf.horaFinAuto) && (aulaCnf.minIniAuto > aulaCnf.minFinAuto)) {
                aulaCnf.horaFinAuto += 23;
                aulaCnf.minFinAuto += 59;
            }
        }
        setTimerTimeout(aulaCnf.timeOutAulaVacia);
        return true;
    }
    return false;
}

void checkAulaState() {
    if (!aulaCnf.hasConfig) {
        readAulaConfig();
    }
    if (emptyRoomState()) {
        struct tm timeinfo;
        if(getLocalTime(&timeinfo)) {
            if ((timeinfo.tm_hour > aulaCnf.horaIniAuto) && (timeinfo.tm_hour < aulaCnf.horaFinAuto)) {
                // if (detectCurrentFlow()) {
                //     uploadLogs("Automata: apagando luces y AC...");
                //     apagarLuces();
                //     delay(2000);
                //     turnOffAC();
                // }
            } else {
                if (timeinfo.tm_hour == aulaCnf.horaIniAuto) {
                    if (timeinfo.tm_min >= aulaCnf.minIniAuto) {
                        // if (detectCurrentFlow()) {
                        //     uploadLogs("Automata: apagando luces y AC...");
                        //     apagarLuces();
                        //     delay(2000);
                        //     turnOffAC();
                        // }
                    }
                } else {
                    if (timeinfo.tm_hour == aulaCnf.horaFinAuto) {
                        if (timeinfo.tm_min <= aulaCnf.minFinAuto) {
                            // if (detectCurrentFlow()) {
                            //     uploadLogs("Automata: apagando luces y AC...");
                            //     apagarLuces();
                            //     delay(2000);
                            //     turnOffAC();
                            // }
                        }
                    }
                }
            }
        }
    }
}

void resetAulaConfig() {
    aulaCnf.hasConfig = false;
}

bool hasAulaConfig() {
    return aulaCnf.hasConfig;
}

int getTimeoutAulaVacia() {
    return aulaCnf.timeOutAulaVacia;
}