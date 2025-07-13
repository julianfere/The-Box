#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "UIMessage.h"
#include "Config/Pins.h"
#include "Config/Config.h"

class ButtonHandler
{
private:
  unsigned long lastPress = 0;
  unsigned long lastRelease = 0;
  InputMessage::Type lastButton = InputMessage::BUTTON_BACK;
  bool buttonPressed = false;

public:
  void setup();
  void update();
  InputMessage::Type readButtonType();
  void debugADC();
};

extern ButtonHandler buttonHandler;

#endif