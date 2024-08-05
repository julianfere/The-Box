#ifndef WheatherService_h
#define WheatherService_h

#include "Arduino.h"
#include "HttpClient.h"

#define LAT -34.6131
#define LONG -58.3772

#define WHEATHER_ENDPOINT "https://api.open-meteo.com/v1/forecast"

struct WheatherInfo
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

class WheatherService
{
public:
  WheatherService();
  WheatherInfo getCurrent();

private:
  HttpClient *_client;
  String parseWheatherCode(int code);
  const String query;
};

#endif