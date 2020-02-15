#include <AC.h>

ulong tiempoEntreComandos = 0;

decode_results ACcommand;

void turnOffAC() {
    bool resultado = getCommand(&ACcommand);
    resultado ? uploadLogs("getCommand: true") : uploadLogs("getCommand: false");

    for (size_t i = 0; i < 3; i++) {
        // sendCommand(&command);
        delay(100);
    }
}