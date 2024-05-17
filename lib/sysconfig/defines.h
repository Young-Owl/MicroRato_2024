#ifndef DEFINES_H
#define DEFINES_H

/* ------- Menu options ------- */
#define CALIBRATION     0
#define INITIAL_RUN     1
#define PERFECTED_RUN   2

/* ------ Pinout defines ------ */
// UI Module
#define START_BTN_PIN   8
#define STOP_BTN_PIN    9
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_X_PIN  26
#define JOYSTICK_Y_PIN  27

// SENSOR Module
#define ADC1_ADDRESS    0x49
#define ADC2_ADDRESS    0x48
#define ALERT_ADC2_PIN  14
#define ALERT_ADC1_PIN  15
#define IR_LEDS_PIN     21
#define IR_THRESHOLD    800

// MOTOR Module
#define NUM_MOTORS      2
#define ENCODER_A1L_PIN 19
#define ENCODER_B1L_PIN 20
#define ENCODER_A2R_PIN 6
#define ENCODER_B2R_PIN 7
#define MOTOR_IN1R_PIN  10
#define MOTOR_IN2R_PIN  11
#define MOTOR_IN3L_PIN  12
#define MOTOR_IN4L_PIN  13

// MOTOR Control
#define MIN_PWM                 20                           // Percentage
#define MAX_PWM                 90                          // Percentage
#define DEBOUNCE_TIME           5                           // uS debounce time
#define MAX_RPM                 500           
#define COUNTS_PER_TURN         840                         // 7PPR * 30 * 4
#define MAX_TURNS_PER_SECOND    (double)(MAX_RPM / 60) * MAX_PWM    // 500/60 * 90%
#define MAX_METERS_PER_SECOND    1.6

#endif