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
extern String aulaKey;
bool uploadCommand(decode_results &command);

// test
// decode_results results,command;
// void getResults(decode_results *command);
// bool sendCommand(decode_results results, uint16_t *size, decode_type_t *protocol);

void setUpIRSender() {
    irsend.begin();
}

void setUpIRReceiver() {
    irrecv.enableIRIn();
}

bool recordingAndUploadCommand() {

    uploadLogs("Iniciando la grabacion del comando AC...");
    // irrecv.enableIRIn();
    bool todoOk = false;
    bool commandDecode = false;
    bool timeOut = false;
    ulong recordTimeOut = 30000;    // 30 segundos
    ulong redordStartTime = millis();
    while ((!commandDecode) && (!timeOut)) {
        blinkLed();
        commandDecode = irrecv.decode(&results);
        yield();
        if (lapTimer(recordTimeOut, &redordStartTime)){
            timeOut = true;
        }
    }
    
    if (commandDecode) {
        encenderLed();
        if (!uploadCommand(results)) {
            todoOk = false;
        }
        irrecv.resume();
        todoOk = true;
    }
    uploadLogs("Saliendo del modo grabación comando AC...");
    apagarLed();
    // irrecv.disableIRIn();
    return todoOk;
}

bool sendCommand(decode_results *command) {

    uploadLogs("Emitir comando");

    if (!pauseStream()) {
        return false;
    }
    delay(100);

    uint16_t size = command->bits;
    decode_type_t protocol = command->decode_type;

    bool success = true;
    if (protocol == decode_type_t::UNKNOWN) {
        uint16_t *raw_array = resultToRawArray(command);
        size = getCorrectedRawLength(command);
        irsend.sendRaw(raw_array, size, kFrequency);
        delete[] raw_array;
    } else if (hasACState(protocol)) {
        success = irsend.send(protocol, command->state, size / 8);
    } else {
        success = irsend.send(protocol, command->value, size);
    }
    success ? uploadLogs("Comando emitido: true") : uploadLogs("Comando emitido: false");
    
    if (!restoreStream()) {
        return false;
    }
    delay(100);

    return success;
}

bool uploadCommand(decode_results &command) {
    bool todoOk = true;
    commandKey = createKeyCommand();

    if (commandKey != "") {
        uploadLogs("Uploading command...");

        int totalSize = 0;  // command.decode_type = 1  // command.value = 8    // command.command = 4  // command.address = 4  // command.state = kStateSizeMax = 53
        totalSize = 76 + (command.rawlen * 2);  // command.rawbuf = command.rawlen * 2  // command.rawlen = 2   // command.bits = 2 // command.repeat = 1   // command.overflow = 1
        uint8_t arrayToUpload[totalSize];

        arrayToUpload[0] = command.decode_type;

        arrayToUpload[1] = command.value >> 56;
        arrayToUpload[2] = command.value >> 48;
        arrayToUpload[3] = command.value >> 40;
        arrayToUpload[4] = command.value >> 32;
        arrayToUpload[5] = command.value >> 24;
        arrayToUpload[6] = command.value >> 16;
        arrayToUpload[7] = command.value >> 8;
        arrayToUpload[8] = command.value & 0xFF;

        arrayToUpload[9] = command.command >> 24;
        arrayToUpload[10] = command.command >> 16;
        arrayToUpload[11] = command.command >> 8;
        arrayToUpload[12] = command.command & 0xFF;

        arrayToUpload[13] = command.address >> 24;
        arrayToUpload[14] = command.address >> 16;
        arrayToUpload[15] = command.address >> 8;
        arrayToUpload[16] = command.address & 0xFF;

        arrayToUpload[17] = command.rawlen >> 8;
        arrayToUpload[18] = command.rawlen & 0xFF;

        arrayToUpload[19] = command.bits >> 8;
        arrayToUpload[20] = command.bits & 0xFF;

        arrayToUpload[21] = command.repeat;
        arrayToUpload[22] = command.overflow;

        for (size_t i = 0; i < kStateSizeMax; i++) {
            arrayToUpload[23 + i] = command.state[i];
        }

        uint8_t firstByte, secondByte;
        size_t index = 23 + kStateSizeMax;
        for (size_t i = 0; i < command.rawlen; i++) {
            firstByte = command.rawbuf[i] >> 8;
            secondByte = command.rawbuf[i] & 0xFF;
            arrayToUpload[index + (i * 2)] = firstByte;
            arrayToUpload[index + (i * 2) + 1] = secondByte;
        }

        if (uploadBlobData(commandKey + "/fullCommand", arrayToUpload, totalSize)) {
            // uploadLogs("Repeat: PASSED");
        } else {
            uploadLogs("fullCommand: FAILED");
            todoOk = false;
        }

        // ***** SETTIMESTAMP en MARCA *****
        if (setCommandValue(commandKey + "/marca",tiempoToString())) {
            // uploadLogs("Marca: PASSED");
        } else {
            uploadLogs("Marca: FAILED");
            todoOk = false;
        }
        if (setCommandValue(commandKey + "/modelo","")) {
            // uploadLogs("Modelo: PASSED");
        } else {
            uploadLogs("Modelo: FAILED");
            todoOk = false;
        }

        todoOk = todoOk && true;
    } else {
        todoOk = false;
    }

    if (!todoOk) {
        deleteKeyCommand(commandKey);
    }

    if ((aulaKey != "") && (todoOk)) {
        uploadKeyCommand(aulaKey,commandKey);
    }

    return todoOk;
}

