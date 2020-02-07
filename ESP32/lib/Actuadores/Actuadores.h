#ifndef ACTUADORES_H
#define ACTUADORES_H

#define EMITIR_IR 1
#define GRABAR_IR 2
#define ACTUALIZAR_IR 3
#define APAGAR_LUCES 4
#define ENCENDER_LUCES 5
#define ACTUALIZAR_FIRMWARE 6
#define TOGGLE_LED 7
#define BLINK_LED 8

#include <AC.h>
#include <Led.h>
#include <Luces.h>
#include <IRModule.h>
#include <OTAUpdater.h>
#include <FirebaseModule.h>

void loopActuadores();
void setUpActuadores(const char *fVersion);
void setUpStream();
void setDownStream();
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);

#endif