#include "WiFiSetupScreen.h"
#include "../Core/ScreenManager.h"
#include <ESPAsyncWebServer.h>
#include "Store.h"
#include "ApServer.h"
#include <NTPClient.h>

// Referencias a servicios globales
extern TFT_eSPI tft;
extern ApServer apServer;
extern Store store;
extern SpotifyBuddy spotifyBuddy;
extern AsyncWebServer httpServer;
extern ApServer apServer;
extern NTPClient timeClient;

void WiFiSetupScreen::onEnter()
{
  wifiState = STARTING_AP;
  stateStartTime = millis();
  drawWiFiStatus();
  Serial.println("WIFI");
  if (store.hasWifiSettings())
  {
    WifiSettings settings = store.getWifiSettings();
    Serial.println("WiFi guardado encontrado: " + settings.ssid);
    wifiState = CONNECTING;
    WiFi.begin(settings.ssid.c_str(), settings.password.c_str());
  }
  else
  {
    Serial.println("No hay WiFi guardado, iniciando AP");
    wifiState = STARTING_AP;
    apServer.setup();
  }
  stateStartTime = millis();
}

void WiFiSetupScreen::onUpdate()
{
  unsigned long now = millis();

  if (now - animationTime > ANIMATION_INTERVAL)
  {
    dotCount = (dotCount + 1) % 4;
    animationTime = now;
    drawWiFiStatus();
  }

  switch (wifiState)
  {
  case CONNECTING:
    if (WiFi.status() == WL_CONNECTED)
    {
      wifiState = CONNECTED;
      Serial.println("WiFi conectado: " + WiFi.localIP().toString());

      spotifyBuddy.setCallbackUrl("http://" + WiFi.localIP().toString() + ":8080/callback");
      httpServer.begin();

      drawWiFiStatus();
    }
    else if (now - stateStartTime > WIFI_TIMEOUT)
    {
      wifiState = FAILED;
      stateStartTime = now;
      drawWiFiStatus();
    }
    break;

  case STARTING_AP:
    if (now - stateStartTime > 2000)
    {
      wifiState = WAITING_CONFIG;
      stateStartTime = now;
      drawWiFiStatus();
    }
    break;

  case WAITING_CONFIG:
    apServer.handleNextRequest();
    if (apServer.networkSelected())
    {
      wifiState = CONNECTING;
      stateStartTime = now;

      WiFi.mode(WIFI_STA);
      WiFi.begin(apServer.getSsid().c_str(), apServer.getPassword().c_str());

      WifiSettings settings = {apServer.getSsid(), apServer.getPassword()};
      store.storeWifiSettings(settings);
      drawWiFiStatus();
    }
    break;

  case FAILED:
    if (now - stateStartTime > 3000)
    {
      wifiState = STARTING_AP;
      stateStartTime = now;
      apServer.setup();
      drawWiFiStatus();
    }
    break;

  case CONNECTED:
    // Manejado en screenUpdateTask
    break;
  }
}

void WiFiSetupScreen::onInput(InputMessage::Type input)
{
  // No input handling durante setup WiFi
}

void WiFiSetupScreen::onExit()
{
  if (wifiState == WAITING_CONFIG)
  {
    apServer.teardown();
  }
}

String WiFiSetupScreen::getName()
{
  return "WIFI_SETUP";
}