bool getCommand(decode_results *command) {
    bool todoOk = false;
    if (aulaKey != "") {

        Serial.println("Downloading command...");
        uploadLogs("Downloading command...");

        commandKey = getKeyCommand(aulaKey);
        
        if (commandKey != "") {

            std::vector<uint8_t> fullCommand;
            if (!getBlobData(commandKey + "/fullCommand",&fullCommand)) {
                return todoOk;
            }
            
            command->decode_type = (decode_type_t)fullCommand[0];

            command->value = fullCommand[1] << 56;
            command->value |= fullCommand[2] << 48;
            command->value |= fullCommand[3] << 40;
            command->value |= fullCommand[4] << 32;
            command->value |= fullCommand[5] << 24;
            command->value |= fullCommand[6] << 16;
            command->value |= fullCommand[7] << 8;
            command->value |= fullCommand[8];

            command->command = fullCommand[9] << 24;
            command->command |= fullCommand[10] << 16;
            command->command |= fullCommand[11] << 8;
            command->command |= fullCommand[12];

            command->address = fullCommand[13] << 24;
            command->address |= fullCommand[14] << 16;
            command->address |= fullCommand[15] << 8;
            command->address |= fullCommand[16];

            command->rawlen = fullCommand[17] << 8;
            command->rawlen |= fullCommand[18];

            command->bits = fullCommand[19] << 8;
            command->bits |= fullCommand[20];

            command->repeat = fullCommand[21];
            command->overflow = fullCommand[22];

            for (size_t i = 0; i < kStateSizeMax; i++) {
                command->state[i] = fullCommand[23 + i];
            }

            size_t index = 23 + kStateSizeMax;
            uint16_t doubleByteArray[command->rawlen];
            for(size_t i = 0; i < (command->rawlen * 2); i++) {
                if (i % 2 != 0) {
                    doubleByteArray[i / 2] |= fullCommand[index + i];
                } else {
                    doubleByteArray[i / 2] = fullCommand[index + i] << 8;
                }
            }
            command->rawbuf = doubleByteArray;

            todoOk = true;
        }
    }
    return todoOk;
}

// --------------------- BUTTON -----------------------

struct RecordButton {
    const uint8_t PIN;
    bool state;
    bool pressed;
    ulong pressTime;
};

RecordButton recordButton = {IR_BUTTON_PIN, false, false, 0};

void IRAM_ATTR recordButtonInterrupt() {
    recordButton.pressTime = millis();
    recordButton.pressed = true;
}

void setUpRecordCommandButton() {
    pinMode(recordButton.PIN, PULLUP);
    attachInterrupt(recordButton.PIN, recordButtonInterrupt, FALLING);
}

void recordButtonListener() {
    if (recordButton.pressed) {
        if (digitalRead(recordButton.PIN) == LOW) {
            ulong readTime = millis();
            if ((readTime - recordButton.pressTime) > 1000) {
                uploadLogs("IR Record Command Button pressss!");
                recordButton.pressed = false;
                recordingAndUploadCommand();
            }
        } else {
            recordButton.pressed = false;
        }
    }
}

void listenRecordCommandButton() {
    recordButtonListener();
}


// bool getCommand(decode_results *command) {
//     bool todoOk = false;
//     if (aulaKey != "") {

//         uploadLogs("Downloading command...");

//         commandKey = getKeyCommand(aulaKey);
//         Serial.println("getKeyCommand(): "+ commandKey);
        
//         if (commandKey != "") {
            
