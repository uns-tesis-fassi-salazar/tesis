#include "Led.h"
#include "FirebaseModule.h"

#define ACTUADOR_LED "/Actuadores/Led"
#define LED_PIN GPIO_NUM_4

int turnOnOffLed = 0;
FirebaseData firebaseLedDataStream;

void setUpLed() {
    pinMode(LED_PIN,OUTPUT);
    // En este punto ya debería estar inicializada Firebase
    setStreamToActuador(firebaseLedDataStream,ACTUADOR_LED,streamCallbackLed,(StreamTimeoutCallback)__null);
}

int ledValue() {
    return turnOnOffLed;
}

void toggleLed() {
    turnOnOffLed ? digitalWrite(LED_PIN,HIGH) : digitalWrite(LED_PIN,LOW);
}

void blinkLed() {
    for (size_t i = 0; i < 3; i++)
    {
        digitalWrite(LED_PIN,HIGH);
        delay(500);
        digitalWrite(LED_PIN,LOW);
        delay(500);
    }
    
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