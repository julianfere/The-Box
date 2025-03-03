#ifndef SPOTIFY_BUDDY_H
#define SPOTIFY_BUDDY_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <base64.h>
#include <SPIFFS.h>
#include <TJpg_Decoder.h>

struct SongDetails
{
  String Id;
  String song;
  String artist;
  String album;
  String imageLink;
  bool isPlaying;
  float progressMs;
  bool isLiked;
  int durationMs;

  String toString()
  {
    return "Id: " + Id + " Song: " + song + " Artist: " + artist + " Album: " + album + " ImageLink: " + imageLink + " ProgressMs: " + String(progressMs) + " IsLiked: " + String(isLiked) + " DurationMs: " + String(durationMs);
  }
};

class SpotifyBuddy
{
public:
  SpotifyBuddy(String clientId, String clientSecret);
  bool getUserCode(String serverCode);
  bool refreshAuth();
  SongDetails getTrackInfo();
  bool findLikedStatus(String songId);
  bool toggleLiked(String songId);
  bool togglePlay();
  bool adjustVolume(int vol);
  bool skipForward();
  bool skipBack();
  bool likeSong(String songId);
  bool dislikeSong(String songId);
  bool isAccessTokenSet();
  int getTokenExpireTime();
  long getTokenStartTime();
  void setCallbackUrl(String url);

private:
  bool requestAuth(String serverCode, bool isRefresh = false);
  bool executeRequest(String url, String method, String requestBody = "");
  String getValue(HTTPClient &https, String key);
  bool getFile(String url, String filename);

  std::unique_ptr<WiFiClientSecure> client;
  HTTPClient https;
  bool isPlaying = false;
  String accessToken;
  String refreshToken;
  bool accessTokenSet = false;
  long tokenStartTime;
  int tokenExpireTime;
  SongDetails currentSong;
  float currentSongPositionMs;
  float lastSongPositionMs;
  int currVol;
  String clientId;
  String clientSecret;
  String callbackUrl;
  String urlencode(String str);
  void printSplitString(String text, int maxLineSize, int yPos);
  String lastImgUrl;
};
#endif
