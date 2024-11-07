/*
  AnalogHandler.h - Library for easy handling of analog inputs.
*/
#ifndef AnalogHandler_h
#define AnalogHandler_h
#include <Arduino.h>

class AnalogHandler
{
public:
  AnalogHandler(int xPin, int yPin, int swPin);
  void setup();
  bool isUp();
  bool isDown();
  bool isLeft();
  bool isRight();
  bool isPressed();

private:
  int _xPin;
  int _yPin;
  int _swPin;
};
#endif