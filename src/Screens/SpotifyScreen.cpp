#include "Screens/BaseScreen.h"
#include "SpotifyBuddy.h"
#include "SpotifyScreen.h"

extern SpotifyBuddy spotifyBuddy;
extern TFT_eSPI tft;

void SpotifyScreen::onEnter()
{
  mode = PLAYBACK;
  updateSpotifyData();
}

void SpotifyScreen::onUpdate()
{
  if (millis() - lastUpdate > 3000)
  {
    updateSpotifyData();
  }

  if (mode == VOLUME && millis() - modeDisplayTime > 3000)
  {
    mode = PLAYBACK;
    updateSpotifyData();
  }
}

void SpotifyScreen::onInput(InputMessage::Type input)
{
  switch (mode)
  {
  case PLAYBACK:
    switch (input)
    {
    case InputMessage::BUTTON_UP:
      spotifyBuddy.skipBack();
      break;
    case InputMessage::BUTTON_DOWN:
      spotifyBuddy.skipForward();
      break;
    case InputMessage::BUTTON_SELECT:
      spotifyBuddy.togglePlay();
      break;
    }
    break;
  case VOLUME:
    switch (input)
    {
    case InputMessage::BUTTON_UP:
      volume = min(100, volume + 10);
      spotifyBuddy.adjustVolume(volume);
      showVolumeDisplay();
      modeDisplayTime = millis();
      break;
    case InputMessage::BUTTON_DOWN:
      volume = max(0, volume - 10);
      spotifyBuddy.adjustVolume(volume);
      showVolumeDisplay();
      modeDisplayTime = millis();
      break;
    case InputMessage::BUTTON_SELECT:
      mode = PLAYBACK;
      updateSpotifyData();
      break;
    }
    break;
  }
}

void SpotifyScreen::onExit()
{
  // Implementación vacía
}

String SpotifyScreen::getName()
{
  return "SPOTIFY";
}

void SpotifyScreen::updateSpotifyData()
{
  if (spotifyBuddy.isAccessTokenSet())
  {
    SongDetails song = spotifyBuddy.getTrackInfo();

    UIMessage msg = UIMessage::createSpotify(song);

    // CRÍTICO: No bloquear si la cola está llena
    if (xQueueSend(uiQueue, &msg, 0) != pdTRUE)
    {
      Serial.println("⚠️ Cola UI llena, spotify update ignorado");
    }

    lastUpdate = millis();
  }
}

void SpotifyScreen::showVolumeDisplay()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(50, 100);
  tft.print("VOL: " + String(volume));
}
