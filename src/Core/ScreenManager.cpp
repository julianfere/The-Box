#include "ScreenManager.h"
#include "../Screens/WiFiSetupScreen.h"
#include "../Screens/MenuScreen.h"
#include "../Screens/ClockScreen.h"
#include "../Screens/WeatherScreen.h"
#include "../Screens/DollarScreen.h"
#include "../Screens/SpotifyScreen.h"
#include "../Screens/ScreenSaverScreen.h"

ScreenManager &ScreenManager::getInstance()
{
  static ScreenManager instance;
  return instance;
}

ScreenManager::ScreenManager()
{
  screens[SCREEN_WIFI_SETUP] = new WiFiSetupScreen();
  screens[SCREEN_MENU] = new MenuScreen();
  screens[SCREEN_CLOCK] = new ClockScreen();
  screens[SCREEN_WEATHER] = new WeatherScreen();
  screens[SCREEN_DOLLAR] = new DollarScreen();
  screens[SCREEN_SPOTIFY] = new SpotifyScreen();
  screens[SCREEN_SCREENSAVER] = new ScreenSaverScreen();
}

void ScreenManager::changeTo(ScreenType newScreen)
{
  if (changingScreen || newScreen >= SCREEN_COUNT || newScreen == currentScreen)
  {
    Serial.println("⚠️ Cambio ignorado - ya cambiando o inválido");
    return;
  }

  changingScreen = true;
  Serial.println("📱 Cambiando: " + screens[currentScreen]->getName() +
                 " -> " + screens[newScreen]->getName());

  yield();
  screens[currentScreen]->onExit();
  yield();
  currentScreen = newScreen;
  yield();
  screens[currentScreen]->onEnter();
  yield();

  changingScreen = false;
  Serial.println("✅ Cambio completado");
}

void ScreenManager::handleInput(InputMessage::Type input)
{
  if (changingScreen)
  {
    Serial.println("⚠️ Input ignorado - cambiando pantalla");
    return;
  }

  screens[currentScreen]->onInput(input);
  yield();
}

void ScreenManager::update()
{
  if (changingScreen)
    return;

  screens[currentScreen]->onUpdate();
  yield();
}

ScreenType ScreenManager::getCurrentScreen()
{
  return currentScreen;
}

String ScreenManager::getCurrentScreenName()
{
  return screens[currentScreen]->getName();
}