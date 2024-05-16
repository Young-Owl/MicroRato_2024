#include <FS_MX1508.h>    // Motor driver
#include "pid.h"
#include "defines.h"
#include "motorControl.h"

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

void rotate90Right(MX1508 motorA, MX1508 motorB, int posi[], int pulsesPer90Degree){
	int lastPosi[] = {posi[0], posi[1]};
	int currPosi[] = {posi[0], posi[1]};
	motorA.motorGoP(75);
	motorB.motorGoP(75);

	while(1){
		currPosi[0] = abs(posi[0] - lastPosi[0]);
		currPosi[1] = abs(posi[1] - lastPosi[1]);
		if(currPosi[0] >= pulsesPer90Degree && currPosi[1] >= pulsesPer90Degree){
			motorA.motorBrake();
			motorB.motorBrake();
			break;
		}
	}
}

void moveMotor(int u, int motor, MX1508 motorA, MX1508 motorB){
	float speed = u;
	if(speed > 90){
		speed = 90;
	}
	else if(speed < -90){
		speed = -90;
	}

	overcomeLoad(speed);

	if(motor == 1){
		motorA.motorGoP(speed);
	}
	else{
		motorB.motorGoP(speed);
	}
}