#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include <Arduino.h>
#include "Core/UIMessage.h"

class BaseScreen
{
public:
  virtual void onEnter() = 0;
  virtual void onUpdate() = 0;
  virtual void onInput(InputMessage::Type input) = 0;
  virtual void onExit() = 0;
  virtual String getName() = 0;
  virtual ~BaseScreen() = default;
};

enum ScreenType
{
  SCREEN_WIFI_SETUP = 0,
  SCREEN_MENU,
  SCREEN_CLOCK,
  SCREEN_WEATHER,
  SCREEN_DOLLAR,
  SCREEN_SPOTIFY,
  SCREEN_SCREENSAVER,
  SCREEN_COUNT
};

#endif