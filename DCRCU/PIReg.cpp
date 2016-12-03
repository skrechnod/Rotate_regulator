#include "PIReg.h"
#include "HardwareSerial.h"


PIReg::PIReg()
{
	KI = 0.01;
	KP = 0.01;
	Sum = 0;
	FreqState = end;
	Time = 32000;
}


PIReg::~PIReg()
{

}

unsigned char PIReg::anglReg(long int err) {
	float		ErrNorm;
	int			alpha;
	const float Kp = 0.001;
	const float Ki = 0.003;       //Коэффициент инерциального звена (скорость разгона)
	const float amax = 255;			// Максимальный ШИМ
	const float amin = 20;       // Минимальный ШИМ

	float MaxErr = 2500;
	if (err > MaxErr) { // _IQ(1.0)
		err = MaxErr;
	}
	else if (err < -MaxErr) {
		err = -MaxErr;
	}

	ErrNorm = err  ;		// регулятор работает в единицах угла
	alpha = (int)(Sum + Kp* ErrNorm);

	if (alpha < amin) {
		alpha = amin;
	}
	else if (alpha > amax) {
		alpha = amax;
	}	
	Serial.println("err ");
	Serial.println(err);
	Sum += (Ki* ErrNorm);
	Serial.println("Sum ");
	Serial.println(Sum);
	if (Sum < amin) {
		Sum = amin;
	}
	else if (Sum > amax) {
		Sum = amax;
	}

	return	(unsigned char)alpha;
}



void PIReg::apply(unsigned long int time)
{
	const unsigned char max = 40;
	float error;
	float out;
	error = (float)(-time);
	if (error >(float)max)
	{
		error = max;
	}
	if (error <(float) -max)
	{
		error = -max;
	}
	out = Sum + (unsigned int)(error * KP);
	Sum += (unsigned int)(error * KI);

	if (out > max)
	{
		out = max;
	}
	if (out < 0)
	{
		out = 0;
	}
	if (Sum > max)
	{
		Sum = max;
	}
	if (Sum < 0)
	{
		Sum = 0;
	}
}

unsigned int PIReg::RegState(state st)
{
	switch (st)
	{
	case Error:
	case STOP:{
		Sum = 0;
		break;
	}
	case FWD: {
		anglReg((int)(this->Time- UstPeriod));
		//Serial.print("err123 ");
		//Serial.println((int)(this->Time - UstPeriod));
		break;
	}
	case RWD: {
		anglReg((int)(this->Time- UstPeriod ));
		break;
	}
	default:
		break;
	}
	return (unsigned char) Sum;
}
