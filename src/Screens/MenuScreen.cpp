#include "MenuScreen.h"
#include "Core/ScreenManager.h"

void MenuScreen::onEnter()
{
  fastScroll = false;
  updateDisplay();
}

void MenuScreen::onUpdate()
{
  if (fastScroll && millis() - lastInput > 1000)
  {
    fastScroll = false;
  }
}

void MenuScreen::onInput(InputMessage::Type input)
{
  lastInput = millis();

  if (millis() - lastInput < 300)
  {
    fastScroll = true;
  }

  switch (input)
  {
  case InputMessage::BUTTON_UP:
    if (fastScroll)
    {
      selection = (selection - 2 + totalOptions) % totalOptions;
    }
    else
    {
      selection = (selection - 1 + totalOptions) % totalOptions;
    }
    updateDisplay();
    break;

  case InputMessage::BUTTON_DOWN:
    if (fastScroll)
    {
      selection = (selection + 2) % totalOptions;
    }
    else
    {
      selection = (selection + 1) % totalOptions;
    }
    updateDisplay();
    break;

  case InputMessage::BUTTON_SELECT:
    executeSelection();
    break;
  }
}

void MenuScreen::onExit()
{
  // Cleanup si es necesario
}

String MenuScreen::getName()
{
  return "MENU";
}

void MenuScreen::updateDisplay()
{
  UIMessage msg = UIMessage::createMenu(selection, totalOptions);

  if (xQueueSend(uiQueue, &msg, 0) != pdTRUE)
  {
    Serial.println("⚠️ Cola UI llena, menu update ignorado");
  }
}

void MenuScreen::executeSelection()
{
  ScreenManager &manager = ScreenManager::getInstance();
  switch (selection)
  {
  case 0:
    manager.changeTo(SCREEN_DOLLAR);
    break;
  case 1:
    manager.changeTo(SCREEN_CLOCK);
    break;
  case 2:
    manager.changeTo(SCREEN_WEATHER);
    break;
  case 3:
    manager.changeTo(SCREEN_SCREENSAVER);
    break;
  case 4:
    manager.changeTo(SCREEN_SPOTIFY);
    break;
  }
}