#ifndef SPOTIFY_SCREEN_H
#define SPOTIFY_SCREEN_H

#include "Screens/BaseScreen.h"
#include "SpotifyBuddy.h"
#include <TFT_eSPI.h>

extern SpotifyBuddy spotifyBuddy;
extern TFT_eSPI tft;

class SpotifyScreen : public BaseScreen
{
private:
  unsigned long lastUpdate = 0;
  enum SpotifyMode
  {
    PLAYBACK,
    VOLUME
  } mode = PLAYBACK;
  int volume = 50;
  unsigned long modeDisplayTime = 0;

public:
  void onEnter() override;
  void onUpdate() override;
  void onInput(InputMessage::Type input) override;
  void onExit() override;
  String getName() override;

private:
  void updateSpotifyData();
  void showVolumeDisplay();
};

#endif // SPOTIFY_SCREEN_H