#include "SpotifyBuddy.h"
#include <TFT_eSPI.h>

SpotifyBuddy::SpotifyBuddy(String clientId, String clientSecret)
    : clientId(clientId), clientSecret(clientSecret)
{
  client = std::unique_ptr<WiFiClientSecure>(new WiFiClientSecure());
  client->setInsecure(); // Usa setCACert(spotify_cert) si tienes el certificado de Spotify
}

// Helper to execute POST/GET/PUT requests to Spotify API
bool SpotifyBuddy::executeRequest(String url, String method, String requestBody)
{
  https.begin(*client, url);
  https.addHeader("Authorization", "Bearer " + accessToken);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");
  https.addHeader("Content-Length", String(requestBody.length())); // Soluciona error 411

  int httpResponseCode;
  if (method == "POST")
    httpResponseCode = https.POST(requestBody);
  else if (method == "PUT")
    httpResponseCode = https.PUT(requestBody);
  else if (method == "GET")
    httpResponseCode = https.GET();
  else
    return false;

  Serial.println("HTTP Response Code: " + String(httpResponseCode));

  if (httpResponseCode == HTTP_CODE_OK)
  {
    String response = https.getString();
    Serial.println(response);
  }

  https.end();
  return httpResponseCode == HTTP_CODE_OK;
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

String SpotifyBuddy::urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (isalnum(c))
    {
      encodedString += c;
    }
    else
    {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9)
      {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

bool SpotifyBuddy::requestAuth(String serverCode, bool isRefresh)
{
  HTTPClient https;
  String url = "https://accounts.spotify.com/api/token";
  String credentials = this->clientId + ":" + this->clientSecret;
  String auth = "Basic " + base64::encode(credentials);
  String requestBody = isRefresh ? "grant_type=refresh_token&refresh_token=" + refreshToken
                                 : "grant_type=authorization_code&code=" + serverCode + "&redirect_uri=" + urlencode(this->callbackUrl);
  https.begin(url);
  https.addHeader("Authorization", auth);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");
  https.addHeader("Content-Length", String(requestBody.length())); // Soluciona error 411

  int httpResponseCode = https.POST(requestBody);
  Serial.println("Auth request HTTP Code: " + String(httpResponseCode));

  if (httpResponseCode == 200)
  {
    String response = https.getString();
    Serial.println("Response: " + response);
    JsonDocument doc;
    deserializeJson(doc, response);
    accessToken = doc["access_token"].as<String>();
    refreshToken = doc["refresh_token"].as<String>();
    tokenExpireTime = doc["expires_in"].as<int>();
    tokenStartTime = millis();
    accessTokenSet = true;
  }
  else
  {
    Serial.println("Auth request failed. Response: " + https.getString());
  }

  https.end();
  return accessTokenSet;
}

bool SpotifyBuddy::findLikedStatus(String songId)
{
  String url = "https://api.spotify.com/v1/me/tracks/contains?ids=" + songId;
  https.begin(*client, url);
  String auth = "Bearer " + String(this->accessToken);
  https.addHeader("Authorization", auth);
  https.addHeader("Content-Type", "application/json");
  int httpResponseCode = https.GET();
  bool success = false;
  // Check if the request was successful
  if (httpResponseCode == 200)
  {
    String response = https.getString();
    https.end();
    return (response == "[ true ]");
  }
  else
  {
    Serial.print("Error toggling liked songs: ");
    Serial.println(httpResponseCode);
    String response = https.getString();
    Serial.println(response);
    https.end();
  }

  return success;
}

SongDetails SpotifyBuddy::getTrackInfo()
{
  Serial.println("Getting track info");
  String url = "https://api.spotify.com/v1/me/player/currently-playing";
  https.begin(*client, url);
  String auth = "Bearer " + String(accessToken);
  bool refresh = false;
  https.addHeader("Authorization", auth);
  String imageLink = "";

  int httpResponseCode = https.GET();
  bool success = false;

  SongDetails song;
  if (httpResponseCode == 200)
  {
    String response = https.getString();
    JsonDocument doc;
    deserializeJson(doc, response);

    currentSongPositionMs = doc["progress_ms"].as<float>();
    song.durationMs = doc["item"]["duration_ms"].as<int>();
    song.Id = doc["item"]["id"].as<String>();
    song.song = doc["item"]["name"].as<String>();
    song.artist = doc["item"]["artists"][0]["name"].as<String>();
    song.album = doc["item"]["album"]["name"].as<String>();
    imageLink = doc["item"]["album"]["images"][0]["url"].as<String>();
    song.imageLink = imageLink;
    song.isLiked = findLikedStatus(song.Id);
    song.isPlaying = doc["is_playing"].as<bool>();
    song.progressMs = currentSongPositionMs;
    isPlaying = doc["is_playing"].as<bool>();
    Serial.println("Song: " + song.song);
    //   if (SPIFFS.exists("/albumArt.jpg") && this->lastImgUrl != imageLink)
    //   {
    //     SPIFFS.remove("/albumArt.jpg");
    //     this->getFile(imageLink, "/albumArt.jpg");
    //   }
    //   this->lastImgUrl = imageLink;
  }

  https.end();
  return song;
}

bool SpotifyBuddy::toggleLiked(String songId)
{
  String url = "https://api.spotify.com/v1/me/tracks/contains?ids=" + songId;
  https.begin(*client, url);
  String auth = "Bearer " + String(accessToken);
  https.addHeader("Authorization", auth);
  https.addHeader("Content-Type", "application/json");
  int httpResponseCode = https.GET();
  bool success = false;

  if (httpResponseCode == 200)
  {
    String response = https.getString();
    https.end();
    if (response == "[ true ]")
    {
      currentSong.isLiked = false;
      dislikeSong(songId);
    }
    else
    {
      currentSong.isLiked = true;
      likeSong(songId);
    }
    Serial.println(response);
    success = true;
  }
  else
  {
    Serial.print("Error toggling liked songs: ");
    Serial.println(httpResponseCode);
    String response = https.getString();
    Serial.println(response);
    https.end();
  }

  return success;
}

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
  Serial.println("Adjusting volume" + String(vol));
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

void SpotifyBuddy::setCallbackUrl(String url)
{
  callbackUrl = url;
}

bool SpotifyBuddy::getFile(String url, String filename)
{

  if (SPIFFS.exists(filename))
  {
    Serial.println("Found " + filename);
    return true; // Archivo ya existe
  }
  if (this->lastImgUrl == url)
  {
    Serial.println("Already downloaded " + filename);
    return true; // Archivo ya descargado
  }

  Serial.println("Downloading " + filename + " from " + url);

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected!");
    return false;
  }

  HTTPClient http;
  http.begin(*client, url);

  int httpCode = http.GET();
  if (httpCode <= 0)
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
  }

  if (httpCode != HTTP_CODE_OK)
  {
    Serial.printf("[HTTP] Error: %d\n", httpCode);
    http.end();
    return false;
  }

  fs::File f = SPIFFS.open(filename, "w");
  if (!f)
  {
    Serial.println("Failed to open file for writing!");
    http.end();
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  int total = http.getSize();
  int len = total;
  uint8_t buff[512]; // Aumentar buffer mejora rendimiento

  Serial.println("[HTTP] Downloading...");

  unsigned long lastMillis = millis();
  while (http.connected() && (len > 0 || len == -1))
  {
    size_t size = stream->available();
    if (size)
    {
      int c = stream->readBytes(buff, min(size, sizeof(buff)));
      f.write(buff, c);
      if (len > 0)
        len -= c;
      lastMillis = millis();
    }

    if (millis() - lastMillis > 10000) // Timeout de 10s para evitar cuelgues
    {
      Serial.println("Timeout downloading file!");
      break;
    }

    delay(1); // Ayuda a evitar bloqueos
  }

  Serial.println("[HTTP] Download complete.");
  f.close();
  http.end();
  return true;
}
