#include <WeatherService.h>
#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>
#include "WeatherScreen.h"

extern WeatherService weatherService;
extern TFT_eSPI tft;

void WeatherScreen::onEnter()
{
  detailMode = false;
  if (millis() - lastUpdate > 300000 || lastUpdate == 0)
  {
    cachedData = weatherService.getCurrent();
    lastUpdate = millis();
  }
  updateDisplay();
}

void WeatherScreen::onUpdate()
{
  // Implementación vacía
}

void WeatherScreen::onInput(InputMessage::Type input)
{
  if (!detailMode)
  {
    switch (input)
    {
    case InputMessage::BUTTON_SELECT:
      ScreenManager::getInstance().changeTo(SCREEN_MENU);
      break;
    case InputMessage::BUTTON_UP:
      detailMode = true;
      selectedDetail = 0;
      showDetailMode();
      break;
    case InputMessage::BUTTON_DOWN:
      cachedData = weatherService.getCurrent();
      lastUpdate = millis();
      updateDisplay();
      break;
    }
  }
  else
  {
    switch (input)
    {
    case InputMessage::BUTTON_UP:
      selectedDetail = (selectedDetail - 1 + 4) % 4;
      showDetailMode();
      break;
    case InputMessage::BUTTON_DOWN:
      selectedDetail = (selectedDetail + 1) % 4;
      showDetailMode();
      break;
    case InputMessage::BUTTON_SELECT:
      detailMode = false;
      updateDisplay();
      break;
    }
  }
}

void WeatherScreen::onExit()
{
  // Implementación vacía
}

String WeatherScreen::getName()
{
  return "WEATHER";
}

void WeatherScreen::updateDisplay()
{
  UIMessage msg = UIMessage::createWeather(cachedData);

  // CRÍTICO: No bloquear si la cola está llena
  if (xQueueSend(uiQueue, &msg, 0) != pdTRUE)
  {
    Serial.println("⚠️ Cola UI llena, weather update ignorado");
  }
}

void WeatherScreen::showDetailMode()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 60);
  tft.print(details[selectedDetail]);

  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3);
  tft.setCursor(20, 100);

  switch (selectedDetail)
  {
  case 0:
    tft.print(String(cachedData.temp) + "°C");
    break;
  case 1:
    tft.print(String(cachedData.hum) + "%");
    break;
  case 2:
    tft.print(String(cachedData.rain) + "%");
    break;
  case 3:
    tft.setTextSize(2);
    tft.print(cachedData.status);
    break;
  }
}