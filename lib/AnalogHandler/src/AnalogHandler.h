
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
  bool isDoublePressed();

private:
  int _xPin;
  int _yPin;
  int _swPin;
  unsigned long _lastPressTime = 0;
  unsigned long _debounceTime = 0;
  unsigned long _doublePressInterval = 500; // Intervalo máximo entre pulsaciones (ms)
  bool _waitingForSecondPress = false;
  bool _doublePressHandled = false; // Evita colisión entre métodos
};
#endif
