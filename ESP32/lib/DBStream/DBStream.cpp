#include <DBStream.h>

FirebaseData fbDataStream;
String FV;
extern String aulaKey;
extern bool blink;
int accion = 0;
bool flagEmitirIR = false;
bool flagGrabarIR = false;
bool flagActualizarIR = false;
bool flagApagarLuces = false;
bool flagEncenderLuces = false;
bool flagActualizarFirmware = false;
bool flagActualizarAula = false;
bool flagRecargarAulaConfig = false;
bool flagRecargarTimer = false;
bool flagToggleLed = false;
bool flagBlinkLed = false;

void setUpStream(const char *fVersion) {
    FV = String(fVersion);

    // En este punto ya debería estar inicializada Firebase
    setStreamCallback(fbDataStream,(String)NODOS + WiFi.macAddress() + "/" + COMANDO,streamCallback,(StreamTimeoutCallback)__null);
}

void setDownStream() {
    removeStreamCallback(fbDataStream);
}

bool pauseStream() {
    return pauseStreamCallback(fbDataStream);
}

bool restoreStream() {
    return restoreStreamCallback(fbDataStream,(String)NODOS + WiFi.macAddress() + "/" + COMANDO);
}

void streamCallback(StreamData data)
{
    accion = data.intData();
    switch (accion)
    {
        case EMITIR_IR:
            flagEmitirIR = true;
            break;

        case GRABAR_IR:
            flagGrabarIR = true;
            break;

        case ACTUALIZAR_IR:
            flagActualizarIR = true;
            break;

        case ENCENDER_LUCES:
            flagEncenderLuces = true;
            break;

        case APAGAR_LUCES:
            flagApagarLuces = true;
            break;

        case ACTUALIZAR_FIRMWARE:
            flagActualizarFirmware = true;
            break;

        case ACTUALIZAR_AULA:
            flagActualizarAula = true;
            break;

        case RECARGAR_AULA_CONFIG:
            flagRecargarAulaConfig = true;
            break;

        case RECARGAR_TIMER:
            flagRecargarTimer = true;
            break;

        case TOGGLE_LED:
            flagToggleLed = true;
            break;

        case BLINK_LED:
            flagBlinkLed = true;
            break;
    
        default:
            Serial.println("Comando no encontrado");
            break;
    }
}

void streamTimeoutCallback(bool timeout)
{
    if(timeout) {
        uploadLogs("Acciones Stream timeout, resume...");
    }
}

void loopStream() {
    if (flagEmitirIR) {
        uploadLogs("EMITIR_IR");
        turnOffAC();
        flagEmitirIR = false;
    }
    if (flagGrabarIR) {
        uploadLogs("GRABAR_IR");
        recordingAndUploadCommand();
        flagGrabarIR = false;
    }
    if (flagActualizarIR) {
        uploadLogs("ACTUALIZAR_IR: Esta funcion está en desuso.");
        flagActualizarIR = false;
    }
    if (flagEncenderLuces) {
        uploadLogs("ENCENDER_LUCES");
        if (!hasCurrentFlow()) {
            toggleLuces();
        }
        flagEncenderLuces = false;
    }
    if (flagApagarLuces) {
        uploadLogs("APAGAR_LUCES");
        if (hasCurrentFlow()) {
            toggleLuces();
        }
        flagApagarLuces = false;
    }
    if (flagActualizarFirmware) {
        uploadLogs("ACTUALIZAR_FIRMWARE");
        checkFirmwareVersion(FV.c_str());
        flagActualizarFirmware = false;
    }
    if (flagActualizarAula) {
        uploadLogs("ACTUALIZAR_AULA");
        resetAulaConfig();
        checkAulaState();
        flagActualizarAula = false;
    }
    if (flagRecargarAulaConfig) {
        uploadLogs("RECARGAR_AULA_CONFIG");
        resetAulaConfig();
        flagRecargarAulaConfig = false;
    }
    if (flagRecargarTimer) {
        uploadLogs("RECARGAR_TIMER");
        setMovementTimeout(60);
        flagRecargarTimer = false;
    }
    if (flagToggleLed) {
        uploadLogs("TOGGLE_LED");
        toggleLed();
        flagToggleLed = false;
    }
    if (flagBlinkLed) {
        uploadLogs("BLINK_LED");
        // blinkLed();
        blink ? blink = false : blink = true;
        flagBlinkLed = false;
    }
}