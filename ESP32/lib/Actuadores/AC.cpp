#include <AC.h>

ulong tiempoEntreComandos = 0;

struct commandoAC {
    decode_results command;
    bool hasCommand = false;
};

commandoAC comando;

void turnOffAC() {
    bool resultado = getCommand(&comando.command);
    resultado ? uploadLogs("getCommand: true") : uploadLogs("getCommand: false");

    // if (!comando.hasCommand) {
    //     if (getCommand(&comando.command)) {
    //         comando.hasCommand = true;
    //     }
    // }
    for (size_t i = 0; i < 3; i++) {
        sendCommand(&comando.command);
        delay(100);
    }
}

void updateCommand() {
    if (getCommand(&comando.command)) {
        comando.hasCommand = true;
    }
}