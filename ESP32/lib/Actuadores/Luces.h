#ifndef ACTUADOR_LUCES_H
#define ACTUADOR_LUCES_H

#include <Arduino.h>
#include <Sensores.h>

#define LUCES_PIN GPIO_NUM_19

void setUpLuces();
void toggleLuces();
int lucesValue();
void apagarLuces();
void encenderLuces();
void blinkLuces();

#endif