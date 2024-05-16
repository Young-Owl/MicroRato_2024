#include <stdlib.h>
#include <Arduino.h>

long prevT = 0;
float eprev[2] = {0,0};
float eintegral[2] = {0,0};

void pidController(long target[2], float kp, float ki, float kd, float deltaT, volatile int posi[2], float* u1, float* u2){
	int e[2];
	float eDerivative[2];
	float u[2];

	e[0] = posi[0] - target[0];
	e[1] = posi[1] - target[1];

	eDerivative[0] = (e[0] - eprev[0]) / deltaT;
	eintegral[0] += e[0] * deltaT;

	eDerivative[1] = (e[1] - eprev[1]) / deltaT;
	eintegral[1] += e[1] * deltaT;

	*u1 = kp * e[0] + ki * eintegral[0] + kd * eDerivative[0];
	*u2 = kp * e[1] + ki * eintegral[1] + kd * eDerivative[1];

	eprev[0] = e[0];
	eprev[1] = e[1];
}