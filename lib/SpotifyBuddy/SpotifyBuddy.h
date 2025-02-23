#ifndef SPOTIFY_BUDDY_H
#define SPOTIFY_BUDDY_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <base64.h>
#include <SPIFFS.h>
#include <TJpg_Decoder.h>

struct SongDetails
{
  String Id;
  String song;
  String artist;
  String album;
  bool isLiked;
  int durationMs;
};

class SpotifyBuddy
{
public:
  SpotifyBuddy(String clientId, String clientSecret, String callbackUrl);
  bool getUserCode(String serverCode);
  bool refreshAuth();
  bool getTrackInfo();
  bool findLikedStatus(String songId);
  bool toggleLiked(String songId);
  bool drawScreen(bool fullRefresh = false, bool likeRefresh = false);
  bool togglePlay();
  bool adjustVolume(int vol);
  bool skipForward();
  bool skipBack();
  bool likeSong(String songId);
  bool dislikeSong(String songId);
  bool isAccessTokenSet();
  int getTokenExpireTime();
  long getTokenStartTime();

private:
  bool requestAuth(String serverCode, bool isRefresh = false);
  bool executeRequest(String url, String method, String requestBody = "");
  String getValue(HTTPClient &https, String key);
  bool getFile(const char *url, const char *path);

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
};
#endif
