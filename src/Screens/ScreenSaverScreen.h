#ifndef SCREENSAVER_SCREEN_H
#define SCREENSAVER_SCREEN_H

#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

class ScreenSaverScreen : public BaseScreen
{
public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;
};

#endif // SCREENSAVER_SCREEN_H