void WiFiSetupScreen::drawWiFiStatus()
{
  static WiFiState lastDrawnState = CHECKING_SAVED;
  static int lastDrawnDots = -1;

  if (wifiState == lastDrawnState && dotCount == lastDrawnDots)
  {
    return;
  }

  if (wifiState != lastDrawnState)
  {
    tft.fillScreen(TFT_BLACK);
    yield();

    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("TheBox WiFi");

    tft.drawLine(0, 40, 320, 40, TFT_CYAN);
    yield();
  }

  if (wifiState == lastDrawnState && dotCount != lastDrawnDots)
  {
    tft.fillRect(200, 60, 100, 30, TFT_BLACK);
  }
  else
  {
    tft.fillRect(20, 60, 280, 120, TFT_BLACK);
  }
  yield();

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  String dots = "";
  for (int i = 0; i < dotCount; i++)
  {
    dots += ".";
  }

  switch (wifiState)
  {
  case CHECKING_SAVED:
    tft.setCursor(20, 60);
    tft.print("Verificando WiFi");
    tft.setCursor(200, 60);
    tft.print(dots);
    break;
  case STARTING_AP:
    tft.setCursor(20, 60);
    tft.print("Iniciando AP");
    tft.setCursor(200, 60);
    tft.print(dots);
    break;
  case WAITING_CONFIG:
    if (wifiState != lastDrawnState)
    {
      tft.setTextSize(1);
      tft.setCursor(20, 60);
      tft.println("Conectate a:");
      tft.setTextColor(TFT_YELLOW);
      tft.setCursor(20, 80);
      tft.println("Red: TheBox");
      tft.setCursor(20, 100);
      tft.println("Pass: TheBox123");
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(20, 130);
      tft.println("Luego ve a cualquier");
      tft.setCursor(20, 150);
      tft.println("sitio web para");
      tft.setCursor(20, 170);
      tft.print("configurar");
    }
    tft.setCursor(160, 170);
    tft.print(dots + "    ");
    break;
  case CONNECTING:
    tft.setCursor(20, 60);
    tft.print("Conectando");
    tft.setCursor(200, 60);
    tft.print(dots);
    if (wifiState != lastDrawnState && store.hasWifiSettings())
    {
      WifiSettings settings = store.getWifiSettings();
      tft.setTextSize(1);
      tft.setTextColor(TFT_DARKGREY);
      tft.setCursor(20, 90);
      tft.print("Red: " + settings.ssid);
    }
    break;
  case CONNECTED:
    if (wifiState != lastDrawnState)
    {
      tft.setTextColor(TFT_GREEN);
      tft.setCursor(20, 60);
      tft.print("Conectado!");
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(20, 90);
      tft.print("IP: " + WiFi.localIP().toString());
    }
    break;
  case FAILED:
    if (wifiState != lastDrawnState)
    {
      tft.setTextColor(TFT_RED);
      tft.setCursor(20, 60);
      tft.print("Error conexion");
      tft.setTextSize(1);
      tft.setTextColor(TFT_DARKGREY);
      tft.setCursor(20, 90);
      tft.print("Reintentando...");
    }
    break;
  }

  yield();

  if (wifiState != lastDrawnState)
  {
    drawProgressBar();
  }

  lastDrawnState = wifiState;
  lastDrawnDots = dotCount;
  yield();
}

void WiFiSetupScreen::drawProgressBar()
{
  int barY = 200;
  int barWidth = 280;
  int barHeight = 8;
  int barX = (320 - barWidth) / 2;

  tft.fillRect(barX - 5, barY - 5, barWidth + 10, barHeight + 20, TFT_BLACK);
  yield();

  tft.drawRect(barX, barY, barWidth, barHeight, TFT_DARKGREY);

  int progress = 0;
  switch (wifiState)
  {
  case CHECKING_SAVED:
    progress = 10;
    break;
  case STARTING_AP:
    progress = 25;
    break;
  case WAITING_CONFIG:
    progress = 50;
    break;
  case CONNECTING:
    progress = 75;
    break;
  case CONNECTED:
    progress = 100;
    break;
  case FAILED:
    progress = 0;
    break;
  }

  int fillWidth = (barWidth * progress) / 100;
  uint16_t color = (wifiState == FAILED) ? TFT_RED : (wifiState == CONNECTED) ? TFT_GREEN
                                                                              : TFT_CYAN;

  if (fillWidth > 2)
  {
    tft.fillRect(barX + 1, barY + 1, fillWidth - 2, barHeight - 2, color);
  }

  yield();

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);

  String progressText = String(progress) + "%";
  int textWidth = tft.textWidth(progressText);
  int textX = barX + (barWidth - textWidth) / 2;

  tft.setCursor(textX, barY + 15);
  tft.print(progressText);

  yield();
}
