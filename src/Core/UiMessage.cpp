#include "UIMessage.h"

extern QueueHandle_t uiQueue;
extern QueueHandle_t inputQueue;

UIMessage UIMessage::createMenu(int selection, int total)
{
  UIMessage msg;
  msg.type = SHOW_MENU;
  msg.menuData.selection = selection;
  msg.menuData.total = total;
  return msg;
}

UIMessage UIMessage::createWeather(const WeatherInfo &weather)
{
  UIMessage msg;
  msg.type = UPDATE_WEATHER;
  msg.weatherData = weather;
  return msg;
}

UIMessage UIMessage::createDollar(const DollarInfo &dollar)
{
  UIMessage msg;
  msg.type = UPDATE_DOLLAR;
  msg.dollarData = dollar;
  return msg;
}

UIMessage UIMessage::createSpotify(const SongDetails &spotify)
{
  UIMessage msg;
  msg.type = UPDATE_SPOTIFY;
  msg.spotifyData = spotify;
  return msg;
}