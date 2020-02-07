#ifndef IRMODULE_H
#define IRMODULE_H

#include <FirebaseModule.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

void setUpIRSender();
void setUpIRReceiver();
boolean recordingAndUploadCommand();
boolean sendCommand(decode_results *command);
// void uploadCommand(decode_results &command);
boolean getCommand(decode_results *command);

#endif