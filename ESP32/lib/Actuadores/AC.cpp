#include "AC.h"
#include "FirebaseModule.h"

#define ACTUADOR_AC "/Actuadores/Aire"

int ACPort = GPIO_NUM_27;
int ACState = 0;
FirebaseData firebaseACDataStream;

void setUpAC() {
    pinMode(ACPort,OUTPUT);
    // En este punto ya debería estar inicializada Firebase
    setStreamToActuador(firebaseACDataStream,ACTUADOR_AC,streamCallbackAC,(StreamTimeoutCallback)__null);
}

void turnOffAC() {
    digitalWrite(ACPort,LOW);
}

void toggleAC() {
    ACState ? digitalWrite(ACPort,LOW) : digitalWrite(ACPort,HIGH);
}

void streamCallbackAC(StreamData data)
{
    ACState = data.intData();
    // turnOffAC();
    toggleAC();
}

void streamTimeoutCallbackAC(bool timeout)
{
    if(timeout) {
        Serial.println("AC Stream timeout, resume...");
    }
}