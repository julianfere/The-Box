#ifndef WIFI_SETUP_SCREEN_H
#define WIFI_SETUP_SCREEN_H

#include "BaseScreen.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "../Config/Config.h"

// Forward declarations
class ApServer;
class Store;
class SpotifyBuddy;
struct WifiSettings;

class WiFiSetupScreen : public BaseScreen
{
private:
  enum WiFiState
  {
    CHECKING_SAVED,
    STARTING_AP,
    WAITING_CONFIG,
    CONNECTING,
    CONNECTED,
    FAILED
  } wifiState = CHECKING_SAVED;

  unsigned long stateStartTime = 0;
  unsigned long animationTime = 0;
  int dotCount = 0;

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;

private:
  void drawWiFiStatus();
  void drawProgressBar();
};

#endif