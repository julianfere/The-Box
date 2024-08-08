#ifndef WeatherService_h
#define WeatherService_h

#include "Arduino.h"
#include "HttpService.h"

#define LAT -34.6131
#define LONG -58.3772

#define weather_ENDPOINT "https://api.open-meteo.com/v1/forecast"

struct WeatherInfo
{
  float temp;
  float hum;
  float rain;
  String status;
  boolean success;

  String toString()
  {
    return "Temp: " + String(this->temp) + " hum: " + String(this->hum) + " rain: " + String(this->rain) + " success: " + String(this->success);
  };
};

class WeatherService
{
public:
  WeatherService();
  WeatherInfo getCurrent();

private:
  HttpService *_client;
  String parseWeatherCode(int code);
  const String query;
};

#endif