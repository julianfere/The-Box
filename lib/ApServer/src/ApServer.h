/*
  ApServer.h - Library for creating an Access Point with a web server.
*/
#ifndef ApServer_h
#define ApServer_h
#include <Arduino.h>
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include <WiFi.h>
#include <vector>

#define AP_SSID "TheBox"
#define AP_PASSWORD "TheBox123"

#define NETWORK_CAP 10

struct Network
{
  String ssid;
  int rssi;

  bool operator<(const Network &rhs) const
  {
    return rssi > rhs.rssi;
  }

  String quality()
  {
    if (rssi > -50)
    {
      return "Excelente";
    }
    else if (rssi > -60)
    {
      return "Buena";
    }
    else if (rssi > -70)
    {
      return "Regular";
    }
    else if (rssi > -80)
    {
      return "Mala";
    }
    else
    {
      return "Muy mala";
    }
  }
};

class ApServer
{

public:
  ApServer();
  void setup();
  void handleNextRequest();
  bool networkSelected();
  void teardown();
  String getSsid();
  String getPassword();
  void setSsid(String ssid);
  void setPassword(String password);

private:
  DNSServer _dnsServer;
  AsyncWebServer _server;
  bool readyToConnect;
  std::vector<Network> networks;
  void setupAp();
  void scanNetworks();
  void setReadyToConnect(bool readyToConnect);
  void handleHome(AsyncWebServerRequest *request);
  String ssid;
  String password;
};
#endif