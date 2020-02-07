#include <OTAUpdater.h>

HTTPClient http;
String newFirmwareVersion = "";

void checkFirmwareVersion(const char *fVersion)
{
	newFirmwareVersion = getFirmwareVersion();
	// Serial.print("Version obtenida: ");
	// Serial.println(newFirmwareVersion.c_str());
	// Serial.print("  Version actual: ");
	// Serial.println(fVersion);
	if ((strcmp(newFirmwareVersion.c_str(), fVersion) != 0) && (strcmp(newFirmwareVersion.c_str(), "") != 0)) {
		// Serial.println("Need to Update...");
		if (!updateFirmware(newFirmwareVersion)) {
			setUpStream();
		}
	}
}

boolean updateFirmware(String new_ver)
{
	String firmware_url = getFirmwareURL(new_ver);
	// Serial.print("firmware URL: ");
	// Serial.println(firmware_url);
	if (firmware_url != "") {
		setDownStream();
		// Start pulling down the firmware binary.
		http.begin(firmware_url);
		int httpCode = http.GET();
		if (httpCode <= 0)
		{
			Serial.printf("HTTP failed, error: %s\n",http.errorToString(httpCode).c_str());
			return false;
		}
		// Check that we have enough space for the new binary.
		int contentLen = http.getSize();
		Serial.printf("Content-Length: %d\n", contentLen);
		bool canBegin = Update.begin(contentLen);
		if (!canBegin)
		{
			Serial.println("Not enough space to begin OTA");
			return false;
		}
		// Write the HTTP stream to the Update library.
		WiFiClient *client = http.getStreamPtr();
		size_t written = Update.writeStream(*client);
		Serial.printf("OTA: %d/%d bytes written.\n", written, contentLen);
		if (written != contentLen)
		{
			Serial.println("Wrote partial binary. Giving up.");
			return false;
		}
		if (!Update.end())
		{
			Serial.println("Error from Update.end(): " + String(Update.getError()));
			return false;
		}
		if (Update.isFinished())
		{
			Serial.println("Update successfully completed. Rebooting.");
			// This line is specific to the ESP32 platform:
			ESP.restart();
		}
		else
		{
			Serial.println("Error from Update.isFinished(): " + String(Update.getError()));
			return false;
		}
	}
}