#include <AC.h>

ulong tiempoEntreComandos = 0;

decode_results ACcommand;

void turnOffAC() {
    bool resultado = getCommand(&ACcommand);
    resultado ? uploadLogs("getCommand: true") : uploadLogs("getCommand: false");

    sendCommand(&ACcommand);
    delay(100);
}