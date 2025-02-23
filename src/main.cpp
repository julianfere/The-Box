#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TFT_eSPI.h>
#include "ApServer.h"
#include "Store.h"
#include "WeatherService.h"
#include "DollarService.h"
#include "AnalogHandler.h"
#include "StateMachine.h"
#include "DisplayManager.h"
#include "index.h"
#include "SpotifyBuddy.h"
//==============================================================================

#define _INIT "INIT"
#define _WIFI_DISCOVER "WIFI_DISCOVER"
#define _WIFI_CONNECT "WIFI_CONNECT"
#define _MENU "MENU"
#define _CLOCK "CLOCK"
#define _SCREEN_SAVER "SCREEN_SAVER"
#define _WEATHER "WEATHER"
#define _DOLAR "DOLAR"
#define _SPOTIFY "SPOTIFY"
#define CLIENT_ID "21977de3dafb4ad69b2610d43783ec69"
#define CLIENT_SECRET "68a061bc68b24eb88f44184f5079dd82"
#define REDIRECT_URI "http://192.168.4.1/callback"

const int xPin = 27;  // the VRX attach to
const int yPin = 32;  // the VRY attach to
const int swPin = 25; // the SW attach to

//==============================================================================
AnalogHandler analogHandler = AnalogHandler(xPin, yPin, swPin);

TFT_eSPI tft = TFT_eSPI(); // Inicializa la pantalla
ApServer apServer;
Store store = Store();
WeatherService weatherService = WeatherService();
WiFiUDP ntpUDP;
DollarService dollarService = DollarService();
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);
WeatherInfo WEATHER_INFO;
DollarInfo DOLLAR_INFO;
DisplayManager displayManager = DisplayManager();
SpotifyBuddy spotifyBuddy(CLIENT_ID, CLIENT_SECRET, REDIRECT_URI);
QueueHandle_t WIFI_STATUS_QUEUE = xQueueCreate(1, sizeof(bool));
AsyncWebServer httpServer(8080);
bool serverOn = true;

//==============================================================================

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height())
    return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

const int numOptions = 5;
const char *menuOptions[numOptions] = {
    "Dolar",
    "Reloj",
    "Clima",
    "Salva Pantallas",
    "Spotify",
};
int selectedOption = -1;
int newOption = 0;

enum StateTransitions : int
{
  INIT = 0,
  WIFI_DISCOVER = 1,
  WIFI_CONNECT = 2,
  MENU = 3,
  CLOCK = 4,
  SCREEN_SAVER = 5,
  WEATHER = 6,
  DOLAR = 7,
  SPOTIFY = 8,
  CLOCK_SCREEN = 9,
};

void onEnterInit(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Iniciando...");
  Serial.println("=========================================================================");

  displayManager.drawInitPage();

  if (store.hasWifiSettings())
  {
    Serial.println("store tiene wifi settings");
    machine.changeState(StateTransitions::WIFI_CONNECT);
  }
  else
  {
    Serial.println("store NO tiene wifi settings");

    machine.changeState(StateTransitions::WIFI_DISCOVER);
  }
}

void onEnterWaiting(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Discovery...");
  Serial.println("=========================================================================");
  displayManager.drawWifiDiscoveryPage();
}

void onStateWaiting(StateMachine &machine)
{
  apServer.handleNextRequest();

  if (apServer.networkSelected())
  {
    machine.changeState(StateTransitions::WIFI_CONNECT);
  }
}

void onExitWaiting(StateMachine &machine)
{
  apServer.teardown();
}

void onEnterConnecting(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Connecting...");
  Serial.println("=========================================================================");
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.fillScreen(TFT_BLACK);
  yield();
  tft.println("Conectando...");
  String ssid;
  String password;

  if (store.hasWifiSettings())
  {
    WifiSettings settints = store.getWifiSettings();
    ssid = settints.ssid;
    password = settints.password;
  }
  else
  {
    ssid = apServer.getSsid();
    password = apServer.getPassword();
  }

  WiFi.begin(ssid.c_str(), password.c_str());
}

