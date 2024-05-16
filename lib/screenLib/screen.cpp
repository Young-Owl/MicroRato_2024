#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "screen.h"
#include "screenIcons.h"

// Define the Joystick pins and threshold
#define START_BTN1_PIN 8
#define STOP_BTN1_PIN 9
#define JOYSTICK_X1_PIN 26
#define JOYSTICK_Y1_PIN 27
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_THRESHOLD 300

// Set up the OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Define the variables
int8_t selectedItem = 0;
int8_t previousItem = 0;
int8_t nextItem = 0;
uint16_t xValue = 0;
uint16_t yValue = 0;
uint8_t previousState = 0;
uint16_t defaultXValue = 0;
uint16_t defaultYValue = 0;
uint8_t buttonState = 0;
float startTimer = 0;
float stopTimer = 0;
uint8_t rotateGear = 0;

void SetupScreen() {
	// Set up the I2C bus
	Wire.begin();

	// Set up SSD1306 display
	u8g.begin();  
	u8g.setFont(u8g_font_7x14);
	u8g.setColorIndex(1);

	// Set up the Joystick module
	pinMode(JOYSTICK_X1_PIN, INPUT);
	pinMode(JOYSTICK_Y1_PIN, INPUT);
	pinMode(START_BTN1_PIN, INPUT_PULLUP);
	pinMode(STOP_BTN1_PIN, INPUT_PULLUP);

	analogReadResolution(12);

	// Get default Joystick position values
	for(int i = 0; i < 10; i++){
		defaultXValue += analogRead(JOYSTICK_X1_PIN);
		defaultYValue += analogRead(JOYSTICK_Y1_PIN);
	}
	defaultXValue = defaultXValue / 10;
	defaultYValue = defaultYValue / 10;

	// Draw the initial screen
	u8g.clearBuffer();
	u8g.drawXBMP(0,0, 128, 64, initialScreenArray[0]);
	u8g.sendBuffer();
}

void ShowStartup(){
	u8g.clearBuffer();
	u8g.drawXBMP(0,0, 128, 64, initialScreenArray[0]);
	u8g.sendBuffer();
	
	while(digitalRead(START_BTN1_PIN) == HIGH){
		if(rotateGear == 0){
			u8g.drawXBMP(52,2, 23, 22, gearArray[0]);
			u8g.sendBuffer();
			rotateGear = 1;
		}
		else{
			u8g.drawXBMP(52,2, 23, 22, gearArray[1]);
			u8g.sendBuffer();
			rotateGear = 0;
		}
		delay(250);
	}
}

uint8_t ShowMenu(){
	selectedItem = 0;
	previousItem = 0;
	nextItem = 0;
	previousState = 0;
	buttonState = 0;

	while(1) {
		xValue = analogRead(JOYSTICK_X1_PIN);	
		yValue = analogRead(JOYSTICK_Y1_PIN);

		if ((xValue < defaultXValue + JOYSTICK_THRESHOLD) && (xValue > defaultXValue - JOYSTICK_THRESHOLD)){
			previousState = 0;
		}

		// Read the joystick
		if ((analogRead(JOYSTICK_X1_PIN) <= JOYSTICK_THRESHOLD) && previousState == 0) {
			selectedItem++;
			previousState = 1;
			if(selectedItem >= menuIconArray_LEN) selectedItem = 0;
		}
		if ((analogRead(JOYSTICK_X1_PIN) >= 4096 - JOYSTICK_THRESHOLD) && previousState == 0) {
			selectedItem--;
			previousState = 1;
			if(selectedItem < 0) selectedItem = menuIconArray_LEN - 1;
		}

		previousItem = selectedItem - 1;
		if(previousItem < 0) previousItem = menuIconArray_LEN - 1;
		
		nextItem = selectedItem + 1;
		if(nextItem >= menuIconArray_LEN) nextItem = 0;

		// Clear the screen
		u8g.clearBuffer();

		u8g.drawXBMP( 0, 22, 128, 20, menuSelectArray[0]);

		// Draw the icons
		u8g.drawXBMP( 2, 3, 16, 16, menuIconArray[previousItem]);
		u8g.drawXBMP( 2, 24, 16, 16, menuIconArray[selectedItem]);
		u8g.drawXBMP( 2, 46, 16, 16, menuIconArray[nextItem]);

		// Draw the text labels
		u8g.setFont(u8g_font_7x14);
		u8g.drawStr( 25, 15, menuTextArray[previousItem]);

		u8g.setFont(u8g_font_7x14B);
		u8g.drawStr( 25, 37, menuTextArray[selectedItem]);

		u8g.setFont(u8g_font_7x14);
		u8g.drawStr( 25, 59, menuTextArray[nextItem]);

		u8g.sendBuffer();

		delay(100);
		
		// If the button is pressed, then display the selected menu item
		if(digitalRead(START_BTN1_PIN) == LOW){
			return selectedItem;
		}
	}
}

void ShowCalibration(){
	u8g.clearDisplay();
	u8g.clearBuffer();
	u8g.drawXBMP(49,1, 30, 30, CalibrateArray[0]);
	u8g.sendBuffer();
}

void ShowDebugLedBG(){
	
	// Draw all Led Icons
	for(int i = 0; i < 8; i++){
		u8g.drawXBMP(4 + (16*i), 4, 14, 14, LedDebugArray[1]);
	}

	// Draw the battery shaped box
	for(int i = 0; i < 8; i++){
		u8g.drawXBMP(6 + (16*i), 24, 11, 34, LedDebugArray[0]);
	} 
	u8g.sendBuffer();
}

void ShowDebugLedP(volatile uint16_t valADC[8]){
	/* Draw the percentage pills based on the ADC values
	 * The ADC values are between 0 and 4095, each pill will be 10% of 4096, so in total theres 10 pills
	 * for each sensor. The ADC values need to be calculated and grant a percentage value.
	 * According to that percentage, draw the needed pills.
	 */
	for(int i = 0; i < 8; i++){
		uint8_t percentage = (valADC[i] * 100) / 1600;
		uint8_t pills = percentage / 10;
		u8g.drawXBMP(8 + (16*i), 26, 6, 30, PillArray[pills]);
	}
	u8g.sendBuffer();

}