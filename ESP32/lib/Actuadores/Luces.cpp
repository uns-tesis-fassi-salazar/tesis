#include <Luces.h>

#define LUCES_PIN GPIO_NUM_19

int turnOnOffLuces = 0;

void setUpLuces() {
    pinMode(LUCES_PIN,OUTPUT);
}

int lucesValue() {
    return turnOnOffLuces;
}

void apagarLuces() {
    digitalWrite(LUCES_PIN,LOW);
}

void encenderLuces() {
    digitalWrite(LUCES_PIN,HIGH);
}

void toggleLuces() {
    turnOnOffLuces ? digitalWrite(LUCES_PIN,HIGH) : digitalWrite(LUCES_PIN,LOW);
}

void blinkLuces() {
    for (size_t i = 0; i < 5; i++)
    {
        digitalWrite(LUCES_PIN,HIGH);
        delay(5000);
        digitalWrite(LUCES_PIN,LOW);
        delay(5000);
    }
}