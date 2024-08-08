#ifndef HTTP_SERVICE_h
#define HTTP_SERVICE_h
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

class HttpService
{
public:
  HttpService(const String host);
  bool get(String url, String &response);
  bool get(String url, String &response, int &httpStatusCode);

private:
  String host_;
  HTTPClient http_;
};

#endif
