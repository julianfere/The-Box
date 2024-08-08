#ifndef DollarService_H
#define DollarService_H

#include "Arduino.h"
#include "HttpService.h"

#define DOLLAR_ENDPOINT "https://api.bluelytics.com.ar/v2/latest"

struct DollarInfo
{
  float blue;
  float oficial;
  boolean success;

  String toString()
  {
    return "Blue: " + String(this->blue) + " Oficial: " + String(this->oficial) + " success: " + String(this->success);
  };
};

class DollarService
{
public:
  DollarService();
  DollarInfo getCurrent();

private:
  HttpService *_client;
};

#endif