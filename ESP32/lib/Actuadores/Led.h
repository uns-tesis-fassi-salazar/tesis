#ifndef ACTUADOR_LED_H
#define ACTUADOR_LED_H

#include <Arduino.h>
#include <utilidades.h>

#define LED_PIN GPIO_NUM_4

void setUpLed();
void toggleLed();
int getLedValue();
void apagarLed();
void encenderLed();
void blinkLed();
void startBlink();
void endBlink();

#endif