void onStateConnecting(StateMachine &machine)
{
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
  {
    Serial.printf("Conectando, tiempo restante: %d\n", 10 - (millis() - startTime) / 1000);
    yield();    // Permite que el servidor HTTP siga funcionando
    delay(100); // Pequeño delay para no saturar el CPU
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    WifiSettings settings = WifiSettings{apServer.getSsid(), apServer.getPassword()};
    store.storeWifiSettings(settings);
    delay(100); // Pequeño delay para no saturar el CPU
    machine.changeState(StateTransitions::MENU);
  }
  else
  {
    machine.changeState(StateTransitions::WIFI_DISCOVER);
  }
}

void handleRoot(AsyncWebServerRequest *request)
{
  Serial.println("handling root");

  String page = String(mainPage);

  // Reemplazar %client_id% con CLIENT_ID
  page.replace("%client_id%", CLIENT_ID);

  // Reemplazar %redirect_uri% con REDIRECT_URI
  page.replace("%redirect_uri%", REDIRECT_URI);

  request->send(200, "text/html", page);
}

void handleCallback(AsyncWebServerRequest *request)
{
  if (!spotifyBuddy.isAccessTokenSet())
  {
    if (request->arg("code") == "")
    { // Parameter not found
      char page[500];
      sprintf(page, errorPage, CLIENT_ID, REDIRECT_URI);
      request->send(200, "text/html", String(page)); // Send web page
    }
    else
    { // Parameter found
      if (spotifyBuddy.getUserCode(request->arg("code")))
      {
        request->send(200, "text/html", "Spotify setup complete Auth refresh in :" + String(spotifyBuddy.getTokenExpireTime()));
      }
      else
      {
        char page[500];
        sprintf(page, errorPage, CLIENT_ID, REDIRECT_URI);
        request->send(200, "text/html", String(page)); // Send web page
      }
    }
  }
  else
  {
    request->send(200, "text/html", "Spotify setup complete");
  }
}

void onEnterConnected(StateMachine &machine)
{
  displayManager.drawMenu(numOptions, selectedOption, menuOptions);
}

void onMenu(StateMachine &machine)
{
  if (analogHandler.isDown())
  {
    if (newOption == numOptions - 1)
    {
      newOption = 0;
    }
    else
    {
      newOption++;
    }
  }
  else if (analogHandler.isUp())
  {
    if (newOption == 0)
    {
      newOption = numOptions - 1;
    }
    else
    {
      newOption--;
    }
  }

  if (selectedOption != newOption)
  {
    selectedOption = newOption;
    displayManager.drawMenu(numOptions, selectedOption, menuOptions);
  }

  if (analogHandler.isPressed())
  {
    Serial.printf("Selected option: %d\n", selectedOption);
    switch (selectedOption)
    {
    case 0:
      machine.changeState(StateTransitions::DOLAR);
      break;
    case 1:
      machine.changeState(StateTransitions::CLOCK_SCREEN);
      break;
    case 2:
      machine.changeState(StateTransitions::WEATHER);
      break;
    case 3:
      machine.changeState(StateTransitions::SCREEN_SAVER);
      break;
    case 4:
      machine.changeState(StateTransitions::SPOTIFY);
      break;
    default:
      Serial.println("Invalid option");
      break;
    }
  }
  delay(100);
}

void onExitMenu(StateMachine &machine)
{
  tft.fillScreen(TFT_BLACK);
  delay(100);
}

void onEnterDolar(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Dolar...");
  Serial.println("=========================================================================");
  DOLLAR_INFO = dollarService.getCurrent();
  displayManager.drawDolarPage(DOLLAR_INFO);
}

void onDolar(StateMachine &machine)
{
  if (analogHandler.isPressed())
  {
    machine.changeState(StateTransitions::MENU);
  }
}

