#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TFT_eSPI.h>
#include "ApServer.h"
#include "Store.h"
#include "WeatherService.h"
#include "DollarService.h"
#include "DisplayManager.h"
#include "index.h"
#include "SpotifyBuddy.h"
#include "Logger.h"
#include "Core/ScreenManager.h"
#include "Core/ButtonHandler.h"
#include "Core/Tasks.h"

// =============================================================================
// CONFIGURACIÓN HARDWARE
// =============================================================================

#define CLIENT_ID "CLIENT_ID"
#define CLIENT_SECRET "CLIENT_SECRET"

QueueHandle_t uiQueue;
QueueHandle_t inputQueue;

TFT_eSPI tft = TFT_eSPI();
ApServer apServer;
Store store = Store();
WeatherService weatherService = WeatherService();
WiFiUDP ntpUDP;
DollarService dollarService = DollarService();
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);
DisplayManager displayManager = DisplayManager();
SpotifyBuddy spotifyBuddy(CLIENT_ID, CLIENT_SECRET);
AsyncWebServer httpServer(8080);
Logger logger = Logger();
ButtonHandler buttonHandler = ButtonHandler();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
  if (y >= tft.height())
    return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void handleRoot(AsyncWebServerRequest *request)
{
  String page = String(mainPage);
  page.replace("%client_id%", CLIENT_ID);
  String localIp = WiFi.localIP().toString();
  page.replace("%redirect_uri%", "http://" + localIp + ":8080/callback");
  request->send(200, "text/html", page);
}

void handleCallback(AsyncWebServerRequest *request)
{
  if (!spotifyBuddy.isAccessTokenSet())
  {
    if (request->arg("code") == "")
    {
      String page = String(errorPage);
      page.replace("%client_id%", CLIENT_ID);
      String localIp = WiFi.localIP().toString();
      page.replace("%redirect_uri%", "http://" + localIp + ":8080/callback");
      request->send(200, "text/html", String(page));
    }
    else
    {
      if (spotifyBuddy.getUserCode(request->arg("code")))
      {
        request->send(200, "text/html", successPage);
      }
      else
      {
        String page = String(errorPage);
        page.replace("%client_id%", CLIENT_ID);
        String localIp = WiFi.localIP().toString();
        page.replace("%redirect_uri%", "http://" + localIp + "/callback");
        request->send(200, "text/html", String(page));
      }
    }
  }
  else
  {
    request->send(200, "text/html", "Spotify setup complete");
  }
}

void setupHTTPServer()
{
  httpServer.on("/", HTTP_GET, handleRoot);
  httpServer.on("/callback", HTTP_GET, handleCallback);
  httpServer.on("/callback", HTTP_POST, handleCallback);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("🚀 TheBox iniciando...");

  displayManager.init();
  buttonHandler.setup();

  uiQueue = xQueueCreate(15, sizeof(UIMessage));
  inputQueue = xQueueCreate(8, sizeof(InputMessage));

  if (uiQueue == NULL || inputQueue == NULL)
  {
    Serial.println("❌ Error creando colas!");
    return;
  }

  Serial.println("✅ Colas creadas correctamente");

  SPIFFS.begin();
  TJpgDec.setCallback(tft_output);

  xTaskCreatePinnedToCore(uiTask, "UI_Task", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(inputTask, "Input_Task", 2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(screenUpdateTask, "Screen_Task", 4096, NULL, 2, NULL, 0);

  Serial.println("✅ Tasks creados");

  setupHTTPServer();

  ScreenManager::getInstance().changeTo(SCREEN_WIFI_SETUP);

  Serial.println("🌐 Iniciando configuración WiFi...");
}

void loop()
{
  apServer.handleNextRequest();
  vTaskDelay(pdMS_TO_TICKS(1000));
}