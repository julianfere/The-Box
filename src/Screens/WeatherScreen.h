#ifndef WEATHER_SCREEN_H
#define WEATHER_SCREEN_H

#include <WeatherService.h>
#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>
#include <TFT_eSPI.h>

extern WeatherService weatherService;
extern TFT_eSPI tft;

class WeatherScreen : public BaseScreen
{
private:
  unsigned long lastUpdate = 0;
  WeatherInfo cachedData;
  bool detailMode = false;
  int selectedDetail = 0;
  const char *details[4] = {"Temperatura", "Humedad", "Lluvia", "Estado"};

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;

private:
  void updateDisplay();
  void showDetailMode();
};

#endif // WEATHER_SCREEN_H