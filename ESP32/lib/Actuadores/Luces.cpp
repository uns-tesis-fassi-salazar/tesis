#include <Luces.h>

int turnOnOffLuces = HIGH;

void setUpLuces() {
    pinMode(LUCES_PIN,OUTPUT);
    digitalWrite(LUCES_PIN,HIGH);
}

int lucesValue() {
    return turnOnOffLuces;
}

void apagarLuces() {
    if (hasCurrentFlow()) {
        toggleLuces();
    }
}

void encenderLuces() {
    if (!hasCurrentFlow()) {
        toggleLuces();
    }
}

void toggleLuces() {
    turnOnOffLuces = !turnOnOffLuces;
    digitalWrite(LUCES_PIN, turnOnOffLuces);
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