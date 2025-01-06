#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SimpleFSM.h>
#include <TFT_eSPI.h>
#include "State.h"
#include "ApServer.h"
#include "Store.h"
#include "WeatherService.h"
#include "DollarService.h"
#include "AnalogHandler.h"
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

#define BUTTON_1_PIN 12
#define BUTTON_2_PIN 13

const int xPin = 27;  // the VRX attach to
const int yPin = 32;  // the VRY attach to
const int swPin = 25; // the SW attach to

//==============================================================================
AnalogHandler analogHandler = AnalogHandler(xPin, yPin, swPin);

TFT_eSPI tft = TFT_eSPI(); // Inicializa la pantalla
SimpleFSM machine = SimpleFSM();
ApServer apServer;
Store store = Store();
WeatherService weatherService = WeatherService();
WiFiUDP ntpUDP;
DollarService dollarService = DollarService();
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);
WeatherInfo WEATHER_INFO;
DollarInfo DOLLAR_INFO;

QueueHandle_t WIFI_STATUS_QUEUE = xQueueCreate(1, sizeof(bool));

//==============================================================================

int TRIES = 10;
bool SHOW_DOLLAR = false;

enum Triggers
{
  INIT = 1,
  WIFI_DISCOVER = 2,
  WIFI_CONNECT = 3,
  DOLAR = 4,
  WEATHER = 5,
  MENU = 6,
  SCREEN_SAVER = 7,
  CLOCK_SCREEN = 8,
  SPOTIFY = 9,
};

#pragma region Ui functions
const int numOptions = 5;
const char *menuOptions[numOptions] = {
    "Dolar",
    "Reloj",
    "Clima",
    "Salva Pantallas",
    "Spotify",
};
int selectedOption = 0;
int newOption = 0;

#define COLOR_BLUE tft.color565(0, 0, 255)
#define COLOR_BLACK tft.color565(0, 0, 0)
#define COLOR_CYAN tft.color565(0, 255, 255)
#define COLOR_LIGHT_BLUE tft.color565(173, 216, 230)
#define COLOR_YELLOW tft.color565(255, 255, 0)
#define COLOR_WHITE tft.color565(255, 255, 255)

void drawHeader(const char *title)
{
  // Title "Menu" (larger size than options)
  tft.setTextColor(COLOR_BLUE);
  tft.setTextSize(2);  // Increase size of title
  tft.setCursor(0, 0); // Center title horizontally
  tft.print(title);

  // Draw lines (adjusted for spacing with the title)
  tft.drawLine(0, 40, 250, 40, COLOR_BLUE);   // Horizontal line
  tft.drawLine(210, 0, 210, 40, COLOR_BLUE);  // Vertical line
  tft.drawLine(250, 40, 320, 40, COLOR_BLUE); // Horizontal line

  // Custom label "TheBox" (larger size than options)
  tft.setCursor(220, 0);
  tft.setTextColor(tft.color565(255, 102, 67)); // Custom color for "TheBox"
  tft.print("TheBox");
  tft.setTextColor(COLOR_WHITE); // Custom color for "TheBox"
}

void drawMenu()
{
  int verticalSpacing = tft.fontHeight() + 4; // Vertical spacing between options, increased for readability
  tft.fillScreen(COLOR_BLACK);                // Clear screen with black background

  drawHeader("Menu");
  tft.setTextColor(COLOR_WHITE); // Custom color for "TheBox"

  int yOffset = 60; // Initial vertical position for options (below the title)

  // Draw menu options with selection effect
  for (int i = 0; i < numOptions; i++)
  {
    if (i == selectedOption)
    {
      // Highlight selected option with yellow background (full width)
      tft.drawRect(0, yOffset - 2, tft.width(), verticalSpacing, COLOR_CYAN);
    }

    // Position each option centered horizontally
    tft.setCursor((tft.width() - tft.textWidth(menuOptions[i])) / 2, yOffset);
    tft.setTextFont(2); // Use a larger font for options
    tft.print(menuOptions[i]);
    yOffset += verticalSpacing; // Move down for the next option
  }
}

void drawInitPage()
{
  drawHeader("");
  tft.fillScreen(TFT_BLACK);

  // Título centrado
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2); // Fuente pequeña y moderna
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Inicializando...", tft.width() / 2, tft.height() / 2);
}

