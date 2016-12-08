#include "Filtred_digital_read.h"






Filtred_digital_read::Filtred_digital_read(uint8_t _pin): pin(_pin)
{
	pinMode(_pin, INPUT_PULLUP);
	currSample = 0;
	prevstate = LOW;
}

Filtred_digital_read::~Filtred_digital_read()
{
}

int Filtred_digital_read::read()
{
	if (digitalRead(pin) == HIGH) {
		if (currSample < 5)
			currSample++;
	}
	else {
		if (currSample > 0) {
			currSample--;
		}
	}
	if (currSample == 5) {
		prevstate = HIGH;
	}
	if (currSample == 0) {
		prevstate = LOW;
	}
	return prevstate;
}