//             // ***** DECODE_TYPE *****
//             std::vector<uint8_t> blobDecodeType;
//             if (!getBlobData(commandKey + "/decodeType",&blobDecodeType)) {
//                 return todoOk;
//             }
//             Serial.printf("DecodeType from firebase: %hu\n", blobDecodeType[0]);
//             command->decode_type = (decode_type_t)blobDecodeType[0];

//             // ***** VALUE *****
//             std::vector<uint8_t> blobValue;
//             if (!getBlobData(commandKey + "/value",&blobValue)) {
//                 return todoOk;
//             }
//             uint64_t auxValue;
//             auxValue = blobValue[0] << 56;
//             auxValue |= blobValue[1] << 48;
//             auxValue |= blobValue[2] << 40;
//             auxValue |= blobValue[3] << 32;
//             auxValue |= blobValue[4] << 24;
//             auxValue |= blobValue[5] << 16;
//             auxValue |= blobValue[6] << 8;
//             auxValue |= blobValue[7];
//             Serial.printf("Value from firebase: %iu", (uint32_t)auxValue >> 32);
//             Serial.printf("%iu\n", (uint32_t)(auxValue) && 0xFFFFFFFF);
//             command->value = auxValue;

//             // ***** COMMAND *****
//             std::vector<uint8_t> blobCommand;
//             if (!getBlobData(commandKey + "/command",&blobCommand)) {
//                 return todoOk;
//             }
//             uint32_t auxCommand;
//             auxCommand = blobCommand[0] << 24;
//             auxCommand |= blobCommand[1] << 16;
//             auxCommand |= blobCommand[2] << 8;
//             auxCommand |= blobCommand[3];
//             Serial.printf("Command from firebase: %hu\n", auxCommand);
//             command->command = auxCommand;

//             // ***** ADDRESS *****
//             std::vector<uint8_t> blobAddress;
//             if (!getBlobData(commandKey + "/address",&blobAddress)) {
//                 return todoOk;
//             }
//             uint32_t auxAddress;
//             auxAddress = blobAddress[0] << 24;
//             auxAddress |= blobAddress[1] << 16;
//             auxAddress |= blobAddress[2] << 8;
//             auxAddress |= blobAddress[3];
//             Serial.printf("Address from firebase: %hu\n", auxAddress);
//             command->address = auxAddress;

//             // ***** STATE *****
//             std::vector<uint8_t> blobState;
//             if (!getBlobData(commandKey + "/state",&blobState)){
//                 return todoOk;
//             }
//             Serial.println("State from firebase: ");
//             for (size_t i = 0; i < blobState.size(); i++) {
//                 Serial.printf("%hu ",blobState[i]);
//                 command->state[i] = blobState[i];
//             }

//             // ***** RAWBUF *****
//             std::vector<uint8_t> blobRawbuf;
//             if (!getBlobData(commandKey + "/rawbuf",&blobRawbuf)) {
//                 return todoOk;
//             }
//             // convert vector<uint8_t> to uint16_t[]
//             uint16_t doubleByteArray[blobRawbuf.size() / 2];
//             for (int i = 0; i < blobRawbuf.size(); i++) {
//                 if (i % 2 != 0) {
//                     doubleByteArray[i / 2] |= blobRawbuf[i];
//                 } else {
//                     doubleByteArray[i / 2] = blobRawbuf[i];
//                     doubleByteArray[i / 2] = doubleByteArray[i / 2] << 8;
//                 }
//             }
//             command->rawbuf = doubleByteArray;

//             // ***** RAWLEN *****
//             std::vector<uint8_t> blobRawlen;
//             if (!getBlobData(commandKey + "/rawlen",&blobRawlen)) {
//                 return todoOk;
//             }
//             uint16_t auxRawlen;
//             auxRawlen = blobRawlen[0] << 8;
//             auxRawlen |= blobRawlen[1];
//             Serial.println();
//             Serial.printf("Rawlen from firebase: %hu\n", auxRawlen);
//             command->rawlen = auxRawlen;

//             // ***** BITS *****
//             std::vector<uint8_t> blobBits;
//             if (!getBlobData(commandKey + "/bits",&blobBits)) {
//                 return todoOk;
//             }
//             uint16_t auxBits;
//             auxBits = blobBits[0] << 8;
//             auxBits |= blobBits[1];
//             Serial.printf("Bits from firebase: %hu\n", auxBits);
//             command->bits = auxBits;

//             // ***** REPEAT *****
//             std::vector<uint8_t> blobRepeat;
//             if (!getBlobData(commandKey + "/repeat",&blobRepeat)) {
//                 return todoOk;
//             }
//             Serial.printf("Repeat from firebase: %hu\n", blobRepeat[0]);
//             command->repeat = blobRepeat[0];

