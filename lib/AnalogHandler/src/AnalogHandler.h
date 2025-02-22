#ifndef ANALOGHANDLER_H
#define ANALOGHANDLER_H

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
  unsigned long _lastDebounceTime;
  unsigned long _lastPressTime;
  bool _waitingForSecondPress;
  bool _doublePressHandled;
  int _buttonState;
  int _lastButtonState;
  static const unsigned long DEBOUNCE_DELAY = 50;        // Tiempo de debounce en ms
  static const unsigned long _doublePressInterval = 300; // Intervalo para doble pulsación en ms
};

#endif