#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "Screens/BaseScreen.h"

class ScreenManager
{
private:
  BaseScreen *screens[SCREEN_COUNT];
  volatile ScreenType currentScreen = SCREEN_WIFI_SETUP;
  volatile bool changingScreen = false;

public:
  static ScreenManager &getInstance();

  ScreenManager();
  void changeTo(ScreenType newScreen);
  void handleInput(InputMessage::Type input);
  void update();
  ScreenType getCurrentScreen();
  String getCurrentScreenName();
};

#endif