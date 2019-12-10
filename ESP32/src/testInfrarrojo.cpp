/*
 * IRremoteESP8266: SmartIRRepeater.ino - Record and playback IR codes.
 * Copyright 2019 David Conran (crankyoldgit)
 *
 * This program will try to capture incoming IR messages and tries to
 * intelligently replay them back.
 * It uses the advanced detection features of the library, and the custom
 * sending routines. Thus it will try to use the correct frequencies,
 * duty cycles, and repeats as it thinks is required.
 * Anything it doesn't understand, it will try to replay back as best it can,
 * but at 38kHz.
 * Note:
 *   That might NOT be the frequency of the incoming message, so some not
 *   recogised messages that are replayed may not work. The frequency & duty
 *   cycle of unknown incoming messages is lost at the point of the Hardware IR
 *   demodulator. The ESP can't see it.
 *
 *                               W A R N I N G
 *   This code is just for educational/example use only. No help will be given
 *   to you to make it do something else, or to make it work with some
 *   weird device or circuit, or to make it more usable or practical.
 *   If it works for you. Great. If not, Congratulations on changing/fixing it.
 *
 * An IR detector/demodulator must be connected to the input, kRecvPin.
 * An IR LED circuit must be connected to the output, kIrLedPin.
 *
 * Example circuit diagrams (both are needed):
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Some digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 *
 * Changes:
 *   Version 1.0: June, 2019
 *     - Initial version.
 */

#include <Arduino.h>
#include <FirebaseModule.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>

// ==================== start of TUNEABLE PARAMETERS ====================

// The GPIO an IR detector/demodulator is connected to. Recommended: 14 (D5)
const uint16_t kRecvPin = 33;

// GPIO to use to control the IR LED circuit. Recommended: 4 (D2).
const uint16_t kIrLedPin = 25;

// The Serial connection baud rate.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 9600;

// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
const uint16_t kCaptureBufferSize = 1024;  // 1024 == ~511 bits

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all UNKNOWN messages will be sent at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

// ==================== end of TUNEABLE PARAMETERS ====================

// The IR transmitter.
IRsend irsend(kIrLedPin);
// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
// Somewhere to store the captured message.
decode_results results;

void setup() {
    irrecv.enableIRIn();  // Start up the IR receiver.
    irsend.begin();       // Start up the IR sender.

    // Serial.begin(kBaudRate, SERIAL_8N1);
    Serial.begin(kBaudRate);
    while (!Serial)  // Wait for the serial connection to be establised.
        delay(50);
    Serial.println();

    Serial.print(
        "SmartIRRepeater is now running and waiting for IR input "
        "on Pin ");
    Serial.println(kRecvPin);
    Serial.print("and will retransmit it on Pin ");
    Serial.println(kIrLedPin);

    WiFi.begin("DIEGO", "MontyPython");
    while (!WiFi.isConnected()) {
        /* code */
    }

    setupFirebase(WiFi.macAddress());
}

void uploadResult(decode_results *results);
std::vector<uint8_t> uploadBlob(uint8_t *data, uint16_t length);
FirebaseData firebaseDataData;
String path = "/Test";

