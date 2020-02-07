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

#include <IRModule.h>

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
decode_results results;

String commandKey = "";
void uploadCommand(decode_results &command);

void setUpIRSender() {
    irsend.begin();
    // Serial.print("IRSender will retransmit it on Pin ");
    // Serial.println(kIrLedPin);
}

void setUpIRReceiver() {
    irrecv.enableIRIn();
    // Serial.print("IRReceiver is now running and waiting for IR input on Pin ");
    // Serial.println(kRecvPin);
}

boolean recordingAndUploadCommand() { // Pasar por parametro un timeout de escucha

    if (irrecv.decode(&results)) {
        
        uploadCommand(results);

        // Resume capturing IR messages. It was not restarted until after we sent
        // the message so we didn't capture our own message.
        irrecv.resume();
        return true;
    }
    yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.
    return false;
}

bool sendCommand(decode_results *command) {

    uint16_t size = command->bits;
    decode_type_t protocol = command->decode_type;

    // The capture has stopped at this point.
    bool success = true;
    // Is it a protocol we don't understand?
    if (protocol == decode_type_t::UNKNOWN) {  // Yes.
        // Convert the results into an array suitable for sendRaw().
        // resultToRawArray() allocates the memory we need for the array.
        uint16_t *raw_array = resultToRawArray(command);
        // Find out how many elements are in the array.
        size = getCorrectedRawLength(command);
        // Send it out via the IR LED circuit.
        irsend.sendRaw(raw_array, size, kFrequency);
        // Deallocate the memory allocated by resultToRawArray().
        delete[] raw_array;
    } else if (hasACState(protocol)) {  // Does the message require a state[]?
        // It does, so send with bytes instead.
        success = irsend.send(protocol, command->state, size / 8);
    } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
        success = irsend.send(protocol, command->value, size);
    }
    // Serial.print("Saliendo de sendCommand con status: ");
    // Serial.println(success);
    return success;
}

boolean getCommand(decode_results *command) {
    boolean toRet = false;
    String aulaKey = getAulaAsignada();
    if (aulaKey != "") {

        commandKey = getKeyCommand(aulaKey);
        
        if (commandKey != "") {
            
            // ***** DECODE_TYPE *****
            std::vector<uint8_t> blobDecodeType;
            getBlobData(commandKey + "/decodeType",&blobDecodeType);
            // Serial.printf("DecodeType from firebase: %hu\n", blobDecodeType[0]);
            command->decode_type = (decode_type_t)blobDecodeType[0];

            // ***** VALUE *****
            std::vector<uint8_t> blobValue;
            getBlobData(commandKey + "/value",&blobValue);
            uint64_t auxValue;
            auxValue = blobValue[0] << 56;
            auxValue |= blobValue[1] << 48;
            auxValue |= blobValue[2] << 40;
            auxValue |= blobValue[3] << 32;
            auxValue |= blobValue[4] << 24;
            auxValue |= blobValue[5] << 16;
            auxValue |= blobValue[6] << 8;
            auxValue |= blobValue[7];
            // Serial.printf("Value from firebase: %iu", (uint32_t)auxValue >> 32);
            // Serial.printf("%iu\n", (uint32_t)(auxValue) && 0xFFFFFFFF);
            command->value = auxValue;

            // ***** COMMAND *****
            std::vector<uint8_t> blobCommand;
            getBlobData(commandKey + "/command",&blobCommand);
            uint32_t auxCommand;
            auxCommand = blobCommand[0] << 24;
            auxCommand |= blobCommand[1] << 16;
            auxCommand |= blobCommand[2] << 8;
            auxCommand |= blobCommand[3];
            // Serial.printf("Command from firebase: %hu\n", auxCommand);
            command->command = auxCommand;

            // ***** ADDRESS *****
            std::vector<uint8_t> blobAddress;
            getBlobData(commandKey + "/address",&blobAddress);
            uint32_t auxAddress;
            auxAddress = blobAddress[0] << 24;
            auxAddress |= blobAddress[1] << 16;
            auxAddress |= blobAddress[2] << 8;
            auxAddress |= blobAddress[3];
            // Serial.printf("Address from firebase: %hu\n", auxAddress);
            command->address = auxAddress;

            // ***** STATE *****
            std::vector<uint8_t> blobState;
            getBlobData(commandKey + "/state",&blobState);
            // Serial.println("State from firebase: ");
            for (size_t i = 0; i < blobState.size(); i++) {
                // Serial.printf("%hu ",blobState[i]);
                command->state[i] = blobState[i];
            }

            // ***** RAWBUF *****
            std::vector<uint8_t> blobRawbuf;
            getBlobData(commandKey + "/rawbuf",&blobRawbuf);
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
            command->rawbuf = doubleByteArray;

            // ***** RAWLEN *****
            std::vector<uint8_t> blobRawlen;
            getBlobData(commandKey + "/rawlen",&blobRawlen);
            uint16_t auxRawlen;
            auxRawlen = blobRawlen[0] << 8;
            auxRawlen |= blobRawlen[1];
            // Serial.println();
            // Serial.printf("Rawlen from firebase: %hu\n", auxRawlen);
            command->rawlen = auxRawlen;

            // ***** BITS *****
            std::vector<uint8_t> blobBits;
            getBlobData(commandKey + "/bits",&blobBits);
            uint16_t auxBits;
            auxBits = blobBits[0] << 8;
            auxBits |= blobBits[1];
            // Serial.printf("Bits from firebase: %hu\n", auxBits);
            command->bits = auxBits;

            // ***** REPEAT *****
            std::vector<uint8_t> blobRepeat;
            getBlobData(commandKey + "/repeat",&blobRepeat);
            // Serial.printf("Repeat from firebase: %hu\n", blobRepeat[0]);
            command->repeat = blobRepeat[0];

            // ***** OVERFLOW *****
            std::vector<uint8_t> blobOverflow;
            getBlobData(commandKey + "/overflow",&blobOverflow);
            // Serial.printf("Overflow from firebase: %hu\n", blobOverflow[0]);
            command->overflow = blobOverflow[0];

            toRet = true;
        }
    }
    return toRet;
}

