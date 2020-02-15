#include <Arduino.h>
#include <ACS712.h>

#define HALL_PIN GPIO_NUM_34

ACS712 hallSensor(ACS712_20A, HALL_PIN);

float prevHallValue, currentHallValue;
int calibratedZero = 0;

void setup()
{
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect.
    }
    delay(1000);
    calibratedZero = hallSensor.calibrate();
    delay(1000);
    Serial.printf("Valor calibrado: %i\n", calibratedZero);
}

void loop()
{
    currentHallValue = hallSensor.getCurrentAC(50);
    Serial.printf("Hall: %f\n", currentHallValue);
    delay(200);
}