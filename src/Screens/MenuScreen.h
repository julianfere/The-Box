#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include "BaseScreen.h"
#include "Config/Config.h"

class MenuScreen : public BaseScreen
{
private:
  int selection = 0;
  const int totalOptions = 5;
  const char *options[5] = {"Dolar", "Reloj", "Clima", "Screensaver", "Spotify"};
  bool fastScroll = false;
  unsigned long lastInput = 0;

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;

private:
  void updateDisplay();
  void executeSelection();
};

#endif