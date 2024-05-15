#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>

/**
 * @brief Set up the OLED Screen
 * 
 * @return void
*/
void SetupScreen();

void ShowStartup();

uint8_t ShowMenu();

void ShowCalibration();

void ShowDebugLedBG();

void ShowDebugLedP(uint16_t valADC[8]);

#endif