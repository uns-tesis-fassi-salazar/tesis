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
const uint16_t kIrLedPin = 23;

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
decode_results results,command;

void uploadResult(decode_results &results);
void getResults(decode_results *command);
bool sendCommand(decode_results results, uint16_t *size, decode_type_t *protocol);
void printRawbuf(decode_results &results);
void printBlobRawbuf(uint16_t * doubleByteArray, int arrayLenght);

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
    // WiFi.begin("BVNET-FD4B", "2WC468401228");
    Serial.print("Conectando");
    while (!WiFi.isConnected()) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado.");

    // setupFirebase(WiFi.macAddress());
}

// The repeating section of the code
void loop() {
    // Check if an IR message has been received.
    if (irrecv.decode(&results)) {  // We have captured something.
        
        // uploadResult(results);

        // getResults(&command);

        uint16_t size;
        decode_type_t protocol;

        bool success = sendCommand(results, &size, &protocol);

        // Display a crude timestamp & notification.
        uint32_t now = millis();
        Serial.printf(
            "%06u.%03u: A %d-bit %s message was %ssuccessfully retransmitted.\n",
            now / 1000, now % 1000, size, typeToString(protocol).c_str(),
            success ? "" : "un");

        // Resume capturing IR messages. It was not restarted until after we sent
        // the message so we didn't capture our own message.
        irrecv.resume();

    }
    yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
}

bool sendCommand(decode_results results, uint16_t *size, decode_type_t *protocol) {
    // The capture has stopped at this point.
    *protocol = results.decode_type;
    // int aux = -1;
    // protocol = (decode_type_t)aux;
    *size = results.bits;
    bool success = true;
    // Is it a protocol we don't understand?
    if (*protocol == decode_type_t::UNKNOWN) {  // Yes.
        // Convert the results into an array suitable for sendRaw().
        // resultToRawArray() allocates the memory we need for the array.
        uint16_t *raw_array = resultToRawArray(&results);
        // Find out how many elements are in the array.
        *size = getCorrectedRawLength(&results);
        // Send it out via the IR LED circuit.
        irsend.sendRaw(raw_array, *size, kFrequency);
        // Deallocate the memory allocated by resultToRawArray().
        delete[] raw_array;
    } else if (hasACState(*protocol)) {  // Does the message require a state[]?
        // It does, so send with bytes instead.
        success = irsend.send(*protocol, results.state, *size / 8);
    } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
        success = irsend.send(*protocol, results.value, *size);
    }
    return success;
}

void getResults(decode_results *command) {
    
    // ***** DECODE_TYPE *****
    std::vector<uint8_t> blobDecodeType;
    getBlobData("decode_type",&blobDecodeType);
    Serial.printf("Decode_type from firebase: %hu\n", blobDecodeType[0]);
    command->decode_type = (decode_type_t)blobDecodeType[0];

    // ***** VALUE *****
    std::vector<uint8_t> blobValue;
    getBlobData("value",&blobValue);
    uint64_t auxValue;
    auxValue = blobValue[0] << 56;
    auxValue |= blobValue[1] << 48;
    auxValue |= blobValue[2] << 40;
    auxValue |= blobValue[3] << 32;
    auxValue |= blobValue[4] << 24;
    auxValue |= blobValue[5] << 16;
    auxValue |= blobValue[6] << 8;
    auxValue |= blobValue[7];
    Serial.printf("Value from firebase: %iu", (uint32_t)auxValue >> 32);
    Serial.printf("%iu\n", (uint32_t)(auxValue) && 0xFFFFFFFF);
    command->value = auxValue;

    // ***** COMMAND *****
    std::vector<uint8_t> blobCommand;
    getBlobData("command",&blobCommand);
    uint32_t auxCommand;
    auxCommand = blobCommand[0] << 24;
    auxCommand |= blobCommand[1] << 16;
    auxCommand |= blobCommand[2] << 8;
    auxCommand |= blobCommand[3];
    Serial.printf("Command from firebase: %hu\n", auxCommand);
    command->command = auxCommand;

    // ***** ADDRESS *****
    std::vector<uint8_t> blobAddress;
    getBlobData("address",&blobAddress);
    uint32_t auxAddress;
    auxAddress = blobAddress[0] << 24;
    auxAddress |= blobAddress[1] << 16;
    auxAddress |= blobAddress[2] << 8;
    auxAddress |= blobAddress[3];
    Serial.printf("Address from firebase: %hu\n", auxAddress);
    command->address = auxAddress;

    // ***** STATE *****
    std::vector<uint8_t> blobState;
    getBlobData("state",&blobState);
    Serial.println("State from firebase: ");
    for (size_t i = 0; i < blobState.size(); i++) {
        Serial.printf("%hu ",blobState[i]);
        command->state[i] = blobState[i];
    }

    // ***** RAWBUF *****
    std::vector<uint8_t> blobRawbuf;
    getBlobData("rawbuf",&blobRawbuf);
    // convert vector<uint8_t> to uint16_t[]
    uint16_t doubleByteArray[blobRawbuf.size() / 2];
    for (int i = 0; i < blobRawbuf.size(); i++) {
        if (i % 2 != 0) {
            doubleByteArray[i / 2] |= blobRawbuf[i];
        } else {
            doubleByteArray[i / 2] = blobRawbuf[i];
            doubleByteArray[i / 2] = doubleByteArray[i / 2] << 8;
        }
    }
    printBlobRawbuf(doubleByteArray,blobRawbuf.size() / 2);
    command->rawbuf = doubleByteArray;

    // ***** RAWLEN *****
    std::vector<uint8_t> blobRawlen;
    getBlobData("rawlen",&blobRawlen);
    uint16_t auxRawlen;
    auxRawlen = blobRawlen[0] << 8;
    auxRawlen |= blobRawlen[1];
    Serial.printf("Rawlen from firebase: %hu\n", auxRawlen);
    command->rawlen = auxRawlen;

    // ***** BITS *****
    std::vector<uint8_t> blobBits;
    getBlobData("bits",&blobBits);
    uint16_t auxBits;
    auxBits = blobBits[0] << 8;
    auxBits |= blobBits[1];
    Serial.printf("Bits from firebase: %hu\n", auxBits);
    command->bits = auxBits;

    // ***** REPEAT *****
    std::vector<uint8_t> blobRepeat;
    getBlobData("repeat",&blobRepeat);
    Serial.printf("Repeat from firebase: %hu\n", blobRepeat[0]);
    command->repeat = blobRepeat[0];

    // ***** OVERFLOW *****
    std::vector<uint8_t> blobOverflow;
    getBlobData("overflow",&blobOverflow);
    Serial.printf("Overflow from firebase: %hu\n", blobOverflow[0]);
    command->overflow = blobOverflow[0];
}

