#ifndef PID_H
#define PID_H

float overcomeLoad(float pidOutput);

float pidController(int target, float kp, float ki, float kd, volatile int posi[2]);

#endif