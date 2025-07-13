#ifndef CLOCK_SCREEN_H
#define CLOCK_SCREEN_H

#include "BaseScreen.h"
#include <DisplayManager.h>
#include <Core/ScreenManager.h>
#include <NTPClient.h>
#include <TFT_eSPI.h>

extern DisplayManager displayManager;
extern NTPClient timeClient;
extern TFT_eSPI tft;

class ClockScreen : public BaseScreen
{
private:
  enum ClockMode
  {
    NORMAL,
    SET_HOUR,
    SET_MINUTE
  } mode = NORMAL;

  int customHour = 0, customMinute = 0;
  unsigned long modeTimeout = 0;

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;

private:
  void showSetTimeDisplay();
};

#endif