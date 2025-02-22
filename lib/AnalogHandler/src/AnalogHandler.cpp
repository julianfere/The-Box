#include "AnalogHandler.h"

AnalogHandler::AnalogHandler(int xPin, int yPin, int swPin)
{
  this->_xPin = xPin;
  this->_yPin = yPin;
  this->_swPin = swPin;
  this->_lastPressTime = 0;
  this->_waitingForSecondPress = false;
  this->_doublePressHandled = false;
  this->_debounceTime = 0;
}

void AnalogHandler::setup()
{
  pinMode(this->_swPin, INPUT_PULLUP);
}

bool AnalogHandler::isUp()
{
  return analogRead(this->_yPin) > 3000;
}

bool AnalogHandler::isDown()
{
  return analogRead(this->_yPin) < 1000;
}

bool AnalogHandler::isLeft()
{
  return analogRead(this->_xPin) > 3000;
}

bool AnalogHandler::isRight()
{
  return analogRead(this->_xPin) < 1000;
}

bool AnalogHandler::isPressed()
{
  unsigned long currentTime = millis();
  if (currentTime - _debounceTime > 50) // 50ms de debounce
  {
    bool buttonState = digitalRead(this->_swPin) == LOW;
    if (buttonState)
    {
      _debounceTime = currentTime;
      return true;
    }
  }
  return false;
}

bool AnalogHandler::isDoublePressed()
{
  unsigned long currentTime = millis();
  bool buttonState = digitalRead(this->_swPin) == LOW;

  if (buttonState && currentTime - _debounceTime > 50) // 50ms de debounce
  {
    _debounceTime = currentTime;

    if (_waitingForSecondPress && currentTime - _lastPressTime <= _doublePressInterval)
    {
      _waitingForSecondPress = false;
      _doublePressHandled = true; // Marca que se manejó doble pulsación
      return true;
    }
    else
    {
      _lastPressTime = currentTime;
      _waitingForSecondPress = true;
    }
  }

  if (_waitingForSecondPress && currentTime - _lastPressTime > _doublePressInterval)
  {
    _waitingForSecondPress = false;
  }

  return false;
}