void drawWifiDiscoveryPage()
{
  tft.fillScreen(TFT_BLACK);
  drawHeader("WiFi");

  // Título centrado
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Buscando Redes", tft.width() / 2, tft.height() / 2 - 10);

  // Texto pequeño para la descripción
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(1);
  tft.setCursor(30, tft.height() / 2 + 20);
  tft.println("Escaneando redes...");
}

void drawWifiConnectionProgress()
{
  tft.fillScreen(TFT_BLACK);
  drawHeader("WiFi");
  // Título centrado y simple
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Conectando WiFi", tft.width() / 2, 30);

  // Progreso sencillo, no sobrecargar
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(1);
  tft.setCursor(30, tft.height() / 2);
  tft.println("Esperando...");
}

void drawDolarPage(bool loading = false)
{
  tft.fillScreen(TFT_BLACK);
  drawHeader("Dolar");
  // Título centrado y pequeño
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (loading)
  {
    tft.setTextFont(2);
    tft.setCursor(20, 80);
    tft.println("Cargando...");
    return;
  }

  // Sección Dólar Blue y Oficial
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(20, 80);
  tft.println("Blue: " + String(DOLLAR_INFO.blue));

  tft.setCursor(20, 110);
  tft.println("Oficial: " + String(DOLLAR_INFO.oficial));
}

void drawWeatherPage(bool loading = false)
{
  tft.fillScreen(TFT_BLACK);
  drawHeader("Clima");

  if (loading)
  {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(20, 80);
    tft.println("Cargando...");
    return;
  }

  // Título centrado
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(4);

  // Datos del clima con poco texto y espacio
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setCursor(20, 80);
  tft.println("Cielo: " + WEATHER_INFO.status);

  tft.setCursor(20, 110);
  tft.println("Temp: " + String(WEATHER_INFO.temp) + "°C");

  tft.setCursor(20, 140);
  tft.println("Humedad: " + String(WEATHER_INFO.hum) + "%");

  tft.setCursor(20, 170);
  tft.println("Lluvia: " + String(WEATHER_INFO.rain) + "%");
}

#pragma endregion

#pragma region State Handlers
//==============================================================================

void initeState()
{
  drawInitPage();
  if (store.hasWifiSettings())
  {
    // Carga la configuracion del wifi e intenta conectarse a la red
    WifiSettings wifiSettings = store.getWifiSettings();
    apServer.setSsid(wifiSettings.ssid);
    apServer.setPassword(wifiSettings.password);

    Serial.println("Conectando a red guardada");

    machine.trigger(Triggers::WIFI_CONNECT);
  }
  else
  {
    machine.trigger(Triggers::WIFI_DISCOVER);
  }
}

#pragma region Wifi Discovery State

void onEnterwifiDiscoveryState()
{
  apServer.setup();
  drawWifiDiscoveryPage();
  sleep(100);
}

void wifiDiscoveryState()
{
  apServer.handleNextRequest();

  if (apServer.networkSelected())
  {
    machine.trigger(Triggers::WIFI_CONNECT);
  }
}

void onExitWifiDiscoveryState()
{
  apServer.teardown();
}
#pragma endregion
#pragma region Connect State

void connectTask(void *pvParameters)
{
  bool isConnected = false;

  WiFi.begin(apServer.getSsid(), apServer.getPassword());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }
  isConnected = true;

  // Envía el estado de conexión a la cola
  xQueueSend(WIFI_STATUS_QUEUE, &isConnected, portMAX_DELAY);

  vTaskDelete(NULL);
}

void onEnterConnectState()
{
  xTaskCreatePinnedToCore(
      connectTask,
      "ConnectTask",
      10000,
      NULL,
      1,
      NULL,
      0);
}

void connectState()
{
  bool isConnected = false;

  BaseType_t res = xQueueReceive(WIFI_STATUS_QUEUE, &isConnected, 0);
  drawWifiConnectionProgress();

  if (res == pdTRUE)
  {
    if (isConnected)
    {
      WifiSettings wifiSettings = {apServer.getSsid(), apServer.getPassword()};
      store.storeWifiSettings(wifiSettings);

      machine.trigger(Triggers::MENU);
    }
  }

  if (!isConnected)
  {
    machine.trigger(Triggers::WIFI_DISCOVER);
    tft.fillScreen(TFT_BLACK);
  }
}

