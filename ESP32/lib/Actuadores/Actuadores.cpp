
#include <Actuadores.h>

FirebaseData fbDataStream;
String FV;
int accion = 0;
bool flagEmitirIR = false;
bool flagGrabarIR = false;
bool flagActualizarIR = false;
bool flagApagarLuces = false;
bool flagEncenderLuces = false;
bool flagActualizarFirmware = false;
bool flagToggleLed = false;
bool flagBlinkLed = false;

void setUpActuadores(const char *fVersion) {
    FV = String(fVersion);

    // Inicializacion actuadores
    setUpIRSender();
    setUpIRReceiver();
    setUpLed();
    setUpAC();
    setUpLuces();

    setUpStream();
}

void setUpStream() {
    // En este punto ya debería estar inicializada Firebase
    setStreamCallback(fbDataStream,(String)NODOS + WiFi.macAddress() + "/" + COMANDO,streamCallback,(StreamTimeoutCallback)__null);
}

void setDownStream() {
    removeStreamCallback(fbDataStream);
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

        case APAGAR_LUCES:
            flagApagarLuces = true;
            break;

        case ENCENDER_LUCES:
            flagEncenderLuces = true;
            break;

        case ACTUALIZAR_FIRMWARE:
            flagActualizarFirmware = true;
            break;

        case TOGGLE_LED:
            flagToggleLed = true;
            break;

        case BLINK_LED:
            flagBlinkLed = true;
            break;
    
        default:
            // Serial.println("Accion no encontrada...");
            break;
    }
}

void streamTimeoutCallback(bool timeout)
{
    if(timeout) {
        Serial.println("Acciones Stream timeout, resume...");
    }
}

void loopActuadores() {
    if (flagEmitirIR) {
        Serial.println("EMITIR_IR");
        turnOffAC();
        flagEmitirIR = false;
    }
    if (flagGrabarIR) {
        Serial.println("GRABAR_IR");
        recordingAndUploadCommand();
        flagGrabarIR = false;
    }
    if (flagActualizarIR) {
        Serial.println("ACTUALIZAR_IR");
        updateCommand();
        flagActualizarIR = false;
    }
    if (flagApagarLuces) {
        Serial.println("APAGAR_LUCES");
        apagarLuces();
        flagApagarLuces = false;
    }
    if (flagEncenderLuces) {
        Serial.println("ENCENDER_LUCES");
        encenderLuces();
        flagEncenderLuces = false;
    }
    if (flagActualizarFirmware) {
        Serial.println("ACTUALIZAR_FIRMWARE");
        checkFirmwareVersion(FV.c_str());
        flagActualizarFirmware = false;
    }
    if (flagToggleLed) {
        Serial.println("TOGGLE_LED");
        toggleLed();
        flagToggleLed = false;
    }
    if (flagBlinkLed) {
        Serial.println("BLINK_LED");
        blinkLed();
        flagBlinkLed = false;
    }
}