void uploadResult(decode_results &results) {
    Serial.println("");
    Serial.println("Firebase SET result - Our Test");
    // String blobPath = path + "/Binary/Blob/";

    // uint64_t value;
    // uint32_t address;
    // uint32_t command;
    
    // uint8_t state[kStateSizeMax];

    // ***** VALUE *****
    uint8_t byteArrayVal[8];
    byteArrayVal[0] = results.value >> 56;
    byteArrayVal[1] = results.value >> 48;
    byteArrayVal[2] = results.value >> 40;
    byteArrayVal[3] = results.value >> 32;
    byteArrayVal[4] = results.value >> 24;
    byteArrayVal[5] = results.value >> 16;
    byteArrayVal[6] = results.value >> 8;
    byteArrayVal[7] = results.value & 0xFF;

    Serial.printf("Value: %iu", (uint32_t)results.value >> 32);
    Serial.printf("%iu\n", (uint32_t)(results.value) && 0xFFFFFFFF);
    // serialPrintUint64(results.value,8);
    if (uploadBlobData("value", byteArrayVal, 8)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobVal;
    getBlobData("value",&blobVal);
    uint64_t auxVal;
    auxVal = blobVal[0] << 56;
    auxVal |= (blobVal[1] << 48);
    auxVal |= (blobVal[2] << 40);
    auxVal |= (blobVal[3] << 32);
    auxVal |= (blobVal[4] << 24);
    auxVal |= (blobVal[5] << 16);
    auxVal |= (blobVal[6] << 8);
    auxVal |= blobVal[7];
    Serial.printf("Value from firebase: %iu", (uint32_t)auxVal >> 32);
    Serial.printf("%iu\n", (uint32_t)(auxVal) && 0xFFFFFFFF);

    // ***** COMMAND *****
    uint8_t byteArrayComm[4];
    byteArrayComm[0] = results.command >> 24;
    byteArrayComm[1] = results.command >> 16;
    byteArrayComm[2] = results.command >> 8;
    byteArrayComm[3] = results.command & 0xFF;

    Serial.printf("Command: %hu\n", results.command);
    if (uploadBlobData("command", byteArrayComm, 4)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobComm;
    getBlobData("command",&blobComm);
    uint32_t auxComm;
    auxComm = blobComm[0] << 24;
    auxComm |= (blobComm[1] << 16);
    auxComm |= (blobComm[2] << 8);
    auxComm |= blobComm[3];
    Serial.printf("Command from firebase: %hu\n", auxComm);

    // ***** ADDRESS *****
    uint8_t byteArrayAddr[4];
    byteArrayAddr[0] = results.address >> 24;
    byteArrayAddr[1] = results.address >> 16;
    byteArrayAddr[2] = results.address >> 8;
    byteArrayAddr[3] = results.address & 0xFF;

    Serial.printf("Address: %hu\n", results.address);
    if (uploadBlobData("address", byteArrayAddr, 4)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobAddr;
    getBlobData("address",&blobAddr);
    uint32_t auxAddr;
    auxAddr = blobAddr[0] << 24;
    auxAddr |= (blobAddr[1] << 16);
    auxAddr |= (blobAddr[2] << 8);
    auxAddr |= blobAddr[3];
    Serial.printf("Address from firebase: %hu\n", auxAddr);

    // ***** DECODE_TYPE *****
    uint8_t byteD = results.decode_type;
    Serial.printf("Decode_type: %hu\n", results.decode_type);
    if (uploadBlobData("decode_type", &byteD, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobD;
    getBlobData("decode_type",&blobD);
    uint8_t auxD;
    auxD = blobD[0];
    Serial.printf("Decode_type from firebase: %hu\n", auxD);

    // ***** RAWLEN *****
    uint8_t byteArrayRL[2];
    byteArrayRL[0] = results.rawlen >> 8;
    byteArrayRL[1] = results.rawlen & 0xFF;

    Serial.printf("Rawlen: %hu\n", results.rawlen);
    if (uploadBlobData("rawlen", byteArrayRL, 2)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobRL;
    getBlobData("rawlen",&blobRL);
    uint16_t auxRL;
    auxRL = blobRL[0] << 8;
    auxRL |= blobRL[1];
    Serial.printf("Rawlen from firebase: %hu\n", auxRL);

    // ***** BITS *****
    uint8_t byteArrayB[2];
    byteArrayB[0] = results.bits >> 8;
    byteArrayB[1] = results.bits & 0xFF;

    Serial.printf("Bits: %hu\n", results.bits);
    if (uploadBlobData("bits", byteArrayB, 2)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobB;
    getBlobData("bits",&blobB);
    uint16_t auxB;
    auxB = blobB[0] << 8;
    auxB |= blobB[1];
    Serial.printf("Bits from firebase: %hu\n", auxB);

    // ***** REPEAT *****
    uint8_t byteR = results.repeat;
    Serial.printf("Repeat: %hu\n", results.repeat);
    if (uploadBlobData("repeat", &byteR, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobR;
    getBlobData("repeat",&blobR);
    uint8_t auxR;
    auxR = blobR[0];
    Serial.printf("Repeat from firebase: %hu\n", auxR);

    // ***** OVERFLOW *****
    uint8_t byteO = results.overflow;
    Serial.printf("Overflow: %hu\n", results.overflow);
    if (uploadBlobData("overflow", &byteO, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    std::vector<uint8_t> blobO;
    getBlobData("overflow",&blobO);
    uint8_t auxO;
    auxO = blobO[0];
    Serial.printf("Overflow from firebase: %hu\n", auxO);
}

// The repeating section of the code
void loop() {
    // Check if an IR message has been received.
    if (irrecv.decode(&results)) {  // We have captured something.
        // The capture has stopped at this point.
        decode_type_t protocol = results.decode_type;
        // int aux = -1;
        // protocol = (decode_type_t)aux;
        uint16_t size = results.bits;
        bool success = true;

        Serial.printf("rawlen: %hu\n", results.rawlen);
        Serial.println();

        uint8_t firstByte;
        uint8_t secondByte;

        uint8_t byteArray[results.rawlen * 2];

        Serial.println("IRRemote uint16_t array:");
        for (int i = 0; i < results.rawlen; i++) {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            firstByte = results.rawbuf[i] >> 8;
            if (firstByte < 16)
                Serial.print("0");
            Serial.print(firstByte, HEX);

            secondByte = results.rawbuf[i] & 0xFF;
            if (secondByte < 16)
                Serial.print("0");
            Serial.print(secondByte, HEX);

            Serial.print(" ");
            byteArray[i * 2] = firstByte;
            byteArray[i * 2 + 1] = secondByte;
        }

        Serial.println();
        Serial.println();
        Serial.println("uint16_t to uint8_t array:");
        for (int i = 0; i < results.rawlen * 2; i++) {
            if (i > 0 && i % 32 == 0)
                Serial.println();

            if (byteArray[i] < 16)
                Serial.print("0");

            Serial.print(byteArray[i], HEX);
            if (i % 2 != 0)
                Serial.print(" ");
        }
        Serial.println();

        std::vector<uint8_t> blob = uploadBlob(byteArray, results.rawlen * 2);
        // uploadBlobData("rawbuf",byteArray,results.rawlen * 2);

        // convert vector<uint8_t> to uint16_t[]
        uint16_t doubleByteArray[blob.size() / 2];
        Serial.println();
        Serial.println("vector<uint8_t>:");
        for (int i = 0; i < blob.size(); i++) {
            if (i > 0 && i % 32 == 0)
                Serial.println();

            if (blob[i] < 16)
                Serial.print("0");

            Serial.print(blob[i], HEX);
            if (i % 2 != 0) {
                Serial.print(" ");
                doubleByteArray[i / 2] |= blob[i];
            } else {
                doubleByteArray[i / 2] = blob[i];
                doubleByteArray[i / 2] = doubleByteArray[i / 2] << 8;
            }
        }
        Serial.println();
        Serial.println("vector to uint16_t array:");
        for (int i = 0; i < results.rawlen; i++) {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            firstByte = doubleByteArray[i] >> 8;
            if (firstByte < 16)
                Serial.print("0");
            Serial.print(firstByte, HEX);

            secondByte = doubleByteArray[i] & 0xFF;
            if (secondByte < 16)
                Serial.print("0");
            Serial.print(secondByte, HEX);

            Serial.print(" ");
        }

        uploadResult(results);

        Serial.println();

        // Is it a protocol we don't understand?
        if (protocol == decode_type_t::UNKNOWN) {  // Yes.
            // Convert the results into an array suitable for sendRaw().
            // resultToRawArray() allocates the memory we need for the array.
            uint16_t *raw_array = resultToRawArray(&results);
            // Find out how many elements are in the array.
            size = getCorrectedRawLength(&results);
            // Send it out via the IR LED circuit.
            irsend.sendRaw(raw_array, size, kFrequency);
            // Deallocate the memory allocated by resultToRawArray().
            delete[] raw_array;
        } else if (hasACState(protocol)) {  // Does the message require a state[]?
            // It does, so send with bytes instead.
            success = irsend.send(protocol, results.state, size / 8);
        } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
            success = irsend.send(protocol, results.value, size);
        }

        // Resume capturing IR messages. It was not restarted until after we sent
        // the message so we didn't capture our own message.
        irrecv.resume();

        // Display a crude timestamp & notification.
        uint32_t now = millis();
        Serial.printf(
            "%06u.%03u: A %d-bit %s message was %ssuccessfully retransmitted.\n",
            now / 1000, now % 1000, size, typeToString(protocol).c_str(),
            success ? "" : "un");
    }
    yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
}

std::vector<uint8_t> uploadBlob(uint8_t *data, uint16_t length) {
    // uint16_t *out = NULL;
    Serial.println("Firebase SET and GET");
    //Set binary data to database (also can use Firebase.set)
    if (Firebase.setBlob(firebaseDataData, path + "/Binary/Blob/data", data, sizeof(uint8_t) * length)) {
        Serial.println("PASSED");
        Serial.println("------------------------------------");
        Serial.println();
    } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseDataData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    Serial.println("------------------------------------");
    Serial.println("Get BLOB data test...");

    //Get binary data from database (also can use Firebase.get)
    if (Firebase.getBlob(firebaseDataData, path + "/Binary/Blob/data")) {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseDataData.dataPath());
        Serial.println("TYPE: " + firebaseDataData.dataType());
        Serial.print("VALUE: ");
        if (firebaseDataData.dataType() == "blob") {
            // std::vector<uint8_t> blob = firebaseDataData.blobData();
            return firebaseDataData.blobData();

            // uint16_t doubleByteArray[blob.size()/2];
            // DataToUint8 unionData;

            // for (int i = 0; i < blob.size(); i++) {
            //     if (i > 0 && i % 32 == 0)
            //         Serial.println();

            //     if (blob[i] < 16)
            //         Serial.print("0");

            //     Serial.print(blob[i], HEX);
            //     if (i % 2 != 0) {
            //         Serial.print(" ");
            //         unionData.data8x2[0] = blob[i];
            //         doubleByteArray[i/2] = unionData.data16;
            //         // doubleByteArray[i / 2] = blob[i];
            //         // doubleByteArray[i / 2] = doubleByteArray[i / 2] << 8;
            //     } else {
            //         unionData.data8x2[1] = blob[i];
            //         // doubleByteArray[i / 2] |= blob[i];
            //     }
            // }
            // out = doubleByteArray;
            Serial.println();
        }
        Serial.println("------------------------------------");
        Serial.println();
    } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseDataData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }
    // return;
}
