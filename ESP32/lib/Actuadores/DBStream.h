#ifndef DBSTREAM_H
#define DBSTREAM_H

#define RESET 0
#define EMITIR_IR 1
#define GRABAR_IR 2
#define ACTUALIZAR_IR 3
#define ENCENDER_LUCES 4
#define APAGAR_LUCES 5
#define ACTUALIZAR_FIRMWARE 6
#define ACTUALIZAR_AULA 7
#define RECARGAR_AULA_CONFIG 8
#define RECARGAR_TIMER 9

#define TOGGLE_LED 100
#define BLINK_LED 101

#include <AC.h>
#include <Led.h>
#include <Luces.h>
#include <IRModule.h>
// #include <OTAUpdater.h>
#include <FirebaseModule.h>
#include <Automata.h>
#include <Utils.h>

void loopStream();
void setUpStream(const char *fVersion);
void setDownStream();
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);

#endif