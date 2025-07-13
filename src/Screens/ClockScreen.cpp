#include "BaseScreen.h"
#include <DisplayManager.h>
#include <Core/ScreenManager.h>
#include "ClockScreen.h"

extern DisplayManager displayManager;
extern NTPClient timeClient;
extern TFT_eSPI tft;

void ClockScreen::onEnter()
{
  mode = NORMAL;
  tft.fillScreen(TFT_BLACK);
}

void ClockScreen::onUpdate()
{
  if (mode == NORMAL)
  {
    displayManager.drawClockPage(timeClient);
  }
  else
  {
    showSetTimeDisplay();
    if (millis() - modeTimeout > 10000)
    {
      mode = NORMAL;
    }
  }
}

void ClockScreen::onInput(InputMessage::Type input)
{
  switch (mode)
  {
  case NORMAL:
    switch (input)
    {
    case InputMessage::BUTTON_SELECT:
      ScreenManager::getInstance().changeTo(SCREEN_MENU);
      break;
    case InputMessage::BUTTON_UP:
      mode = SET_HOUR;
      modeTimeout = millis();
      customHour = timeClient.getHours();
      customMinute = timeClient.getMinutes();
      break;
    }
    break;
  case SET_HOUR:
    switch (input)
    {
    case InputMessage::BUTTON_UP:
      customHour = (customHour + 1) % 24;
      break;
    case InputMessage::BUTTON_DOWN:
      customHour = (customHour - 1 + 24) % 24;
      break;
    case InputMessage::BUTTON_SELECT:
      mode = SET_MINUTE;
      break;
    }
    modeTimeout = millis();
    break;
  case SET_MINUTE:
    switch (input)
    {
    case InputMessage::BUTTON_UP:
      customMinute = (customMinute + 1) % 60;
      break;
    case InputMessage::BUTTON_DOWN:
      customMinute = (customMinute - 1 + 60) % 60;
      break;
    case InputMessage::BUTTON_SELECT:
      mode = NORMAL;
      break;
    }
    modeTimeout = millis();
    break;
  }
}

void ClockScreen::onExit()
{
  // Implementación vacía como en el original
}

String ClockScreen::getName()
{
  return "CLOCK";
}

void ClockScreen::showSetTimeDisplay()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(50, 80);

  if (mode == SET_HOUR)
  {
    tft.print("HORA: ");
    tft.setTextColor(TFT_YELLOW);
    tft.print(String(customHour));
    tft.setTextColor(TFT_WHITE);
    tft.print(":" + String(customMinute));
  }
  else
  {
    tft.print("HORA: " + String(customHour) + ":");
    tft.setTextColor(TFT_YELLOW);
    tft.print(String(customMinute));
  }
}