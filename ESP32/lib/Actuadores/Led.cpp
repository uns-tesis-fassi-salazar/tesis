#include <Led.h>

#define LED_PIN GPIO_NUM_4

int ledValue = 0;

void setUpLed() {
    pinMode(LED_PIN,OUTPUT);
}

int getLedValue() {
    return ledValue;
}

void toggleLed() {
    ledValue = !ledValue;
    ledValue ? digitalWrite(LED_PIN,HIGH) : digitalWrite(LED_PIN,LOW);
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