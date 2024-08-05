#include "WheatherService.h"
#include <ArduinoJson.h>

WheatherService::WheatherService() : _client(new HttpClient(WHEATHER_ENDPOINT)), query("?latitude=-34.6131&longitude=-58.3772&current=temperature_2m,relative_humidity_2m,rain,weather_code&timezone=auto")
{
}

WheatherInfo WheatherService::getCurrent()
{
  WheatherInfo info;
  String response;

  bool success = _client->get(query, response);

  Serial.println("Success" + String(success));

  info.success = success;

  if (success)
  {

    JsonDocument doc;

    deserializeJson(doc, response);

    info.hum = doc["current"]["relative_humidity_2m"];
    info.temp = doc["current"]["temperature_2m"];
    info.rain = doc["current"]["rain"];
    info.status = this->parseWheatherCode(doc["current"]["weather_code"]);
  }
  return info;
}

// 0	Clear sky
// 1, 2, 3	Mainly clear, partly cloudy, and overcast
// 45, 48	Fog and depositing rime fog
// 51, 53, 55	Drizzle: Light, moderate, and dense intensity
// 56, 57	Freezing Drizzle: Light and dense intensity
// 61, 63, 65	Rain: Slight, moderate and heavy intensity
// 66, 67	Freezing Rain: Light and heavy intensity
// 71, 73, 75	Snow fall: Slight, moderate, and heavy intensity
// 77	Snow grains
// 80, 81, 82	Rain showers: Slight, moderate, and violent
// 85, 86	Snow showers slight and heavy
// 95 *	Thunderstorm: Slight or moderate
// 96, 99 *	Thunderstorm with slight and heavy hail

String WheatherService::parseWheatherCode(int code)
{
  String status = "";

  if (code >= 0 && code <= 3)
  {
    status = "Limpio";
  }

  if (code >= 45 && code <= 48)
  {
    status = "Niebla";
  }

  if (code >= 51 && code <= 55)
  {
    status = "Llovizna";
  }

  if (code >= 61 && code <= 67)
  {
    status = "Lluvia";
  }

  if (code >= 80 && code <= 99)
  {
    status = "Tormenta";
  }

  return status;
}