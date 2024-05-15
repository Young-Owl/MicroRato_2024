#include <Arduino.h>	// AVR RP2040
#include <Wire.h>		// I2C
#include <U8g2lib.h>	// OLED
#include <ADS1X15.h>	// ADC
#include <FS_MX1508.h>	// Motor driver
#include "screen.h"

/* ------- Menu options ------- */
#define CALIBRATION 0
#define INITIAL_RUN 1
#define PERFECTED_RUN 2

/* ------ Pinout defines ------ */
// UI Module
#define START_BTN_PIN 8
#define STOP_BTN_PIN 9
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27

// SENSOR Module
#define ADC1_ADDRESS 0x49
#define ADC2_ADDRESS 0x48
#define ALERT_ADC2_PIN 14
#define ALERT_ADC1_PIN 15
#define IR_LEDS_PIN 21

// MOTOR Module
#define ENCODER_A1_PIN 19
#define ENCODER_A2_PIN 20
#define ENCODER_B1_PIN 6
#define ENCODER_B2_PIN 7
#define MOTOR_R1_PIN 10
#define MOTOR_R2_PIN 11
#define MOTOR_L1_PIN 12
#define MOTOR_L2_PIN 13

/* ------ Pinmodes ------ */
uint8_t inputPins[] = {JOYSTICK_X_PIN, JOYSTICK_Y_PIN, ALERT_ADC2_PIN, ALERT_ADC1_PIN, ENCODER_A1_PIN, ENCODER_A2_PIN, ENCODER_B1_PIN, ENCODER_B2_PIN};
uint8_t outputPins[] = {LED_BUILTIN, IR_LEDS_PIN, MOTOR_R1_PIN, MOTOR_R2_PIN, MOTOR_L1_PIN, MOTOR_L2_PIN};
uint8_t pullupPins[] = {START_BTN_PIN, STOP_BTN_PIN, JOYSTICK_BUTTON_PIN};

uint8_t selectedMenu = NULL;
uint8_t led = 0;
uint16_t a0_ADC1 = 0, a1_ADC1 = 0, a2_ADC1 = 0, a3_ADC1 = 0;
uint16_t a0_ADC2 = 0, a1_ADC2 = 0, a2_ADC2 = 0, a3_ADC2 = 0;
uint16_t valADCs[8] = {0};

/* ------ Objects ------ */
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g;

ADS1015 adc1(ADC1_ADDRESS);
ADS1015 adc2(ADC2_ADDRESS);

MX1508 motorA(MOTOR_R1_PIN, MOTOR_R2_PIN, SLOW_DECAY);
MX1508 motorB(MOTOR_L2_PIN, MOTOR_L1_PIN, SLOW_DECAY);
 
/* ------ Setup ------ */
void setup() {

	/* Setup the pinmodes */
	for (uint8_t i = 0; i < sizeof(inputPins); i++){
		pinMode(inputPins[i], INPUT);
	}
	for (uint8_t i = 0; i < sizeof(outputPins); i++){
		pinMode(outputPins[i], OUTPUT);
	}
	for (uint8_t i = 0; i < sizeof(pullupPins); i++){
		pinMode(pullupPins[i], INPUT_PULLUP);
	}

	Serial.begin(9600);

	adc1.begin();
	adc1.setGain(1);
	adc1.setDataRate(4);

	adc2.begin();
	adc2.setGain(1);
	adc2.setDataRate(4);

	digitalWrite(LED_BUILTIN, LOW);

	/* Also sets up the I2C for 12 bit reading */
	SetupScreen();
}

void loop() {

	ShowStartup();

	/* Display the menu and wait for the user input via joystick / buttons
	 * This will lock the program until the user has selected an option
	 */
	selectedMenu = ShowMenu();
	u8g.clearBuffer();
	delay(100);

	switch (selectedMenu){
		case CALIBRATION:
			/* Show the calibration screen */
			ShowCalibration();

			/* Turn on the IR LEDs */
			digitalWrite(IR_LEDS_PIN, HIGH);

			/* Read the sensors:
			 * Change the ADC address to the first sensor, read the value and store it
			 * Change the ADC address to the second sensor, read the value and store it
			 */
			a0_ADC1 = adc1.readADC(0);
			a1_ADC1 = adc1.readADC(1);
			a2_ADC1 = adc1.readADC(2);
			a3_ADC1 = adc1.readADC(3);
			a0_ADC2 = adc2.readADC(0);
			a1_ADC2 = adc2.readADC(1);
			a2_ADC2 = adc2.readADC(2);
			a3_ADC2 = adc2.readADC(3);

			/* Show the calibration screen */
			ShowCalibration();
			u8g.setFont(u8g_font_7x14B);
			u8g.drawStr( 10, 50, "Calibrating");
			u8g.sendBuffer();

			delay(2000);
			u8g.clearDisplay();

			ShowDebugLedBG();
			while(1){
				a0_ADC1 = adc1.readADC(0);
				a1_ADC1 = adc1.readADC(1);
				a2_ADC1 = adc1.readADC(2);
				a3_ADC1 = adc1.readADC(3);	
				a0_ADC2 = adc2.readADC(0);
				a1_ADC2 = adc2.readADC(1);
				a2_ADC2 = adc2.readADC(2);
				a3_ADC2 = adc2.readADC(3);

				valADCs[0] = a0_ADC2;
				valADCs[1] = a1_ADC2;
				valADCs[2] = a2_ADC2;
				valADCs[3] = a3_ADC2;
				valADCs[4] = a0_ADC1;
				valADCs[5] = a1_ADC1;
				valADCs[6] = a2_ADC1;
				valADCs[7] = a3_ADC1;

				ShowDebugLedP(valADCs);

				if(digitalRead(STOP_BTN_PIN) == LOW){
					break;
				}
				
			}

			break;
		case INITIAL_RUN:
			led = !led;
			digitalWrite(LED_BUILTIN, led);

			motorB.motorGo(20);
			delay(100);
			motorB.motorGo(40);
			delay(100);
			motorB.motorGo(80);
			delay(100);
			motorB.motorGo(150);
			delay(100);
			motorB.motorGo(255);

			/* while(1){
				if(digitalRead(STOP_BTN_PIN) == LOW){
					motorB.motorStop();
					delay(1000);
					break;
				}
			} */

			break;
		default:
			break;
	}

	selectedMenu = NULL;

}