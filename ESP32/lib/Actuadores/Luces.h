#ifndef ACTUADOR_LUCES_H
#define ACTUADOR_LUCES_H

#include <Arduino.h>

void setUpLuces();
void toggleLuces();
int lucesValue();
void apagarLuces();
void encenderLuces();
void blinkLuces();

#endif