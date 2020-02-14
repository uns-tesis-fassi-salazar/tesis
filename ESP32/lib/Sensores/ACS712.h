#ifndef ACS712_H
#define ACS712_H

#include <Arduino.h>

// #define ADC_SCALE 1023.0
// #define VREF 5
#define ADC_SCALE 4095.0
#define VREF 3.3
#define DEFAULT_FREQUENCY 50

enum ACS712_type {ACS712_05B, ACS712_20A, ACS712_30A, CUSTOM};

class ACS712 {
public:
	ACS712(ACS712_type type, uint8_t _pin);
	int calibrate();
	void setZeroPoint(int _zero);
	void setSensitivity(float sens);
	float getCurrentDC();
	float getCurrentAC(uint16_t frequency = 50);

private:
	int zero = 1318; // Por divisor resistivo de 5 a 3v3
	float sensitivity;
	uint8_t pin;
};

#endif