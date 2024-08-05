#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

class HttpClient
{
public:
  HttpClient(const String host);
  bool get(String url, String &response);
  bool get(String url, String &response, int &httpStatusCode);
  bool post(String url, String payload, String &response);
  bool post(String url, String payload, String &response, int &httpStatusCode);
  bool patch(String url, String payload, String &response);
  bool patch(String url, String payload, String &response, int &httpStatusCode);

private:
  String host_;
  HTTPClient http_;
};

#endif
