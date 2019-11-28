#include "Led.h"
#include "FirebaseModule.h"

#define ACTUADOR_LED "/Actuadores/Led"

int ledPort = GPIO_NUM_32;
int turnOnOffLed = 0;
FirebaseData firebaseLedDataStream;

void setUpLed() {
    pinMode(ledPort,OUTPUT);
    // En este punto ya debería estar inicializada Firebase
    setStreamToActuador(firebaseLedDataStream,ACTUADOR_LED,streamCallbackLed,(StreamTimeoutCallback)__null);
}

int ledValue() {
    return turnOnOffLed;
}

void toggleLed() {
    turnOnOffLed ? digitalWrite(ledPort,HIGH) : digitalWrite(ledPort,LOW);
}

void streamCallbackLed(StreamData data)
{
    turnOnOffLed = data.intData();
    toggleLed();
}

void streamTimeoutCallbackLed(bool timeout)
{
    if(timeout) {
        Serial.println("Led Stream timeout, resume...");
    }
}