#pragma end region

#pragma redion Menu State

void onEnterMenuState()
{
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  drawMenu();
}

void menuState()
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
    drawMenu();
  }

  if (analogHandler.isPressed())
  {
    switch (selectedOption)
    {
    case 0:
      machine.trigger(Triggers::DOLAR);
      break;
    case 1:
      machine.trigger(Triggers::CLOCK_SCREEN);
      break;
    case 2:
      machine.trigger(Triggers::WEATHER);
      break;
    case 3:
      machine.trigger(Triggers::SCREEN_SAVER);
      break;
    case 4:
      machine.trigger(Triggers::SPOTIFY);
      break;
    }
  }
}

#pragma endregion

#pragma region Dolar State

int DOLLAR_FETCH_INTERVAL = 600000;
int lastDollarFetch = 0;

void onEnterDolarState()
{
  drawDolarPage(true);
  DOLLAR_INFO = dollarService.getCurrent();
  drawDolarPage();
}

void dolarState()
{
  if (millis() - lastDollarFetch > DOLLAR_FETCH_INTERVAL)
  {
    DOLLAR_INFO = dollarService.getCurrent();
    lastDollarFetch = millis();
    drawDolarPage();
  }

  if (analogHandler.isPressed())
  {
    machine.trigger(Triggers::MENU);
  }
}

#pragma endregion

#pragma region Weather State

int WEATHER_FETCH_INTERVAL = 60000;
int lastWeatherFetch = 0;

void onEnterWeatherState()
{
  drawWeatherPage(true);
  WEATHER_INFO = weatherService.getCurrent();
  drawWeatherPage();
}

void weatherState()
{
  if (millis() - lastWeatherFetch > WEATHER_FETCH_INTERVAL)
  {
    WEATHER_INFO = weatherService.getCurrent();
    lastWeatherFetch = millis();
    drawWeatherPage();
  }

  if (analogHandler.isPressed())
  {
    machine.trigger(Triggers::MENU);
  }
}

#pragma endregion

#pragma region Screen Saver State

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

void screenSaverState()
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
    machine.trigger(Triggers::MENU);
  }
}

void onExitScreenSacverState()
{
  tft.fillScreen(TFT_BLACK);
  setupScrollArea(0, 0);
  scrollAddress(0);
}

#pragma endregion

#pragma region Clock State

uint32_t targetTime = 0; // for next 1 second timeout

#define TFT_GREY 0x5AEB

byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

void onEnterClockState()
{
  tft.setTextSize(1);
}

void drawClockFace(int x, int y)
{
  tft.fillCircle(x, y, 110, TFT_DARKGREY); // Fondo del reloj
  tft.fillCircle(x, y, 100, TFT_BLACK);    // Borde interno
  tft.drawCircle(x, y, 110, TFT_WHITE);    // Borde externo
}

// Dibuja la hora y minutos
void drawTime(int x, int y, int hh, int mm)
{
  char timeBuffer[6]; // Formato HH:MM
  int xpos = x - 5;
  sprintf(timeBuffer, "%02d:%02d", hh, mm);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);             // Centra el texto
  tft.drawString(timeBuffer, xpos, y, 7); // Fuente 7 para hora
}

// Dibuja los segundos
void drawSeconds(int x, int y, int ss)
{
  char secondsBuffer[3]; // Formato SS
  sprintf(secondsBuffer, "%02d", ss);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);             // Centra el texto
  tft.drawString(secondsBuffer, x, y, 2); // Fuente 4 para segundos
}

void clockState()
{
  timeClient.update();

  // Obtén la hora actual
  int hh = timeClient.getHours();
  int mm = timeClient.getMinutes();
  int ss = timeClient.getSeconds();

  // Estilo del reloj
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;

  // Actualizar la pantalla si hay cambios
  if (omm != mm)
  {
    omm = mm;
    drawClockFace(centerX, centerY);    // Dibuja el fondo del reloj
    drawTime(centerX, centerY, hh, mm); // Dibuja la hora y minutos
  }

  if (oss != ss)
  {
    oss = ss;
    drawSeconds(centerX, centerY + 60, ss); // Dibuja los segundos
  }

  if (analogHandler.isPressed())
  {
    machine.trigger(Triggers::MENU);
  }
}

