#include "AnalogHandler.h"

AnalogHandler::AnalogHandler(int xPin, int yPin, int swPin)
{
  this->_xPin = xPin;
  this->_yPin = yPin;
  this->_swPin = swPin;
  this->_lastDebounceTime = 0;
  this->_lastPressTime = 0;
  this->_waitingForSecondPress = false;
  this->_doublePressHandled = false;
  this->_buttonState = HIGH;
  this->_lastButtonState = HIGH;
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
  // Leer el estado actual del botón
  int reading = digitalRead(this->_swPin);

  // Si el estado cambió (debido a ruido o pulsación)
  if (reading != this->_lastButtonState)
  {
    this->_lastDebounceTime = millis(); // Reiniciar el temporizador de debounce
  }

  // Si ha pasado el tiempo de debounce, considerar el estado como válido
  if ((millis() - this->_lastDebounceTime) > DEBOUNCE_DELAY)
  {
    // Si el estado del botón ha cambiado
    if (reading != this->_buttonState)
    {
      this->_buttonState = reading;

      // Si el botón está presionado (LOW)
      if (this->_buttonState == LOW)
      {
        this->_lastButtonState = reading;
        return true;
      }
    }
  }

  this->_lastButtonState = reading;
  return false;
}

bool AnalogHandler::isDoublePressed()
{
  // Si ya se manejó una doble pulsación, no activar `isDoublePressed`
  if (_doublePressHandled)
  {
    _doublePressHandled = false; // Reinicia para futuras llamadas
    return false;
  }

  // Verificar si hay una pulsación válida
  bool buttonState = isPressed();
  unsigned long currentTime = millis();

  if (buttonState && (currentTime - _lastPressTime) > DEBOUNCE_DELAY)
  {
    _lastPressTime = currentTime;

    if (_waitingForSecondPress && (currentTime - _lastPressTime) <= _doublePressInterval)
    {
      _waitingForSecondPress = false;
      _doublePressHandled = true; // Marca que se manejó doble pulsación
      return true;
    }
    else
    {
      _waitingForSecondPress = true;
    }
  }

  // Si ha pasado el tiempo de espera para la segunda pulsación, reiniciar
  if (_waitingForSecondPress && (currentTime - _lastPressTime) > _doublePressInterval)
  {
    _waitingForSecondPress = false;
  }

  return false;
}