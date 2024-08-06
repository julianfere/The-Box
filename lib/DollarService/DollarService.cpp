#include "DollarService.h"

DollarService::DollarService() : _client(new HttpClient(DOLLAR_ENDPOINT))
{
}

DollarInfo DollarService::getCurrent()
{
  DollarInfo info;
  String response;
  bool success = _client->get("", response);
  if (success)
  {
    JsonDocument doc;

    deserializeJson(doc, response);

    info.oficial = doc["oficial"]["value_sell"];
    info.blue = doc["blue"]["value_sell"];
    info.success = true;
  }
  else
  {
    info.success = false;
  }
  return info;
}