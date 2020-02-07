#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Update.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Actuadores.h>
#include <FirebaseModule.h>

boolean updateFirmware(String new_ver);
void checkFirmwareVersion(const char *fVersion);

#endif