//             // ***** OVERFLOW *****
//             std::vector<uint8_t> blobOverflow;
//             if (!getBlobData(commandKey + "/overflow",&blobOverflow)) {
//                 return todoOk;
//             }
//             Serial.printf("Overflow from firebase: %hu\n", blobOverflow[0]);
//             command->overflow = blobOverflow[0];

//             todoOk = true;
//         }
//     }
//     // uploadLogs("getCommand: " + todoOk);
//     return todoOk;
// }

// bool uploadCommand(decode_results &command) {
//     bool todoOk = true;
//     commandKey = createKeyCommand();

//     if (commandKey != "") {

//         uploadLogs("Uploading command...");

//         // ***** DECODE_TYPE *****
//         uint8_t byteDecodeType = command.decode_type;
//         // Serial.printf("Decode_type: %hu\n", command.decode_type);
//         if (uploadBlobData(commandKey + "/decodeType", &byteDecodeType, 1)) {
//             // uploadLogs("Decode_type: PASSED");
//         } else {
//             uploadLogs("Decode_type: FAILED");
//             todoOk = false;
//         }
        
//         // ***** VALUE *****
//         uint8_t byteArrayValue[8];
//         byteArrayValue[0] = command.value >> 56;
//         byteArrayValue[1] = command.value >> 48;
//         byteArrayValue[2] = command.value >> 40;
//         byteArrayValue[3] = command.value >> 32;
//         byteArrayValue[4] = command.value >> 24;
//         byteArrayValue[5] = command.value >> 16;
//         byteArrayValue[6] = command.value >> 8;
//         byteArrayValue[7] = command.value & 0xFF;

//         // Serial.printf("Value: %iu", (uint32_t)command.value >> 32);
//         // Serial.printf("%iu\n", (uint32_t)(command.value) && 0xFFFFFFFF);
//         if (uploadBlobData(commandKey + "/value", byteArrayValue, 8)) {
//             // uploadLogs("Value: PASSED");
//         } else {
//             uploadLogs("Value: FAILED");
//             todoOk = false;
//         }

//         // ***** COMMAND *****
//         uint8_t byteArrayCommand[4];
//         byteArrayCommand[0] = command.command >> 24;
//         byteArrayCommand[1] = command.command >> 16;
//         byteArrayCommand[2] = command.command >> 8;
//         byteArrayCommand[3] = command.command & 0xFF;

//         // Serial.printf("Command: %hu\n", command.command);
//         if (uploadBlobData(commandKey + "/command", byteArrayCommand, 4)) {
//             // uploadLogs("Command: PASSED");
//         } else {
//             uploadLogs("Command: FAILED");
//             todoOk = false;
//         }

//         // ***** ADDRESS *****
//         uint8_t byteArrayAddress[4];
//         byteArrayAddress[0] = command.address >> 24;
//         byteArrayAddress[1] = command.address >> 16;
//         byteArrayAddress[2] = command.address >> 8;
//         byteArrayAddress[3] = command.address & 0xFF;

//         // Serial.printf("Address: %hu\n", command.address);
//         if (uploadBlobData(commandKey + "/address", byteArrayAddress, 4)) {
//             // uploadLogs("Address: PASSED");
//         } else {
//             uploadLogs("Address: FAILED");
//             todoOk = false;
//         }

//         // ***** STATE *****
//         // Serial.println("State: ");
//         // for (size_t i = 0; i < kStateSizeMax; i++) {
//         //     Serial.printf("%hu ", command.state[i]);
//         // }
//         // Serial.println();
//         if (uploadBlobData(commandKey + "/state", command.state, kStateSizeMax)) {
//             // uploadLogs("State: PASSED");
//         } else {
//             uploadLogs("State: FAILED");
//             todoOk = false;
//         }

//         // ***** RAWBUF *****
//         uint8_t firstByte, secondByte;
//         uint8_t byteArrayRawbuf[command.rawlen * 2];

//         for (int i = 0; i < command.rawlen; i++) {
//             firstByte = command.rawbuf[i] >> 8;
//             secondByte = command.rawbuf[i] & 0xFF;
//             byteArrayRawbuf[i * 2] = firstByte;
//             byteArrayRawbuf[i * 2 + 1] = secondByte;
//         }
//         // printRawbuf(command);
//         // Serial.println();
//         if (uploadBlobData(commandKey + "/rawbuf", byteArrayRawbuf, command.rawlen * 2)) {
//             // uploadLogs("Rawbuf: PASSED");
//         } else {
//             uploadLogs("Rawbuf: FAILED");
//             todoOk = false;
//         }

