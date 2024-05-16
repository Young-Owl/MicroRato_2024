#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

float overcomeLoad(float pidOutput);
void rotate90Right(MX1508 motorA, MX1508 motorB, int posi[], int pulsesPer90Degree);
void moveMotor(int u, int motor, MX1508 motorA, MX1508 motorB);

#endif