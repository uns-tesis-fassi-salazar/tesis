#ifndef ACTUADOR_AC_H
#define ACTUADOR_AC_H

#include <FirebaseModule.h>

void setUpAC();
void turnOffAC();
void toggleAC();
int ACValue();

void streamCallbackAC(StreamData data);
void streamTimeoutCallbackAC(bool timeout);

#endif