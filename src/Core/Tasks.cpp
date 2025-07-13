#include "Tasks.h"
#include "ScreenManager.h"
#include "ButtonHandler.h"
#include "UIMessage.h"
#include "../Config/Config.h"
#include <DisplayManager.h>

// Referencias a servicios globales (declarados en main.cpp)
extern DisplayManager displayManager;
extern NTPClient timeClient;

void uiTask(void *parameters)
{
  UIMessage message;

  Serial.println("UI Task iniciado en Core: " + String(xPortGetCoreID()));

  while (true)
  {
    if (xQueueReceive(uiQueue, &message, pdMS_TO_TICKS(10)))
    {
      yield();

      switch (message.type)
      {
      case UIMessage::UPDATE_WEATHER:
        displayManager.drawWeatherPage(message.weatherData);
        yield();
        break;
      case UIMessage::UPDATE_DOLLAR:
        displayManager.drawDolarPage(message.dollarData);
        yield();
        break;
      case UIMessage::UPDATE_SPOTIFY:
        displayManager.drawSongDetails(&message.spotifyData, true, true);
        yield();
        break;
      case UIMessage::SHOW_MENU:
      {
        const char *menuOptions[5] = {"Dolar", "Reloj", "Clima", "Screensaver", "Spotify"};
        displayManager.drawMenu(message.menuData.total,
                                message.menuData.selection,
                                menuOptions);
        yield();
        break;
      }
      }
      delay(5);
    }

    if (uxQueueMessagesWaiting(uiQueue) > 8)
    {
      Serial.println("⚠️ UI Queue llena, limpiando...");
      xQueueReset(uiQueue);
    }

    vTaskDelay(pdMS_TO_TICKS(16));
  }
}

void inputTask(void *parameters)
{
  ScreenManager &screenManager = ScreenManager::getInstance();

  Serial.println("Input Task iniciado en Core: " + String(xPortGetCoreID()));

  while (true)
  {
    yield();
    buttonHandler.update();

    InputMessage msg;
    if (xQueueReceive(inputQueue, &msg, 0))
    {
      Serial.println("🎮 Input recibido: " + String(msg.type));
      screenManager.handleInput(msg.type);
      Serial.println("✅ Input procesado");
    }

    int waitingMessages = uxQueueMessagesWaiting(inputQueue);
    if (waitingMessages > 3)
    {
      Serial.println("⚠️ Input Queue llena (" + String(waitingMessages) + "), limpiando...");
      xQueueReset(inputQueue);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void screenUpdateTask(void *parameters)
{
  ScreenManager &screenManager = ScreenManager::getInstance();
  unsigned long lastWiFiCheck = 0;
  unsigned long lastNTPSync = 0;

  Serial.println("Screen Update Task iniciado en Core: " + String(xPortGetCoreID()));

  while (true)
  {
    screenManager.update();

    if (millis() - lastWiFiCheck > 1000)
    {
      ScreenType currentScreen = screenManager.getCurrentScreen();

      if (currentScreen == SCREEN_WIFI_SETUP && WiFi.status() == WL_CONNECTED)
      {
        static unsigned long wifiConnectedTime = 0;
        if (wifiConnectedTime == 0)
        {
          wifiConnectedTime = millis();
        }
        else if (millis() - wifiConnectedTime > 3000)
        {
          Serial.println("✅ WiFi conectado, cambiando al menú");
          screenManager.changeTo(SCREEN_MENU);
          wifiConnectedTime = 0;
        }
      }

      lastWiFiCheck = millis();
    }

    // Sincronización NTP
    if (WiFi.status() == WL_CONNECTED && millis() - lastNTPSync > NTP_SYNC_INTERVAL)
    {
      Serial.println("🕐 Sincronizando NTP...");
      if (timeClient.update())
      {
        Serial.println("✅ NTP sincronizado: " + timeClient.getFormattedTime());
      }
      else
      {
        Serial.println("⚠️ Error sincronizando NTP");
      }
      lastNTPSync = millis();
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setupTasks()
{
  xTaskCreatePinnedToCore(uiTask, "UI_Task", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(inputTask, "Input_Task", 2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(screenUpdateTask, "Screen_Task", 4096, NULL, 2, NULL, 0);

  Serial.println("✅ Tasks creados correctamente");
}