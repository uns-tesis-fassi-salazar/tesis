#ifndef ACTUADOR_LED_H
#define ACTUADOR_LED_H

#include <FirebaseModule.h>

void setUpLed();
void toggleLed();
int ledValue();

void streamCallbackLed(StreamData data);
void streamTimeoutCallbackLed(bool timeout);

#endif