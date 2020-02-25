#ifndef IRMODULE_H
#define IRMODULE_H

#include <Led.h>
#include <FirebaseModule.h>
#include <DBStream.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <Utilidades.h>

#define IR_BUTTON_PIN GPIO_NUM_15

void setUpIRSender();
void setUpIRReceiver();
bool recordingAndUploadCommand();
bool sendCommand(decode_results *command);
bool getCommand(decode_results *command);
void turnOffAC();

void setUpRecordCommandButton();
void listenRecordCommandButton();

#endif