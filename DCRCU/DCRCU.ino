/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
#include "PIReg.h"

PIReg MainReg;
volatile unsigned long int Time;
bool meas_flag = false;

void Timer_ISR() {
	switch (MainReg.FreqState)
	{
	case Start: {
		Time = micros();
		MainReg.FreqState = end;
		break;
		}
	case end: {
		Time = micros()-Time;
		MainReg.FreqState = Start;
		if (Time > 32000) Time = 32000;
		MainReg.Time =(int) Time;
		meas_flag = true;
		break;
	}
	default:
		break;
	}
}

/*enum state {
	STOP = 0,
	FWD,
	RWD
};
*/


int input_up = 9;
int input_down = 8;
int output_up = 4;
int output_down = 12;
int PWM_out_up = 3;
int PWM_out_down = 5;
int break_down = 6;
int break_up = 7;
int alarm = 10;
unsigned long previousMillis = 0;       
const long interval = 10;
state st = STOP;
int startTime = 0;
int attemp = 0;
unsigned long  startAttempTime = 0;
unsigned long  endAttempTime = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
	pinMode(input_down, INPUT_PULLUP);
	pinMode(input_up, INPUT_PULLUP);
	pinMode(output_down, OUTPUT);
	pinMode(output_up, OUTPUT);
	pinMode(PWM_out_up, OUTPUT);
	pinMode(PWM_out_down, OUTPUT);
	pinMode(break_down, INPUT_PULLUP);
	pinMode(break_up, INPUT_PULLUP);
	pinMode(2, INPUT_PULLUP);
	pinMode(alarm, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(2), Timer_ISR, RISING);
	Serial.begin(256000);
}

// the loop function runs over and over again forever
void loop() {

	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		// save the last time you blinked the LED
		previousMillis = currentMillis;

		switch (st)
		{
		case STOP: {
			startTime = 0;
			Time = 32000;
			MainReg.Time = (int)Time;
			MainReg.FreqState = end;
			digitalWrite(output_down, LOW);
			digitalWrite(output_up, LOW);
			analogWrite(PWM_out_up, MainReg.RegState(st));
			analogWrite(PWM_out_down, MainReg.RegState(st));
			attemp = 0;
			if (digitalRead(input_down) && (digitalRead(break_down)))
			{
				st = StartRWD;
				digitalWrite(output_down, HIGH);
				attemp = 0;
			}
			else if (digitalRead(input_up) && (digitalRead(break_up))) {
				st = StartFWD;
				digitalWrite(output_up, HIGH);
				attemp = 0;
			}
			break;
		}
		case FWD: {
			if (digitalRead(input_up)==LOW || (digitalRead(break_up) == LOW)) {
				if (digitalRead(break_up) == LOW)
					st = Breakes;
				else
					st = STOP;
				digitalWrite(output_down, LOW);
				digitalWrite(output_up, LOW);
				analogWrite(PWM_out_up, 0);
				analogWrite(PWM_out_down, 0);
				break;
			}
			if (meas_flag == false) {
				st = StartFWD;
				attemp++;
				break;
			}
			meas_flag = false;
			//digitalWrite(output_down, LOW);
			int temp = MainReg.RegState(st);
			//digitalWrite(output_up, HIGH);
			analogWrite(PWM_out_up, temp);
			startTime = 0;
			//Serial.print("PWm ");
			//Serial.println(temp);

			break;
		}
		case RWD: {
			if (digitalRead(input_down)==LOW || (digitalRead(break_down)==LOW)) {
				if (digitalRead(break_down) == LOW)
					st = Breakes;
				else
					st = STOP;
				digitalWrite(output_down, LOW);
				digitalWrite(output_up, LOW);
				analogWrite(PWM_out_up, 0);
				analogWrite(PWM_out_down, 0);
				break;
			}
			if (meas_flag == false) {
				st = StartRWD;
				attemp++;
				break;
			}
			startTime = 0;
			meas_flag = false;
			//digitalWrite(output_down, HIGH);
			//digitalWrite(output_up, LOW);
			int temp = MainReg.RegState(st);
			analogWrite(PWM_out_down, temp);
			//Serial.print("PWm ");
			//Serial.println(temp);

			break;
		}
				  //////////////////////////////////////////
				  //
				  //		Состояния разгона 
				  //
				  ///////////////////////////////////////////
		case StartFWD:{
			if (digitalRead(input_up)==LOW || (digitalRead(break_up)==LOW)) {
				if (digitalRead(break_up) == LOW)
					st = Breakes;
				else
					st = STOP;
				digitalWrite(output_down, LOW);
				digitalWrite(output_up, LOW);
				analogWrite(PWM_out_up, 0);
				analogWrite(PWM_out_down, 0);
				break;
			}
			//digitalWrite(output_down, LOW);
			int temp = MainReg.RegState(st);
			
			analogWrite(PWM_out_up, 200);		//// величина ШИМ на этапе разгона
			startTime++;
			if (startTime>4) {
				st = FWD;
			}
			break;
		}
		case StartRWD: {
			if (digitalRead(input_down)==LOW || (digitalRead(break_down)== LOW)) {
				if (digitalRead(break_down) == LOW)
					st = Breakes;
				else
					st = STOP;
				digitalWrite(output_down, LOW);
				digitalWrite(output_up, LOW);
				analogWrite(PWM_out_up, 0);
				analogWrite(PWM_out_down, 0);
				break;
			}
			//digitalWrite(output_down, HIGH);
			//digitalWrite(output_up, LOW);
			int temp = MainReg.RegState(st);
			analogWrite(PWM_out_down, 200);   /////// величина ШИМ на этапе разгона
			//attemp++;
			startTime++;
			if (startTime>4) {				/////// Значение нужно умножить на дискретность системы 5мс
				st = RWD;
			}
			break;
		}
		case Breakes: {
			if (digitalRead(input_down) == LOW && digitalRead(input_up) == LOW) {
				st = STOP;
			}
			break;
		}
		case Error:
		{
			digitalWrite(output_down, LOW);
			digitalWrite(output_up, LOW);
			analogWrite(PWM_out_up, MainReg.RegState(st));
			analogWrite(PWM_out_down, MainReg.RegState(st));
			digitalWrite(alarm, LOW);
			break;
		}
		default:
			break;
		}
		if (digitalRead(alarm) == LOW) {
			//st = Error;
		}
		if (attemp > 0) {
			if (attemp == 1) {
				startAttempTime = millis();
				attemp++;
			}else{
				if (attemp > 20) {
					st = Error;
					pinMode(alarm, OUTPUT);
				}else if (millis() - startAttempTime > 500) {
					attemp = 0;
				}
			}
		}
		Serial.print("st ");
	Serial.println(st);
	Serial.println(MainReg.Time);
	}
	
}
