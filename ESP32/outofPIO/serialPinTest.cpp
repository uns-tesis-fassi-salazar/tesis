#include <Arduino.h>

String inString = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // send an intro:
  Serial.println("*** Setup OK ***");
}

int pin = 0;
void loop() {
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("Value:");
      pin = inString.toInt();
      Serial.println(pin);
      pinMode(pin, OUTPUT);
      digitalWrite(pin, 1);
      delay(1000);
      digitalWrite(pin, 0);
      // clear the string for new input:
      inString = "";
    }
  }
}