#ifndef UI_MESSAGE_H
#define UI_MESSAGE_H

#include <Arduino.h>
#include "WeatherService.h"
#include "DollarService.h"
#include "SpotifyBuddy.h"

struct UIMessage
{
  enum Type
  {
    UPDATE_WEATHER,
    UPDATE_DOLLAR,
    UPDATE_SPOTIFY,
    SHOW_MENU,
    DRAW_CLOCK
  } type;

  WeatherInfo weatherData;
  DollarInfo dollarData;
  SongDetails spotifyData;
  struct
  {
    int selection;
    int total;
  } menuData;

  UIMessage() : type(SHOW_MENU)
  {
    menuData.selection = 0;
    menuData.total = 0;
  }

  static UIMessage createMenu(int selection, int total);
  static UIMessage createWeather(const WeatherInfo &weather);
  static UIMessage createDollar(const DollarInfo &dollar);
  static UIMessage createSpotify(const SongDetails &spotify);
};

struct InputMessage
{
  enum Type
  {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT,
    BUTTON_BACK
  } type;
  unsigned long timestamp;
};

// Variables globales de colas
extern QueueHandle_t uiQueue;
extern QueueHandle_t inputQueue;

#endif