//         // ***** RAWLEN *****
//         uint8_t byteArrayRawlen[2];
//         byteArrayRawlen[0] = command.rawlen >> 8;
//         byteArrayRawlen[1] = command.rawlen & 0xFF;

//         // Serial.printf("Rawlen: %hu\n", command.rawlen);
//         if (uploadBlobData(commandKey + "/rawlen", byteArrayRawlen, 2)) {
//             // uploadLogs("Rawlen: PASSED");
//         } else {
//             uploadLogs("Rawlen: FAILED");
//             todoOk = false;
//         }

//         // ***** BITS *****
//         uint8_t byteArrayBits[2];
//         byteArrayBits[0] = command.bits >> 8;
//         byteArrayBits[1] = command.bits & 0xFF;

//         // Serial.printf("Bits: %hu\n", command.bits);
//         if (uploadBlobData(commandKey + "/bits", byteArrayBits, 2)) {
//             // uploadLogs("Bits: PASSED");
//         } else {
//             uploadLogs("Bits: FAILED");
//             todoOk = false;
//         }

//         // ***** REPEAT *****
//         uint8_t byteRepeat = command.repeat;
//         // Serial.printf("Repeat: %hu\n", command.repeat);
//         if (uploadBlobData(commandKey + "/repeat", &byteRepeat, 1)) {
//             // uploadLogs("Repeat: PASSED");
//         } else {
//             uploadLogs("Repeat: FAILED");
//             todoOk = false;
//         }

//         // ***** OVERFLOW *****
//         uint8_t byteOverflow = command.overflow;
//         // Serial.printf("Overflow: %hu\n", command.overflow);
//         if (uploadBlobData(commandKey + "/overflow", &byteOverflow, 1)) {
//             // uploadLogs("Overflow: PASSED");
//         } else {
//             uploadLogs("Overflow: FAILED");
//             todoOk = false;
//         }

//         // ***** SETTIMESTAMP en MARCA *****
//         if (setCommandValue(commandKey + "/marca",tiempoToString())) {
//             // uploadLogs("Marca: PASSED");
//         } else {
//             uploadLogs("Marca: FAILED");
//             todoOk = false;
//         }
//         if (setCommandValue(commandKey + "/modelo","")) {
//             // uploadLogs("Modelo: PASSED");
//         } else {
//             uploadLogs("Modelo: FAILED");
//             todoOk = false;
//         }

//         todoOk = todoOk && true;
//     } else {
//         todoOk = false;
//     }

//     if (!todoOk) {
//         deleteKeyCommand(commandKey);
//     }

//     if ((aulaKey != "") && (todoOk)) {
//         uploadKeyCommand(aulaKey,commandKey);
//     }

//     return todoOk;
// }

// void testLoop() {
//     if (irrecv.decode(&results)) {
//         uint16_t size;
//         decode_type_t protocol;

//         bool success = sendCommand(results, &size, &protocol);

//         uint32_t now = millis();
//         Serial.printf(
//             "%06u.%03u: A %d-bit %s message was %ssuccessfully retransmitted.\n",
//             now / 1000, now % 1000, size, typeToString(protocol).c_str(),
//             success ? "" : "un");
//         irrecv.resume();
//     }
//     yield();
// }

// bool sendCommandTest(decode_results results, uint16_t *size, decode_type_t *protocol) {

//     if (!pauseStream()) {
//         return false;
//     }
//     Serial.println("pauseStream()");
//     delay(100);

//     *protocol = results.decode_type;
//     *size = results.bits;
//     bool success = true;
    
//     if (*protocol == decode_type_t::UNKNOWN) {
//         uint16_t *raw_array = resultToRawArray(&results);
//         *size = getCorrectedRawLength(&results);
//         irsend.sendRaw(raw_array, *size, kFrequency);
//         delete[] raw_array;
//         Serial.println(results.decode_type);
//         Serial.println("decode_type_t::UNKNOWN");
//     } else if (hasACState(*protocol)) {
//         success = irsend.send(*protocol, results.state, *size / 8);
//         Serial.println(results.decode_type);
//         Serial.println("hasACState");
//     } else {
//         success = irsend.send(*protocol, results.value, *size);
//         Serial.println(results.decode_type);
//         Serial.println("default");
//     }

//     delay(100);
//     if (!restoreStream()) {
//         return false;
//     }
//     Serial.println("restoreStream()");
//     delay(100);

//     return success;
// }