void uploadResult(decode_results &results) {
    Serial.println("");
    Serial.println("Firebase SET result - Our Test");

    // ***** DECODE_TYPE *****
    uint8_t byteDecodeType = results.decode_type;
    Serial.printf("Decode_type: %hu\n", results.decode_type);
    if (uploadBlobData("decode_type", &byteDecodeType, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
    
    // ***** VALUE *****
    uint8_t byteArrayValue[8];
    byteArrayValue[0] = results.value >> 56;
    byteArrayValue[1] = results.value >> 48;
    byteArrayValue[2] = results.value >> 40;
    byteArrayValue[3] = results.value >> 32;
    byteArrayValue[4] = results.value >> 24;
    byteArrayValue[5] = results.value >> 16;
    byteArrayValue[6] = results.value >> 8;
    byteArrayValue[7] = results.value & 0xFF;

    Serial.printf("Value: %iu", (uint32_t)results.value >> 32);
    Serial.printf("%iu\n", (uint32_t)(results.value) && 0xFFFFFFFF);
    if (uploadBlobData("value", byteArrayValue, 8)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** COMMAND *****
    uint8_t byteArrayCommand[4];
    byteArrayCommand[0] = results.command >> 24;
    byteArrayCommand[1] = results.command >> 16;
    byteArrayCommand[2] = results.command >> 8;
    byteArrayCommand[3] = results.command & 0xFF;

    Serial.printf("Command: %hu\n", results.command);
    if (uploadBlobData("command", byteArrayCommand, 4)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** ADDRESS *****
    uint8_t byteArrayAddress[4];
    byteArrayAddress[0] = results.address >> 24;
    byteArrayAddress[1] = results.address >> 16;
    byteArrayAddress[2] = results.address >> 8;
    byteArrayAddress[3] = results.address & 0xFF;

    Serial.printf("Address: %hu\n", results.address);
    if (uploadBlobData("address", byteArrayAddress, 4)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** STATE *****
    Serial.println("State: ");
    for (size_t i = 0; i < kStateSizeMax; i++) {
        Serial.printf("%hu ", results.state[i]);
    }
    Serial.println();
    if (uploadBlobData("state", results.state, kStateSizeMax)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** RAWBUF *****
    uint8_t firstByte, secondByte;
    uint8_t byteArrayRawbuf[results.rawlen * 2];

    for (int i = 0; i < results.rawlen; i++) {
        firstByte = results.rawbuf[i] >> 8;
        secondByte = results.rawbuf[i] & 0xFF;
        byteArrayRawbuf[i * 2] = firstByte;
        byteArrayRawbuf[i * 2 + 1] = secondByte;
    }
    printRawbuf(results);
    Serial.println();
    if (uploadBlobData("rawbuf", byteArrayRawbuf, results.rawlen * 2)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** RAWLEN *****
    uint8_t byteArrayRawlen[2];
    byteArrayRawlen[0] = results.rawlen >> 8;
    byteArrayRawlen[1] = results.rawlen & 0xFF;

    Serial.printf("Rawlen: %hu\n", results.rawlen);
    if (uploadBlobData("rawlen", byteArrayRawlen, 2)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** BITS *****
    uint8_t byteArrayBits[2];
    byteArrayBits[0] = results.bits >> 8;
    byteArrayBits[1] = results.bits & 0xFF;

    Serial.printf("Bits: %hu\n", results.bits);
    if (uploadBlobData("bits", byteArrayBits, 2)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** REPEAT *****
    uint8_t byteRepeat = results.repeat;
    Serial.printf("Repeat: %hu\n", results.repeat);
    if (uploadBlobData("repeat", &byteRepeat, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }

    // ***** OVERFLOW *****
    uint8_t byteOverflow = results.overflow;
    Serial.printf("Overflow: %hu\n", results.overflow);
    if (uploadBlobData("overflow", &byteOverflow, 1)) {
        Serial.println("PASSED");
    } else {
        Serial.println("FAILED");
    }
}

void printRawbuf(decode_results &results) {
    uint8_t firstByte, secondByte;
    
    Serial.println();
    Serial.println("Rawbuf uint16_t array:");
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
    }
}

void printBlobRawbuf(uint16_t * doubleByteArray, int arrayLenght) {
    uint8_t firstByte, secondByte;

    Serial.println();
    Serial.println("Rawbuf uint16_t array from firebase:");
    for (int i = 0; i < arrayLenght; i++) {
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
}