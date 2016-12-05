#pragma once

#include "Arduino.h"
class Filtred_digital_read
{
public:
	Filtred_digital_read(uint8_t pin);
	~Filtred_digital_read();
	int read();
private:
	const uint8_t comp = 5;
	uint8_t currSample;
	uint8_t pin;
	int prevstate;
	
};

