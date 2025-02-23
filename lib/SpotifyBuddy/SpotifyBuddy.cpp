#include "SpotifyBuddy.h"

SpotifyBuddy::SpotifyBuddy(String clientId, String clientSecret, String callbackUrl)
    : callbackUrl(callbackUrl), clientId(clientId), clientSecret(clientSecret)
{
  client = std::unique_ptr<WiFiClientSecure>(new WiFiClientSecure());
  client->setInsecure();
}

// Helper to execute POST/GET/PUT requests to Spotify API
bool SpotifyBuddy::executeRequest(String url, String method, String requestBody)
{
  https.begin(*client, url);
  String auth = "Bearer " + accessToken;
  https.addHeader("Authorization", auth);

  if (method == "POST")
  {
    return https.POST(requestBody) == HTTP_CODE_OK;
  }
  else if (method == "PUT")
  {
    return https.PUT(requestBody) == HTTP_CODE_OK;
  }
  else if (method == "GET")
  {
    return https.GET() == HTTP_CODE_OK;
  }

  https.end();
  return false;
}

// Get user authorization code
bool SpotifyBuddy::getUserCode(String serverCode)
{
  return requestAuth(serverCode);
}

// Refresh authentication token
bool SpotifyBuddy::refreshAuth()
{
  return requestAuth("", true);
}

// Request authentication from Spotify API
bool SpotifyBuddy::requestAuth(String serverCode, bool isRefresh)
{
  String url = "https://accounts.spotify.com/api/token";
  String auth = "Basic " + base64::encode(String(this->clientId) + ":" + String(this->clientSecret));
  https.addHeader("Authorization", auth);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String requestBody = isRefresh ? "grant_type=refresh_token&refresh_token=" + refreshToken : "grant_type=authorization_code&code=" + serverCode + "&redirect_uri=" + String(this->callbackUrl);

  bool success = executeRequest(url, "POST", requestBody);
  if (success)
  {
    String response = https.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    accessToken = String((const char *)doc["access_token"]);
    refreshToken = String((const char *)doc["refresh_token"]);
    tokenExpireTime = doc["expires_in"];
    tokenStartTime = millis();
    accessTokenSet = true;
    Serial.println(accessToken);
  }
  https.end();
  return accessTokenSet;
}

// Get track information
bool SpotifyBuddy::getTrackInfo()
{
  String url = "https://api.spotify.com/v1/me/player/currently-playing";
  https.begin(*client, url);
  String auth = "Bearer " + String(accessToken);
  https.addHeader("Authorization", auth);

  int httpResponseCode = https.GET();
  bool success = false;

  if (httpResponseCode == 200)
  {
    String response = https.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    currentSongPositionMs = getValue(https, "progress_ms").toFloat();
    currentSong.durationMs = getValue(https, "duration_ms").toInt();
    currentSong.Id = getValue(https, "uri").substring(15);
    currentSong.song = getValue(https, "name");
    currentSong.artist = getValue(https, "name");
    currentSong.album = getValue(https, "name");
    isPlaying = getValue(https, "is_playing") == "true";

    success = true;
  }

  this->drawScreen(success);

  https.end();
  return success;
}

// Get a value from the response
String SpotifyBuddy::getValue(HTTPClient &https, String key)
{
  // Retrieve and parse value from the response string
  return "";
}

// Draw screen method (can be called for full/partial refresh)
bool SpotifyBuddy::drawScreen(bool fullRefresh, bool likeRefresh)
{
  int rectWidth = 120;
  int rectHeight = 10;

  if (fullRefresh)
  {
    if (SPIFFS.exists("/albumArt.jpg"))
    {
      TJpgDec.setSwapBytes(true);
      TJpgDec.setJpgScale(4);
      TJpgDec.drawFsJpg(26, 5, "/albumArt.jpg");
    }
    else
    {
      TJpgDec.setSwapBytes(false);
      TJpgDec.setJpgScale(1);
      TJpgDec.drawFsJpg(0, 0, "/Angry.jpg");
    }
    // Draw artist and song name
  }

  if (fullRefresh || likeRefresh)
  {
    if (currentSong.isLiked)
    {
      TJpgDec.setJpgScale(1);
      TJpgDec.drawFsJpg(128 - 20, 0, "/heart.jpg");
    }
  }

  return true;
}

// Toggle song play/pause
bool SpotifyBuddy::togglePlay()
{
  String url = "https://api.spotify.com/v1/me/player/" + String(isPlaying ? "pause" : "play");
  isPlaying = !isPlaying;
  bool success = executeRequest(url, "PUT");

  if (success)
  {
    getTrackInfo();
  }
  return success;
}

// Adjust volume
bool SpotifyBuddy::adjustVolume(int vol)
{
  String url = "https://api.spotify.com/v1/me/player/volume?volume_percent=" + String(vol);
  bool success = executeRequest(url, "PUT");

  if (success)
  {
    currVol = vol;
  }
  return success;
}

// Skip forward/next track
bool SpotifyBuddy::skipForward()
{
  String url = "https://api.spotify.com/v1/me/player/next";
  bool success = executeRequest(url, "POST");

  if (success)
  {
    getTrackInfo();
  }
  return success;
}

// Skip back/previous track
bool SpotifyBuddy::skipBack()
{
  String url = "https://api.spotify.com/v1/me/player/previous";
  bool success = executeRequest(url, "POST");

  if (success)
  {
    getTrackInfo();
  }
  return success;
}

// Like a song
bool SpotifyBuddy::likeSong(String songId)
{
  String url = "https://api.spotify.com/v1/me/tracks?ids=" + songId;
  String body = "{\"ids\":[\"" + songId + "\"]}";
  bool success = executeRequest(url, "PUT", body);

  if (success)
  {
    Serial.println("added track to liked songs");
  }
  return success;
}

// Dislike a song
bool SpotifyBuddy::dislikeSong(String songId)
{
  String url = "https://api.spotify.com/v1/me/tracks?ids=" + songId;
  bool success = executeRequest(url, "DELETE");

  if (success)
  {
    Serial.println("removed liked songs");
  }
  return success;
}

bool SpotifyBuddy::isAccessTokenSet()
{
  return accessTokenSet;
}

int SpotifyBuddy::getTokenExpireTime()
{
  return tokenExpireTime;
}

long SpotifyBuddy::getTokenStartTime()
{
  return tokenStartTime;
}