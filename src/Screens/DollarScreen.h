#ifndef DOLLAR_SCREEN_H
#define DOLLAR_SCREEN_H

#include <DollarService.h>
#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>

extern DollarService dollarService;

class DollarScreen : public BaseScreen
{
private:
  unsigned long lastUpdate = 0;
  DollarInfo cachedData;

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;
};

#endif // DOLLAR_SCREEN_H