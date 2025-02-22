#include <Arduino.h>
#include "ApServer.h"
#include "Pages.h"
#include <algorithm>

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    return true; // Maneja todas las solicitudes
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    Serial.println("CaptiveRequestHandler: Redirigiendo a la página de inicio");
    request->send(200, "text/html", CAPTIVE_PAGE); // Envía la página de inicio
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
  return this->ssid;
}

String ApServer::getPassword()
{
  return this->password;
}

void ApServer::setupAp()
{
  Serial.println("Iniciando modo AP...");
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("AP IP address: " + IP.toString());
}

void ApServer::scanNetworks()
{
  Serial.println("Escaneando redes Wi-Fi...");
  int networkCount = WiFi.scanNetworks();

  for (int i = 0; i < networkCount; i++)
  {
    networks.push_back(Network{WiFi.SSID(i), WiFi.RSSI(i)});
  }

  std::sort(networks.begin(), networks.end());
  Serial.println("Escaneo completado. " + String(networks.size()) + " redes encontradas.");
}

void ApServer::setSsid(const String ssid)
{
  this->ssid = ssid;
  Serial.println("SSID establecido: " + ssid);
}

void ApServer::setPassword(const String password)
{
  this->password = password;
  Serial.println("Contraseña establecida.");
}

void ApServer::setReadyToConnect(bool readyToConnect)
{
  this->readyToConnect = readyToConnect;
  Serial.println("Listo para conectar: " + String(readyToConnect ? "Sí" : "No"));
}

void ApServer::handleHome(AsyncWebServerRequest *request)
{
  Serial.println("Manejando solicitud /home");
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

  // Iniciar el servidor DNS captive portal
  _dnsServer.start(53, "*", WiFi.softAPIP());
  Serial.println("Servidor DNS captive portal iniciado.");

  // Escanear redes Wi-Fi disponibles
  scanNetworks();

  // Configurar rutas del servidor
  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    Serial.println("Solicitud GET recibida en /");
    this->handleHome(request); });

  _server.on("/connect", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    Serial.println("Solicitud POST recibida en /connect");
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      this->setSsid(request->getParam("ssid", true)->value());
      this->setPassword(request->getParam("password", true)->value());
      this->setReadyToConnect(true);
      Serial.println("Credenciales recibidas: SSID=" + this->ssid + ", Password=" + this->password);
    }
    request->send(200, "text/html", CONNECTING_PAGE); });

  // Añadir el manejador captive portal
  _server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  Serial.println("Manejador captive portal añadido.");

  // Iniciar el servidor
  _server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void ApServer::handleNextRequest()
{
  _dnsServer.processNextRequest();
}

void ApServer::teardown()
{
  _dnsServer.stop();
  _server.end();
  Serial.println("Servidor AP y DNS detenidos.");
}