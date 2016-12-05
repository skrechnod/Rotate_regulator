#pragma once
enum FreqMeasureState_type
{
	Start,
	end,
	timeout
};
enum state {
	STOP = 0,
	FWD,
	RWD,
	StartFWD,
	StartRWD,
	Breakes,
	Error,
	INIT
};



class PIReg
{
public:
	PIReg();
	virtual ~PIReg();
	unsigned char anglReg(long int err);
	void apply(unsigned long int time);
	unsigned int RegState(state st);
	//unsigned int RegState();
	unsigned int Time;
	FreqMeasureState_type FreqState;
private:
	float KP;
	float KI;
	double Sum;
	const unsigned int UstPeriod = 2000;  //Период в Микросекундах
};