void onEnterWeather(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Clima...");
  Serial.println("=========================================================================");
  WEATHER_INFO = weatherService.getCurrent();
  displayManager.drawWeatherPage(WEATHER_INFO);
}

void onWeather(StateMachine &machine)
{
  if (analogHandler.isPressed())
  {
    machine.changeState(StateTransitions::MENU);
  }
}

void onEnterClock(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Reloj...");
  Serial.println("=========================================================================");
  tft.fillScreen(TFT_BLACK);
  yield();
}

void onClock(StateMachine &machine)
{
  if (analogHandler.isPressed())
  {
    machine.changeState(StateTransitions::MENU);
  }
  displayManager.drawClockPage(timeClient);
}

void onEnterScreenSaver(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Salva Pantallas...");
  Serial.println("=========================================================================");
}

#define TEXT_HEIGHT 8    // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 0 // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TOP_FIXED_AREA 0 // Number of lines in top fixed area (lines counted from top of screen)

uint16_t yStart = TOP_FIXED_AREA;
uint16_t yArea = 320 - TOP_FIXED_AREA - BOT_FIXED_AREA;
uint16_t yDraw = 320 - BOT_FIXED_AREA - TEXT_HEIGHT;
byte pos[42];
uint16_t xPos = 0;

void setupScrollArea(uint16_t TFA, uint16_t BFA)
{
  tft.writecommand(ILI9341_VSCRDEF); // Vertical scroll definition
  tft.writedata(TFA >> 8);
  tft.writedata(TFA);
  tft.writedata((320 - TFA - BFA) >> 8);
  tft.writedata(320 - TFA - BFA);
  tft.writedata(BFA >> 8);
  tft.writedata(BFA);
}

void scrollAddress(uint16_t VSP)
{
  tft.writecommand(ILI9341_VSCRSADD); // Vertical scrolling start address
  tft.writedata(VSP >> 8);
  tft.writedata(VSP);
}

int scroll_slow(int lines, int wait)
{
  int yTemp = yStart;
  for (int i = 0; i < lines; i++)
  {
    yStart++;
    if (yStart == 320 - BOT_FIXED_AREA)
      yStart = TOP_FIXED_AREA;
    scrollAddress(yStart);
    delay(wait);
  }
  return yTemp;
}

void onScreenSaver(StateMachine &machine)
{
  // First fill the screen with random streaks of characters
  for (int j = 0; j < 600; j += TEXT_HEIGHT)
  {
    for (int i = 0; i < 40; i++)
    {
      if (analogHandler.isPressed())
      {
        goto exit;
      }
      if (pos[i] > 20)
        pos[i] -= 3; // Rapid fade initially brightness values
      if (pos[i] > 0)
        pos[i] -= 1; // Slow fade later
      if ((random(20) == 1) && (j < 400))
        pos[i] = 63;                                // ~1 in 20 probability of a new character
      tft.setTextColor(pos[i] << 5, ILI9341_BLACK); // Set the green character brightness
      if (pos[i] == 63)
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);      // Draw white character
      xPos += tft.drawChar(random(32, 128), xPos, yDraw, 1); // Draw the character
    }
    yDraw = scroll_slow(TEXT_HEIGHT, 14); // Scroll, 14ms per pixel line
    xPos = 0;
  }
  //  QUE HACES FLACO
  if (false)
  {
  exit:
    machine.changeState(StateTransitions::MENU);
  }
}

void onExitScreenSaver(StateMachine &machine)
{
  tft.fillScreen(TFT_BLACK);
  yield();
  setupScrollArea(0, 0);
  scrollAddress(0);
}

int vol = 0;

void onEnterSpotify(StateMachine &machine)
{
  Serial.println("=========================================================================");
  Serial.println("Spotify...");
  Serial.println("=========================================================================");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Algun dia");
  vol = 100;
}

long refreshLoop;

