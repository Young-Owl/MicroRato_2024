#include "defines.h"
#include <stdlib.h>
#include <Arduino.h>

long prevT = 0;
float eprev = 0;
float eintegral = 0;

float overcomeLoad(float pidOutput){
	if(fabs(pidOutput) < MIN_PWM){
		if(pidOutput > 0){
			pidOutput = MIN_PWM;
		}
		else{
			pidOutput = -MIN_PWM;
		}
	}
	return pidOutput;
}

float pidController(int target, float kp, float ki, float kd, volatile int posi[2]){
	long currentT = micros();
	float deltaT = ((float)(currentT - prevT)) / 1.0e6;

	int e = posi[0] - target;
	float eDerivative = (e - eprev) / deltaT;
	eintegral += e * deltaT;

	float u = (kp * e) + (ki * eintegral) + (kd * eDerivative);

	prevT = currentT;
	eprev = e;

	return u;
}