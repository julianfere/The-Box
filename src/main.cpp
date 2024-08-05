#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SimpleFSM.h>
#include "State.h"
#include "LcdDisplay.h"
#include "ApServer.h"
#include "Store.h"
#include "WheatherService.h"
//==============================================================================

#define _INIT "INIT"
#define _WIFI_DISCOVER "WIFI_DISCOVER"
#define _WIFI_CONNECT "WIFI_CONNECT"
#define _FETCH_UPDATES "FETCH_UPDATES"
#define _SHOW_INFO "SHOW_INFO"

//==============================================================================
LcdDisplay display = LcdDisplay();
SimpleFSM machine = SimpleFSM();
ApServer apServer;
Store store = Store();
WheatherService wheatherService = WheatherService();

WheatherInfo WHEATHER_INFO;

//==============================================================================

int TRIES = 10;

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
  Serial.println("-----------");
  Serial.print(apServer.getSsid());
  Serial.println("-----------");

  WiFi.begin("Fibertel WiFi856 2.4GHz", "0042070239");
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

void onEnterFetchUpdateState()
{
  display.print("Actualizando");
  WHEATHER_INFO = wheatherService.getCurrent();
}

void fetchUpdatesState()
{
  if (WHEATHER_INFO.success)
  {
    machine.trigger(Triggers::SHOW_INFO);
  }
  else
  {
    display.printWithSuspense("Actualizando");
    WHEATHER_INFO = wheatherService.getCurrent();
    sleep(1);
  }
}

void showInfoState()
{
  String firstLine = "T:" + String(WHEATHER_INFO.temp) + " H:" + String(WHEATHER_INFO.hum);
  String secondLine = String(WHEATHER_INFO.rain) + "% " + String(WHEATHER_INFO.status);
  display.printMultiline(firstLine, secondLine);
}
#pragma endregion
//==============================================================================
#pragma region States Transitions
//                        Nombre              OnEnter                 OnState                 OnExit

State INIT_STATE = State(_INIT, initeState);
State WIFI_DISCOVER_STATE = State(_WIFI_DISCOVER, onEnterwifiDiscoveryState, wifiDiscoveryState, onExitWifiDiscoveryState);
State WIFI_CONNECT_STATE = State(_WIFI_CONNECT, onEnterConnectState, connectState);
State FETCH_UPDATES_STATE = State(_FETCH_UPDATES, onEnterFetchUpdateState, fetchUpdatesState);
State SHOW_INFO_STATE = State(_SHOW_INFO, showInfoState);

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

  int num_transitions = sizeof(transitions) / sizeof(Transition);

  machine.add(transitions, num_transitions);
  machine.setInitialState(&INIT_STATE);

  Serial.println(machine.getDotDefinition()); // Imprime la definición del grafo en formato DOT para usar en graphviz
}

void loop()
{
  machine.run();
}
