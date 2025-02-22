#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TFT_eSPI.h>
#include "ApServer.h"
#include "Store.h"
#include "WeatherService.h"
#include "DollarService.h"
#include "AnalogHandler.h"
#include "StateMachine.h"
#include "DisplayManager.h"
//==============================================================================

#define _INIT "INIT"
#define _WIFI_DISCOVER "WIFI_DISCOVER"
#define _WIFI_CONNECT "WIFI_CONNECT"
#define _MENU "MENU"
#define _CLOCK "CLOCK"
#define _SCREEN_SAVER "SCREEN_SAVER"
#define _WEATHER "WEATHER"
#define _DOLAR "DOLAR"
#define _SPOTIFY "SPOTIFY"

const int xPin = 27;  // the VRX attach to
const int yPin = 32;  // the VRY attach to
const int swPin = 25; // the SW attach to

//==============================================================================
AnalogHandler analogHandler = AnalogHandler(xPin, yPin, swPin);

TFT_eSPI tft = TFT_eSPI(); // Inicializa la pantalla
ApServer apServer;
Store store = Store();
WeatherService weatherService = WeatherService();
WiFiUDP ntpUDP;
DollarService dollarService = DollarService();
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);
WeatherInfo WEATHER_INFO;
DollarInfo DOLLAR_INFO;
DisplayManager displayManager = DisplayManager();

QueueHandle_t WIFI_STATUS_QUEUE = xQueueCreate(1, sizeof(bool));

//==============================================================================

const int numOptions = 5;
const char *menuOptions[numOptions] = {
    "Dolar",
    "Reloj",
    "Clima",
    "Salva Pantallas",
    "Spotify",
};
int selectedOption = 0;
int newOption = 0;

enum StateTransitions : int
{
  INIT = 0,
  WIFI_DISCOVER = 1,
  WIFI_CONNECT = 2,
  MENU = 3,
  CLOCK = 4,
  SCREEN_SAVER = 5,
  WEATHER = 6,
  DOLAR = 7,
  SPOTIFY = 8
};

void onEnterInit(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Iniciando...");
  Serial.println("=========================================================================");

  displayManager.drawInitPage();

  if (store.hasWifiSettings())
  {
    Serial.println("store tiene wifi settings");
    machine.changeState(StateTransitions::WIFI_CONNECT);
  }
  else
  {
    Serial.println("store tiene wifi settings");

    machine.changeState(StateTransitions::WIFI_DISCOVER);
  }
}

void onEnterWaiting(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Discovery...");
  Serial.println("=========================================================================");
  displayManager.drawWifiDiscoveryPage();
}

void onStateWaiting(StateMachine &machine)
{
  apServer.handleNextRequest();

  if (apServer.networkSelected())
  {
    machine.changeState(StateTransitions::WIFI_CONNECT);
  }
}

void onExitWaiting(StateMachine &machine)
{
  apServer.teardown();
}

void onEnterConnecting(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Connecting...");
  Serial.println("=========================================================================");
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.fillScreen(TFT_BLACK);
  tft.println("Conectando...");
  String ssid;
  String password;

  if (store.hasWifiSettings())
  {
    WifiSettings settints = store.getWifiSettings();
    ssid = settints.ssid;
    password = settints.password;
  }
  else
  {
    ssid = apServer.getSsid();
    password = apServer.getPassword();
  }

  WiFi.begin(ssid.c_str(), password.c_str());
}

void onStateConnecting(StateMachine &machine)
{
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
  {
    Serial.printf("Conectando, tiempo restante: %d\n", 10 - (millis() - startTime) / 1000);
    delay(100); // Pequeño delay para no saturar el CPU
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    WifiSettings settings = WifiSettings{apServer.getSsid(), apServer.getPassword()};
    store.storeWifiSettings(settings);
    machine.changeState(StateTransitions::MENU);
  }
  else
  {
    machine.changeState(StateTransitions::WIFI_DISCOVER);
  }
}

void onEnterConnected(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Connected...");
  Serial.println("=========================================================================");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.fillScreen(TFT_BLACK);
  tft.println("Conectado!");
  delay(200);
  displayManager.drawMenu(numOptions, selectedOption, menuOptions);
}

void onMenu(StateMachine &machine)
{
  if (analogHandler.isDown())
  {
    if (newOption == numOptions - 1)
    {
      newOption = 0;
    }
    else
    {
      newOption++;
    }
  }
  else if (analogHandler.isUp())
  {
    if (newOption == 0)
    {
      newOption = numOptions - 1;
    }
    else
    {
      newOption--;
    }
  }

  if (selectedOption != newOption)
  {
    selectedOption = newOption;
    displayManager.drawMenu(numOptions, selectedOption, menuOptions);
  }

  if (analogHandler.isPressed())
  {
    switch (selectedOption)
    {
      // case 0:
      //   machine.trigger(Triggers::DOLAR);
      //   break;
      // case 1:
      //   machine.trigger(Triggers::CLOCK_SCREEN);
      //   break;
      // case 2:
      //   machine.trigger(Triggers::WEATHER);
      //   break;
      // case 3:
      //   machine.trigger(Triggers::SCREEN_SAVER);
      //   break;
      // case 4:
      //   machine.trigger(Triggers::SPOTIFY);
      //   break;
    }
  }
}

DState states[] = {
    {_INIT, onEnterInit, NULL, NULL, 0},
    {_WIFI_DISCOVER, onEnterWaiting, onStateWaiting, onExitWaiting, 0},
    {_WIFI_CONNECT, onEnterConnecting, onStateConnecting, NULL, 0},
    {_MENU, onEnterConnected, onMenu, NULL, 0},
};

DTransition transitions[] = {
    {_INIT, StateTransitions::WIFI_DISCOVER, _WIFI_DISCOVER},
    {_INIT, StateTransitions::WIFI_CONNECT, _WIFI_CONNECT},
    {_WIFI_DISCOVER, StateTransitions::WIFI_CONNECT, _WIFI_CONNECT}, // Event 1: User submitted Wi-Fi credentials
    {_WIFI_CONNECT, StateTransitions::MENU, _MENU},                  // Event 2: Wi-Fi connected successfully
    {_WIFI_CONNECT, StateTransitions::WIFI_DISCOVER, _WIFI_DISCOVER}};

StateMachine stateMachine(states, 4, transitions, 5);

void setup()
{
  Serial.begin(115200);
  displayManager.init();
  apServer.setup();
  stateMachine.begin(_INIT);
}

void loop()
{
  stateMachine.update();
}