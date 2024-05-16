#ifndef PID_H
#define PID_H

void pidController(long target[2], float kp, float ki, float kd, float deltaT, volatile int posi[2], float* u1, float* u2);

#endif