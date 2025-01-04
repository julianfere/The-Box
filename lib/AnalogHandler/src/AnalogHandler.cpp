#include "AnalogHandler.h"

AnalogHandler::AnalogHandler(int xPin, int yPin, int swPin)
{
  this->_xPin = xPin;
  this->_yPin = yPin;
  this->_swPin = swPin;
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
  // Si ya se manejó una doble pulsación, no activar `isPressed`
  if (_doublePressHandled)
  {
    _doublePressHandled = false; // Reinicia para futuras llamadas
    return false;
  }
  return digitalRead(this->_swPin) == LOW;
}

bool AnalogHandler::isDoublePressed()
{
  bool buttonState = isPressed(); // Usa la lógica de `isPressed`
  unsigned long currentTime = millis();

  if (buttonState && currentTime - _debounceTime > 50)
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
