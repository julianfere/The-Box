#include "HttpClient.h"
#include <ArduinoJson.h>

HttpClient::HttpClient(const String host)
    : host_(host)
{
}

bool HttpClient::get(const String url, String &response)
{
  int httpStatusCode;
  return get(url, response, httpStatusCode);
}

bool HttpClient::get(const String url, String &response, int &httpStatusCode)
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

bool HttpClient::post(const String url, const String payload, String &response)
{
  int httpStatusCode;
  return post(url, payload, response, httpStatusCode);
}

bool HttpClient::post(const String url, const String payload, String &response, int &httpStatusCode)
{
  http_.begin(host_ + url);
  http_.addHeader("Content-Type", "application/json");

  int httpCode = http_.POST(payload);
  httpStatusCode = httpCode;

  if (httpCode > 0)
  {
    response = http_.getString();
    http_.end();
    return httpCode == 200 || httpCode == 201 || httpCode == 204;
  }
  else
  {
    response = http_.errorToString(httpCode).c_str();
    http_.end();
    return false;
  }
}

bool HttpClient::patch(const String url, const String payload, String &response)
{
  int httpStatusCode;
  return patch(url, payload, response, httpStatusCode);
}

bool HttpClient::patch(const String url, const String payload, String &response, int &httpStatusCode)
{
  http_.begin(host_ + url);
  http_.addHeader("Content-Type", "application/json");

  int httpCode = http_.PATCH(payload);
  httpStatusCode = httpCode;

  if (httpCode > 0)
  {
    response = http_.getString();
    http_.end();
    return httpCode == 200 || httpCode == 201 || httpCode == 204;
  }
  else
  {
    response = http_.errorToString(httpCode).c_str();
    http_.end();
    return false;
  }
}