#ifndef DisplayManager_h
#define DisplayManager_h
#include <Arduino.h>
#include <DollarService.h>
#include "WeatherService.h"
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include "SpotifyBuddy.h"
#include <TJpg_Decoder.h>

#define COLOR_BLUE tft.color565(0, 0, 255)
#define COLOR_BLACK tft.color565(0, 0, 0)
#define COLOR_CYAN tft.color565(0, 255, 255)
#define COLOR_LIGHT_BLUE tft.color565(173, 216, 230)
#define COLOR_YELLOW tft.color565(255, 255, 0)
#define COLOR_WHITE tft.color565(255, 255, 255)

class DisplayManager
{
public:
  void init();
  void drawHeader(const char *title);
  void drawInitPage();
  void drawWifiDiscoveryPage();
  void drawWifiConnectionProgress();
  void drawDolarPage(DollarInfo data, bool loading = false);
  void drawWeatherPage(WeatherInfo data, bool loading = false);
  void drawMenu(int numOptions, int selectedOption, const char *menuOptions[]);
  DisplayManager();
  static DisplayManager &getInstance();
  void drawClockPage(NTPClient &timeClient);
  void drawSongDetails(SongDetails *currentSong, bool fullRefresh = false, bool likeRefresh = false);

private:
  TFT_eSPI tft;
  static DisplayManager *instance;
  void drawClockFace(int x, int y);
  void drawTime(int x, int y, int hh, int mm);
  void drawSeconds(int x, int y, int ss);
  void printSplitString(String text, int maxLineSize, int yPos);
  float songProgress;
  String formatTime(int ms);
};
#endif