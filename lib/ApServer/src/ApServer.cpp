#include <Arduino.h>
#include "ApServer.h"
#include "Pages.h"
#include <SPIFFS.h>
#include <algorithm>

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", CAPTIVE_PAGE);
  }
};

ApServer::ApServer() : _dnsServer(), _server(80), networks{}, readyToConnect(false)
{
  ssid = "";
  password = "";
}

bool ApServer::networkSelected()
{
  return this->readyToConnect && (this->ssid != "");
}

String ApServer::getSsid()
{
  Serial.print(this->ssid);
  return this->ssid;
}

String ApServer::getPassword()
{
  return this->password;
}

void ApServer::setupAp()
{
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress IP = WiFi.softAPIP();
}

void ApServer::scanNetworks()
{
  int networkCount = WiFi.scanNetworks();

  for (int i = 0; i < networkCount; i++)
  {
    networks.push_back(Network{WiFi.SSID(i), WiFi.RSSI(i)});
  }

  std::sort(networks.begin(), networks.end());
}

void ApServer::setSsid(const String ssid)
{
  this->ssid = ssid;
}

void ApServer::setPassword(const String password)
{
  this->password = password;
}

void ApServer::setReadyToConnect(bool readyToConnect)
{
  this->readyToConnect = readyToConnect;
}

void ApServer::handleHome(AsyncWebServerRequest *request)
{
  String html = MAIN_PAGE_1;

  int total_networks = this->networks.size() > NETWORK_CAP ? NETWORK_CAP : this->networks.size();

  for (int i = 0; i < total_networks; i++)
  {
    html += "<option ";
    html += "value='" + this->networks[i].ssid + "'";
    html += " >";
    html += this->networks[i].ssid;
    html += " - ";
    html += this->networks[i].quality();
    html += "</option>";
  };

  html += MAIN_PAGE_2;

  request->send(200, "text/html", html);
}

void ApServer::setup()
{
  setupAp();

  this->setReadyToConnect(false);
  this->ssid = "";
  this->password = "";

  _dnsServer.start(53, "*", WiFi.softAPIP());

  scanNetworks();

  _server.on("/home", HTTP_GET, [this](AsyncWebServerRequest *request)
             { this->handleHome(request); });

  _server.on("/connect", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
              
    if (request->hasParam("ssid", true) && request->hasParam("password", true))
    {
      Serial.println("SSID and password received");
      this->setSsid(request->getParam("ssid", true)->value());
      this->setPassword(request->getParam("password", true)->value());
      this->setReadyToConnect(true);
    };
    request->send(200, "text/html", CONNECTING_PAGE); });

  _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  _server.begin();
}

void ApServer::handleNextRequest()
{
  _dnsServer.processNextRequest();
}

void ApServer::teardown()
{
  _dnsServer.stop();
  _server.end();
}