void clearScreen()
{
  delay(100);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
}

#pragma endregion

#pragma region Spotify State

void onEnterSpotifyState()
{
  tft.fillScreen(TFT_BLACK);
  drawHeader("Spotify");

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 80);
  tft.println("Spotify - no implementado");
}

void spotifyState()
{
  if (analogHandler.isPressed())
  {
    machine.trigger(Triggers::MENU);
  }
}

//==============================================================================
#pragma region States Transitions
//                        Nombre              OnEnter                 OnState                 OnExit

State INIT_STATE = State(_INIT, initeState);
State WIFI_DISCOVER_STATE = State(_WIFI_DISCOVER, onEnterwifiDiscoveryState, wifiDiscoveryState, onExitWifiDiscoveryState);
State WIFI_CONNECT_STATE = State(_WIFI_CONNECT, onEnterConnectState, connectState, clearScreen);
State MENU_STATE = State(_MENU, onEnterMenuState, menuState, clearScreen);
State WEATHER_STATE = State(_WEATHER, onEnterWeatherState, weatherState, clearScreen);
State DOLAR_STATE = State(_DOLAR, onEnterDolarState, dolarState, clearScreen);
State CLOCK_STATE = State(_CLOCK, NULL, clockState, clearScreen);
State SCREEN_SAVER_STATE = State(_SCREEN_SAVER, NULL, screenSaverState, onExitScreenSacverState);
State SPOTIFY_STATE = State(_SPOTIFY, onEnterSpotifyState, spotifyState, clearScreen);

//==============================================================================

void log(String from, String to)
{
  Serial.println("================================================");
  Serial.println("From: " + from + " to: " + to);
  Serial.println("================================================");
}

Transition transitions[]{
    Transition(&INIT_STATE, &WIFI_DISCOVER_STATE, Triggers::WIFI_DISCOVER, []()
               { log("init", "wifi discover"); }),
    Transition(&INIT_STATE, &WIFI_CONNECT_STATE, Triggers::WIFI_CONNECT, []()
               { log("init", "connect"); }),
    Transition(&WIFI_DISCOVER_STATE, &WIFI_CONNECT_STATE, Triggers::WIFI_CONNECT, []()
               { log("wifi discover", "connect"); }),
    Transition(&WIFI_CONNECT_STATE, &WIFI_DISCOVER_STATE, Triggers::WIFI_DISCOVER, []()
               { log("connect", "wifi discover"); }),
    Transition(&WIFI_CONNECT_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("connect", "menu"); }),
    Transition(&MENU_STATE, &WEATHER_STATE, Triggers::WEATHER, []()
               { log("menu", "weather"); }),
    Transition(&MENU_STATE, &DOLAR_STATE, Triggers::DOLAR, []()
               { log("menu", "dolar"); }),
    Transition(&MENU_STATE, &CLOCK_STATE, Triggers::CLOCK_SCREEN, []()
               { log("menu", "clock"); }),
    Transition(&MENU_STATE, &SCREEN_SAVER_STATE, Triggers::SCREEN_SAVER, []()
               { log("menu", "screen saver"); }),
    Transition(&MENU_STATE, &SPOTIFY_STATE, Triggers::SPOTIFY, []()
               { log("menu", "spotify"); }),
    Transition(&WEATHER_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("weather", "menu"); }),
    Transition(&DOLAR_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("dolar", "menu"); }),
    Transition(&CLOCK_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("clock", "menu"); }),
    Transition(&SCREEN_SAVER_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("screen saver", "menu"); }),
    Transition(&SPOTIFY_STATE, &MENU_STATE, Triggers::MENU, []()
               { log("spotify", "menu"); }),
};

#pragma endregion

void setup()
{
  Serial.begin(115200);

  int num_transitions = sizeof(transitions) / sizeof(Transition);

  machine.add(transitions, num_transitions);
  machine.setInitialState(&INIT_STATE);

  analogHandler.setup();

  tft.init();
  tft.setRotation(1); // Ajustar rotación según tu pantalla
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextSize(2);

  Serial.println(machine.getDotDefinition()); // Imprime la definición del grafo en formato DOT para usar en graphviz
}

void loop()
{
  machine.run();
}