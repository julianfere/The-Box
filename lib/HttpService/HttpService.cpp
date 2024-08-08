#include "HttpService.h"
#include <ArduinoJson.h>

HttpService::HttpService(const String host)
    : host_(host)
{
}

bool HttpService::get(const String url, String &response)
{
  int httpStatusCode;
  return get(url, response, httpStatusCode);
}

bool HttpService::get(const String url, String &response, int &httpStatusCode)
{
  http_.begin(host_ + url);

  int httpCode = http_.GET();
  httpStatusCode = httpCode;
  if (httpCode > 0)
  {
    response = http_.getString();

    http_.end();
    return httpCode == 200;
  }
  else
  {
    response = http_.errorToString(httpCode).c_str();
    http_.end();
    return false;
  }
}
