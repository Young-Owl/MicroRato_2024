#ifndef SIMPLEPRESS_H
#define SIMPLEPRESS_H

#include <Arduino.h>

class SimplePress{
  public:
    SimplePress(int _pin, uint32_t _pressInterval, uint32_t _debouncePeriod = 200);
    bool begin();
    int8_t pressed();

  private:
    byte pressCount;
    byte lastState;
    byte pin;
    uint32_t lastMillis;
    uint32_t debouncePeriod;
    uint32_t pressInterval;
};

SimplePress::SimplePress(int _pin, uint32_t _pressInterval, uint32_t _debouncePeriod)
{
  pin = _pin;
  debouncePeriod = _debouncePeriod;
  pressInterval = _pressInterval;
}

bool SimplePress::begin()
{
  pinMode(pin, INPUT_PULLUP);
  lastState = HIGH;
  return true;
}

int8_t SimplePress::pressed()
{
  byte nowState = digitalRead(pin);
  if(nowState != lastState)
  {
    if(millis() - lastMillis < debouncePeriod) return 0;
    if(nowState == LOW)
    {
      lastMillis = millis();
      pressCount++;
    }
    else 
    {
      if (millis() - lastMillis > pressInterval) // a long press
      {
        lastState = nowState;
        pressCount = 0;
        return -1;
      }
    }
  }
  if(pressCount != 0)
  {
    if(millis() - lastMillis > pressInterval and nowState == HIGH)
    {
      int presses = pressCount;
      pressCount = 0;
      return presses;
    }
  }
  lastState = nowState;
  return 0;
}

#endif