void onSpotify(StateMachine &machine)
{
  if (spotifyBuddy.isAccessTokenSet())
  {
    if (serverOn)
    {
      httpServer.end();
      serverOn = false;
    }

    if (millis() - spotifyBuddy.getTokenStartTime() / 1000 > spotifyBuddy.getTokenExpireTime())
    {

      spotifyBuddy.refreshAuth();
    }

    if (millis() - refreshLoop > 5000)
    {
      spotifyBuddy.getTrackInfo();
      refreshLoop = millis();
    }

    if (analogHandler.isPressed())
    {
      spotifyBuddy.togglePlay();
    }

    if (analogHandler.isDown())
    {
      spotifyBuddy.skipForward();
    }

    if (analogHandler.isUp())
    {
      spotifyBuddy.skipBack();
    }

    if (analogHandler.isLeft())
    {
      if (vol > 0)
      {
        vol -= 10;
        spotifyBuddy.adjustVolume(-10);
      }
    }

    if (analogHandler.isRight())
    {
      if (vol < 100)
      {
        vol += 10;
        spotifyBuddy.adjustVolume(10);
      }
    }

    if (analogHandler.isDoublePressed())
    {
      machine.changeState(StateTransitions::MENU);
    }
  }
}

DState states[] = {
    {_INIT, onEnterInit, NULL, NULL, 0},
    {_WIFI_DISCOVER, onEnterWaiting, onStateWaiting, onExitWaiting, 0},
    {_WIFI_CONNECT, onEnterConnecting, onStateConnecting, NULL, 0},
    {_MENU, onEnterConnected, onMenu, onExitMenu, 0},
    {_CLOCK, onEnterClock, onClock, NULL, 0},
    {_SCREEN_SAVER, onEnterScreenSaver, onScreenSaver, onExitScreenSaver, 0},
    {_WEATHER, onEnterWeather, onWeather, NULL, 0},
    {_DOLAR, onEnterDolar, onDolar, NULL, 0},
    {_SPOTIFY, onEnterSpotify, onSpotify, NULL, 0},
};

DTransition transitions[] = {
    {_INIT, StateTransitions::WIFI_DISCOVER, _WIFI_DISCOVER},
    {_INIT, StateTransitions::WIFI_CONNECT, _WIFI_CONNECT},
    {_WIFI_DISCOVER, StateTransitions::WIFI_CONNECT, _WIFI_CONNECT},
    {_WIFI_CONNECT, StateTransitions::WIFI_DISCOVER, _WIFI_DISCOVER},
    {_WIFI_CONNECT, StateTransitions::MENU, _MENU},
    {_MENU, StateTransitions::CLOCK_SCREEN, _CLOCK},
    {_MENU, StateTransitions::SCREEN_SAVER, _SCREEN_SAVER},
    {_MENU, StateTransitions::WEATHER, _WEATHER},
    {_MENU, StateTransitions::DOLAR, _DOLAR},
    {_MENU, StateTransitions::SPOTIFY, _SPOTIFY},
    {_CLOCK, StateTransitions::MENU, _MENU},
    {_SCREEN_SAVER, StateTransitions::MENU, _MENU},
    {_WEATHER, StateTransitions::MENU, _MENU},
    {_DOLAR, StateTransitions::MENU, _MENU},
    {_SPOTIFY, StateTransitions::MENU, _MENU},
    {_CLOCK, StateTransitions::MENU, _MENU},
};

StateMachine stateMachine(states, 9, transitions, 16);

void setup()
{
  Serial.begin(115200);
  displayManager.init();
  apServer.setup();
  delay(500);
  Serial.println("Starting HTTP server");
  httpServer.on("/", HTTP_GET, handleRoot);
  httpServer.on("/callback", handleCallback);
  httpServer.begin();
  Serial.println("HTTP server started");
  stateMachine.begin(_INIT);
  analogHandler.setup();

  Serial.println(WiFi.localIP());

  TJpgDec.setJpgScale(4);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

void loop()
{
  stateMachine.update();
}