void uploadCommand(decode_results &command) {
    commandKey = createKeyCommand();

    // ***** DECODE_TYPE *****
    uint8_t byteDecodeType = command.decode_type;
    // Serial.printf("Decode_type: %hu\n", command.decode_type);
    if (uploadBlobData(commandKey + "/decodeType", &byteDecodeType, 1)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }
    
    // ***** VALUE *****
    uint8_t byteArrayValue[8];
    byteArrayValue[0] = command.value >> 56;
    byteArrayValue[1] = command.value >> 48;
    byteArrayValue[2] = command.value >> 40;
    byteArrayValue[3] = command.value >> 32;
    byteArrayValue[4] = command.value >> 24;
    byteArrayValue[5] = command.value >> 16;
    byteArrayValue[6] = command.value >> 8;
    byteArrayValue[7] = command.value & 0xFF;

    // Serial.printf("Value: %iu", (uint32_t)command.value >> 32);
    // Serial.printf("%iu\n", (uint32_t)(command.value) && 0xFFFFFFFF);
    if (uploadBlobData(commandKey + "/value", byteArrayValue, 8)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** COMMAND *****
    uint8_t byteArrayCommand[4];
    byteArrayCommand[0] = command.command >> 24;
    byteArrayCommand[1] = command.command >> 16;
    byteArrayCommand[2] = command.command >> 8;
    byteArrayCommand[3] = command.command & 0xFF;

    // Serial.printf("Command: %hu\n", command.command);
    if (uploadBlobData(commandKey + "/command", byteArrayCommand, 4)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** ADDRESS *****
    uint8_t byteArrayAddress[4];
    byteArrayAddress[0] = command.address >> 24;
    byteArrayAddress[1] = command.address >> 16;
    byteArrayAddress[2] = command.address >> 8;
    byteArrayAddress[3] = command.address & 0xFF;

    // Serial.printf("Address: %hu\n", command.address);
    if (uploadBlobData(commandKey + "/address", byteArrayAddress, 4)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** STATE *****
    // Serial.println("State: ");
    for (size_t i = 0; i < kStateSizeMax; i++) {
        // Serial.printf("%hu ", command.state[i]);
    }
    Serial.println();
    if (uploadBlobData(commandKey + "/state", command.state, kStateSizeMax)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** RAWBUF *****
    uint8_t firstByte, secondByte;
    uint8_t byteArrayRawbuf[command.rawlen * 2];

    for (int i = 0; i < command.rawlen; i++) {
        firstByte = command.rawbuf[i] >> 8;
        secondByte = command.rawbuf[i] & 0xFF;
        byteArrayRawbuf[i * 2] = firstByte;
        byteArrayRawbuf[i * 2 + 1] = secondByte;
    }
    // printRawbuf(command);
    // Serial.println();
    if (uploadBlobData(commandKey + "/rawbuf", byteArrayRawbuf, command.rawlen * 2)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** RAWLEN *****
    uint8_t byteArrayRawlen[2];
    byteArrayRawlen[0] = command.rawlen >> 8;
    byteArrayRawlen[1] = command.rawlen & 0xFF;

    // Serial.printf("Rawlen: %hu\n", command.rawlen);
    if (uploadBlobData(commandKey + "/rawlen", byteArrayRawlen, 2)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** BITS *****
    uint8_t byteArrayBits[2];
    byteArrayBits[0] = command.bits >> 8;
    byteArrayBits[1] = command.bits & 0xFF;

    // Serial.printf("Bits: %hu\n", command.bits);
    if (uploadBlobData(commandKey + "/bits", byteArrayBits, 2)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** REPEAT *****
    uint8_t byteRepeat = command.repeat;
    // Serial.printf("Repeat: %hu\n", command.repeat);
    if (uploadBlobData(commandKey + "/repeat", &byteRepeat, 1)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }

    // ***** OVERFLOW *****
    uint8_t byteOverflow = command.overflow;
    // Serial.printf("Overflow: %hu\n", command.overflow);
    if (uploadBlobData(commandKey + "/overflow", &byteOverflow, 1)) {
        // Serial.println("PASSED");
    } else {
        // Serial.println("FAILED");
    }
}