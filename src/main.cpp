#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SimpleFSM.h>
#include "State.h"
#include "LcdDisplay.h"
#include "ApServer.h"
#include "Store.h"
#include "WeatherService.h"
#include "DollarService.h"
//==============================================================================

#define _INIT "INIT"
#define _WIFI_DISCOVER "WIFI_DISCOVER"
#define _WIFI_CONNECT "WIFI_CONNECT"
#define _FETCH_UPDATES "FETCH_UPDATES"
#define _SHOW_INFO "SHOW_INFO"

#define BUTTON_1_PIN 12
#define BUTTON_2_PIN 13

//==============================================================================
LcdDisplay display = LcdDisplay();
SimpleFSM machine = SimpleFSM();
ApServer apServer;
Store store = Store();
WeatherService weatherService = WeatherService();
DollarService dollarService = DollarService();

WeatherInfo WEATHER_INFO;
DollarInfo DOLLAR_INFO;

//==============================================================================

int TRIES = 10;
bool BUTTON_1_PRESSED = false;
bool BUTTON_2_PRESSED = false;
bool SHOW_DOLLAR = false;

enum Triggers
{
  INIT = 1,
  WIFI_DISCOVER = 2,
  WIFI_CONNECT = 3,
  FETCH_UPDATES = 4,
  SHOW_INFO = 5
};

#pragma redion State Handlers
//==============================================================================

void initeState()
{
  display.print("Iniciando");

  if (store.hasWifiSettings())
  {
    // Carga la configuracion del wifi e intenta conectarse a la red
    WifiSettings wifiSettings = store.getWifiSettings();
    apServer.setSsid(wifiSettings.ssid);
    apServer.setPassword(wifiSettings.password);

    Serial.println("Conectando a red guardada");

    machine.trigger(Triggers::WIFI_CONNECT);
  }
  else
  {
    machine.trigger(Triggers::WIFI_DISCOVER);
  }
}

#pragma region Wifi Discovery State

void onEnterwifiDiscoveryState()
{
  apServer.setup();
  display.print("Buscando redes");
  sleep(1);
}

void wifiDiscoveryState()
{
  apServer.handleNextRequest();

  if (apServer.networkSelected())
  {
    machine.trigger(Triggers::WIFI_CONNECT);
  }

  display.printWithSuspense("Esperando red");
}

void onExitWifiDiscoveryState()
{
  apServer.teardown();
}
#pragma endregion
#pragma region Connect State

void onEnterConnectState()
{
  TRIES = 12;
  WiFi.mode(WIFI_STA);
  Serial.println("Conectando a red: " + apServer.getSsid());
  WiFi.begin(apServer.getSsid(), apServer.getPassword());
}

void connectState()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WifiSettings wifiSettings = {apServer.getSsid(), apServer.getPassword()};
    store.storeWifiSettings(wifiSettings);

    machine.trigger(Triggers::FETCH_UPDATES);
  }

  TRIES--;
  display.printWithSuspense("Conectando");
  delay(1000);

  if (TRIES < 0)
  {
    machine.trigger(Triggers::WIFI_DISCOVER);
  }
}

#pragma end region

#pragma region Fetch Updates State

void onEnterFetchUpdateState()
{
  display.print("Actualizando");
  WEATHER_INFO = weatherService.getCurrent();
  DOLLAR_INFO = dollarService.getCurrent();
}

void fetchUpdatesState()
{
  if (!WEATHER_INFO.success)
  {
    display.printWithSuspense("Actualizando");
    WEATHER_INFO = weatherService.getCurrent();
    sleep(1);
  }

  if (!DOLLAR_INFO.success)
  {
    display.printWithSuspense("Actualizando");
    DOLLAR_INFO = dollarService.getCurrent();
    sleep(1);
  }

  if (WEATHER_INFO.success && DOLLAR_INFO.success)
  {
    machine.trigger(Triggers::SHOW_INFO);
  }
}
#pragma endregion

void onEnterShowInfoState()
{
  if (SHOW_DOLLAR)
  {
    display.printDollar(DOLLAR_INFO);
  }
  else
  {
    display.printweather(WEATHER_INFO);
  }
}

void showInfoState()
{
  if (digitalRead(BUTTON_1_PIN) == HIGH)
  {

    if (!BUTTON_1_PRESSED)
    {
      BUTTON_1_PRESSED = true;
      SHOW_DOLLAR = !SHOW_DOLLAR;

      if (SHOW_DOLLAR)
      {
        display.printDollar(DOLLAR_INFO);
      }
      else
      {
        display.printweather(WEATHER_INFO);
      }
    }
  }
  else
  {
    BUTTON_1_PRESSED = false;
  }

  if (digitalRead(BUTTON_2_PIN) == HIGH)
  {

    if (!BUTTON_2_PRESSED)
    {
      BUTTON_2_PRESSED = true;
      machine.trigger(Triggers::FETCH_UPDATES);
    }
  }
  else
  {
    BUTTON_2_PRESSED = false;
  }
}

#pragma endregion
//==============================================================================
#pragma region States Transitions
//                        Nombre              OnEnter                 OnState                 OnExit

State INIT_STATE = State(_INIT, initeState);
State WIFI_DISCOVER_STATE = State(_WIFI_DISCOVER, onEnterwifiDiscoveryState, wifiDiscoveryState, onExitWifiDiscoveryState);
State WIFI_CONNECT_STATE = State(_WIFI_CONNECT, onEnterConnectState, connectState);
State FETCH_UPDATES_STATE = State(_FETCH_UPDATES, onEnterFetchUpdateState, fetchUpdatesState);
State SHOW_INFO_STATE = State(_SHOW_INFO, onEnterShowInfoState, showInfoState);

//==============================================================================

void log(String from, String to)
{
  Serial.println("================================================");
  Serial.println("From: " + from + " to: " + to);
  Serial.println("================================================");
}

Transition transitions[]{
    Transition(&INIT_STATE, &WIFI_DISCOVER_STATE, Triggers::WIFI_DISCOVER, []()
               { log("init", "wifi discover"); }),
    Transition(&INIT_STATE, &WIFI_CONNECT_STATE, Triggers::WIFI_CONNECT, []()
               { log("init", "connect"); }),
    Transition(&WIFI_DISCOVER_STATE, &WIFI_CONNECT_STATE, Triggers::WIFI_CONNECT, []()
               { log("wifi discover", "connect"); }),
    Transition(&WIFI_CONNECT_STATE, &FETCH_UPDATES_STATE, Triggers::FETCH_UPDATES, []()
               { log("connect", "fetch"); }),
    Transition(&FETCH_UPDATES_STATE, &SHOW_INFO_STATE, Triggers::SHOW_INFO, []()
               { log("fetch", "show info"); }),
    Transition(&SHOW_INFO_STATE, &FETCH_UPDATES_STATE, Triggers::FETCH_UPDATES, []()
               { log("fetch", "show info"); })};

#pragma endregion

void setup()
{
  Serial.begin(115200);

  display.init();

  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);

  int num_transitions = sizeof(transitions) / sizeof(Transition);

  machine.add(transitions, num_transitions);
  machine.setInitialState(&INIT_STATE);

  Serial.println(machine.getDotDefinition()); // Imprime la definición del grafo en formato DOT para usar en graphviz
}

void loop()
{
  machine.run();
}
