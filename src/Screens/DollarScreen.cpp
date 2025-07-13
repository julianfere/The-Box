#include <DollarService.h>
#include "Screens/BaseScreen.h"
#include <Core/ScreenManager.h>
#include "DollarScreen.h"

extern DollarService dollarService;

void DollarScreen::onEnter()
{
  if (millis() - lastUpdate > 300000 || lastUpdate == 0)
  {
    cachedData = dollarService.getCurrent();
    lastUpdate = millis();
  }

  UIMessage msg = UIMessage::createDollar(cachedData);

  // CRÍTICO: No bloquear si la cola está llena
  if (xQueueSend(uiQueue, &msg, 0) != pdTRUE)
  {
    Serial.println("⚠️ Cola UI llena, dollar update ignorado");
  }
}

void DollarScreen::onUpdate()
{
  // Implementación vacía
}

void DollarScreen::onInput(InputMessage::Type input)
{
  if (input == InputMessage::BUTTON_SELECT)
  {
    ScreenManager::getInstance().changeTo(SCREEN_MENU);
  }
}

void DollarScreen::onExit()
{
  // Implementación vacía
}

String DollarScreen::getName()
{
  return "DOLLAR";
}