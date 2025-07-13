#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>
#include "ScreenSaverScreen.h"

extern TFT_eSPI tft;

void ScreenSaverScreen::onEnter()
{
  Serial.println("Iniciando screensaver...");
}

void ScreenSaverScreen::onUpdate()
{
  // Tu código actual de screensaver aquí
}

void ScreenSaverScreen::onInput(InputMessage::Type input)
{
  ScreenManager::getInstance().changeTo(SCREEN_MENU);
}

void ScreenSaverScreen::onExit()
{
  tft.fillScreen(TFT_BLACK);
}

String ScreenSaverScreen::getName()
{
  return "SCREENSAVER";
}