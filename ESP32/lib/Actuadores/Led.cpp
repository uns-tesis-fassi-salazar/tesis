#include <Led.h>

extern bool blink;
int ledValue = LOW;
ulong ledBlinkTime = 1000;
ulong ledLastBlink = 0;

void setUpLed() {
    pinMode(LED_PIN,OUTPUT);
}

int getLedValue() {
    return ledValue;
}

void encenderLed() {
    ledValue = HIGH;
    digitalWrite(LED_PIN, ledValue);
}

void apagarLed() {
    ledValue = LOW;
    digitalWrite(LED_PIN, ledValue);
}

void toggleLed() {
    ledValue = !ledValue;
    ledValue ? digitalWrite(LED_PIN,HIGH) : digitalWrite(LED_PIN,LOW);
}

void startBlink() {
    blink = true;
}

void endBlink() {
    blink = false;
}

void blinkLed() {
    // ledLastBlink = millis();
    if (lapTimer(ledBlinkTime,&ledLastBlink)) {
        !ledValue ? ledValue = HIGH : ledValue = LOW;
        digitalWrite(LED_PIN, ledValue);
    }
}