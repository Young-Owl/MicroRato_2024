#include <Arduino.h>	// AVR RP2040
#include <Wire.h>		// I2C
#include <U8g2lib.h>	// OLED
#include <ADS1X15.h>	// ADC
#include <FS_MX1508.h>	// Motor driver
#include "screen.h"
#include "defines.h"
#include "pid.h"
#include "pio_rotary_encoder.h"
#include "motorControl.h"

/* ------ Pinmodes ------ */
uint8_t inputPins[] = {JOYSTICK_X_PIN, JOYSTICK_Y_PIN, ALERT_ADC2_PIN, ALERT_ADC1_PIN};
uint8_t outputPins[] = {LED_BUILTIN, IR_LEDS_PIN, MOTOR_IN1R_PIN, MOTOR_IN2R_PIN, MOTOR_IN3L_PIN, MOTOR_IN4L_PIN};
uint8_t pullupPins[] = {START_BTN_PIN, STOP_BTN_PIN, JOYSTICK_BUTTON_PIN, ENCODER_A1L_PIN, ENCODER_A2R_PIN, ENCODER_B1L_PIN, ENCODER_B2R_PIN};

uint8_t selectedMenu = 0;
uint8_t led = 0;
volatile uint16_t valADCs[8] = {0,0,0,0,0,0,0,0};

// Initialize static member of class Rotary_encoder
int RotaryEncoder0::rotation0 = 0;
int RotaryEncoder1::rotation1 = 0;

/* ------ Variables ------ */
volatile int posi[] = {0,0};

const int encodersA[] = {ENCODER_A1L_PIN, ENCODER_A2R_PIN};
const int encodersB[] = {ENCODER_B1L_PIN, ENCODER_B2R_PIN};
const int in1[] = {MOTOR_IN1R_PIN, MOTOR_IN3L_PIN};
const int in2[] = {MOTOR_IN2R_PIN, MOTOR_IN4L_PIN};

/* Motor encoders have 7PPR, so 7 pulses per revolution
 * The motor has 500 RPM with reduction ratio of 1:30
 * When using wheel with 67mm diameter, the circumference is 210mm
 */
uint8_t pulsesPerRevolution = 7;										// Pulses per revolution
uint8_t countsPerRevolution = pulsesPerRevolution * 4;					// Counts per revolution
uint8_t reductionRatio = 30;											// Reduction ratio	
uint16_t pulsesPerTurn = pulsesPerRevolution * reductionRatio;
uint16_t countsPerTurn = countsPerRevolution * reductionRatio;
double wheelCircumference = 0.2104867;
uint16_t pulsesPerMeter = round(pulsesPerTurn / wheelCircumference);

double wheelBase = 0.167;
double circumference = 3.14159 * wheelBase;
double formulaBase = circumference / wheelCircumference;
uint16_t pulsesPer90Degree = round((circumference / 4) * pulsesPerMeter);


/* ------ Objects ------ */
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g;

ADS1015 adc1(ADC1_ADDRESS);
ADS1015 adc2(ADC2_ADDRESS);

MX1508 motorA(MOTOR_IN1R_PIN, MOTOR_IN2R_PIN, SLOW_DECAY);
MX1508 motorB(MOTOR_IN3L_PIN, MOTOR_IN4L_PIN, SLOW_DECAY);

RotaryEncoder0 encoderA(ENCODER_A1L_PIN, ENCODER_B1L_PIN);
RotaryEncoder1 encoderB(ENCODER_A2R_PIN, ENCODER_B2R_PIN);

/* ------ Interrupts ------ */
template <int t>
void readEncoder(){
	if(digitalRead(encodersB[t]) == digitalRead(encodersA[t])){
		posi[t]++;
	}
	else{
		posi[t]--;
	}
}

void readIR(){
	while(1){
		for(int i = 0; i < 8; i++){
			if(i < 4){
				valADCs[i] = adc2.readADC(i);
			}
			else{
				valADCs[i] = adc1.readADC(i-4);
			}
		}
		delay(10);
	}
}

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

	/* Start the Serial for printf */
	// Serial.begin(9600);

	/* Start the ADCs to read the IR sensors */
	adc1.begin();			
	adc1.setGain(1);		
	adc1.setDataRate(4);

	adc2.begin();
	adc2.setGain(1);
	adc2.setDataRate(4);

	digitalWrite(LED_BUILTIN, HIGH);

	/* Set the rotation to 0 */
	encoderA.set_rotation0(0);
	encoderB.set_rotation1(0);

	/* With the encoders being taken care by the PIOs,
	 * Use the extra core of the Rasp Pico W to constantly read the IR sensors
	 */

	//attachInterrupt(digitalPinToInterrupt(encodersA[0]), readEncoder<0>, CHANGE);
	//attachInterrupt(digitalPinToInterrupt(encodersA[1]), readEncoder<1>, CHANGE);

	/* Also sets up the I2C for 12 bit reading */
	SetupScreen();
}

void loop() {

	/* Show the initial frame (Custom rasp logo) with the rotating gears */
	ShowStartup();
	delay(200);

	/* Display the menu and wait for the user input via joystick / buttons
	 * This will lock the program until the user has selected an option
	 */
	selectedMenu = ShowMenu();
	u8g.clearBuffer();
	delay(100);

	switch (selectedMenu){
		case CALIBRATION:
			/* Turn on the IR LEDs */
			digitalWrite(IR_LEDS_PIN, HIGH);

			/* Show the calibration screen */
			ShowCalibration();
			u8g.setFont(u8g_font_7x14B);
			u8g.drawStr(30, 60, "Calibrating");
			u8g.sendBuffer();

			/* Read the sensors:
			 * Change the ADC address to the first sensor, read the value and store it
			 * Change the ADC address to the second sensor, read the value and store it
			 */
			

			delay(1000);
			u8g.clearDisplay();

			ShowDebugLedBG();
			while(1){

				// Read the IR sensors here:
				for(int i = 0; i < 8; i++){
					if(i < 4){
						valADCs[i] = adc2.readADC(i);
					}
					else{
						valADCs[i] = adc1.readADC(i-4);
					}
				}
				ShowDebugLedP(valADCs);
				delay(50);

				if(digitalRead(STOP_BTN_PIN) == LOW){
					break;
				}
			}

			break;
		case INITIAL_RUN:
		{
			/* Initial run
			 * For this mode, the micromouse is supposed to not know the maze
			 * 
			 * Initially, the micromouse (after calibrated) will start moving forward,
			 * once a sec or two passes, it will analyze the sensors and decide the next move;
			 * All minor adjustments will be made to the motors to keep the micromouse on track
			 * and the line in the middle of the sensors.
			 * Whenever it reaches an intersection, cross, deadend, turns or any other, it will detect
			 * said event and store it in the memory.
			 * 
			 * This will be made in a loop function, where the micromouse will keep moving until it reaches
			 * the end of the maze.
			 */

			/* Show the initial run screen */
			//ShowInitialRun(); // Once the start button is pressed, the micromouse will start moving.

			/* Turn on the IR LEDs */
			digitalWrite(IR_LEDS_PIN, HIGH);
			
			



			break;
		}
		case PERFECTED_RUN:
			for(int i = 1; i < 10; i++){
				digitalWrite(LED_BUILTIN, LOW);
				motorA.motorGoP(10*i);
				delay(2000);
				digitalWrite(LED_BUILTIN, HIGH);
				delay(50);
			}
			
			motorA.motorBrake();
			motorA.motorStop();
			break;

		default:
			break;
	}

}

