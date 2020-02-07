#include <AC.h>

decode_results command;
boolean hasCommand = false;
ulong tiempoEntreComandos = 0;

void setUpAC() {
    hasCommand = getCommand(&command);
}

void turnOffAC() {
    if (hasCommand) {
        for (size_t i = 0; i < 3; i++) {
            if (lapTimer(100, &tiempoEntreComandos)) {
                sendCommand(&command);
            }
        }
    } else {
        hasCommand = getCommand(&command);
    }
}

void updateCommand() {
    hasCommand = getCommand(&command);
}