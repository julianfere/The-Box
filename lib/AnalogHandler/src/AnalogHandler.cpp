#include <Arduino.h>
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
  return digitalRead(this->_swPin) == LOW;
}
