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
DollarService dollarService = DollarService();

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

void drawMenu()
{
  int verticalSpacing = tft.fontHeight() + 5;
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < numOptions; i++)
  {
    if (i == selectedOption)
    {
      tft.setTextColor(TFT_BLACK, TFT_WHITE); // Texto invertido
    }
    else
    {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.setCursor(10, i * verticalSpacing + 10);
    tft.println(menuOptions[i]);
  }
}

void drawInitPage()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Inicializando...");
}

void drawWifiDiscoveryPage()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Buscando redes disponibles");
}

int spinnerRadius = 40; // Radio del spinner
int arcThickness = 10;  // Grosor del arco
int spinnerSpeed = 10;  // Velocidad de giro (en milisegundos)
int spinnerAngle = 0;   // Ángulo inicial del spinner

void drawSpinner()
{
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;

  // Borra el fondo del spinner (opcional)
  tft.fillCircle(centerX, centerY, spinnerRadius + arcThickness, TFT_BLACK);

  // Dibuja un arco para el spinner
  for (int i = 0; i < 360; i += 45)
  { // Dibuja segmentos cada 45 grados
    int startAngle = spinnerAngle + i;
    int endAngle = startAngle + 30; // Longitud del arco

    uint16_t color = (i == 0) ? TFT_WHITE : TFT_DARKGREY; // Segmento destacado

    tft.drawArc(centerX, centerY, spinnerRadius, arcThickness, startAngle, endAngle, TFT_WHITE, TFT_DARKGREY, true);
  }
}

void drawWifiConnectionProgress()
{
  drawSpinner();
  spinnerAngle += 15;

  if (spinnerAngle >= 360)
  {
    spinnerAngle = 0;
  }
}

void drawDolarPage()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Dolar Blue: " + String(DOLLAR_INFO.blue));
  tft.setCursor(10, 40);
  tft.println("Dolar Oficial: " + String(DOLLAR_INFO.oficial));
}

void drawWeatherPage()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Clima: " + WEATHER_INFO.status);
  tft.setCursor(10, 40);
  tft.println("Temperatura: " + String(WEATHER_INFO.temp));
  tft.setCursor(10, 70);
  tft.println("Humedad: " + String(WEATHER_INFO.hum));
  tft.setCursor(10, 100);
  tft.println("Lluvia: " + String(WEATHER_INFO.rain));
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
  Serial.println("Conectado: " + String(isConnected));

  if (res == pdTRUE)
  {
    if (isConnected)
    {
      WifiSettings wifiSettings = {apServer.getSsid(), apServer.getPassword()};
      store.storeWifiSettings(wifiSettings);

      machine.trigger(Triggers::MENU);
    }
  }

  drawWifiConnectionProgress();

  if (!isConnected)
  {
    machine.trigger(Triggers::WIFI_DISCOVER);
  }
}

#pragma end region

#pragma redion Menu State

void onEnterMenuState()
{
  delay(100);
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

exit:
  machine.trigger(Triggers::MENU);
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

static uint8_t conv2d(const char *p); // Forward declaration needed for IDE 1.6.x

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

#define TFT_GREY 0x5AEB

byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char *p)
{
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

void onEnterClockState()
{
  tft.setTextSize(1);
}

void clockState()
{
  if (analogHandler.isPressed())
  {
    machine.trigger(Triggers::MENU);
  }

  if (targetTime < millis())
  {
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    // Adjust the time values by adding 1 second
    ss++; // Advance second
    if (ss == 60)
    {           // Check for roll-over
      ss = 0;   // Reset seconds to zero
      omm = mm; // Save last minute time for display update
      mm++;     // Advance minute
      if (mm > 59)
      { // Check for roll-over
        mm = 0;
        hh++; // Advance hour
        if (hh > 23)
        {         // Check for 24hr roll-over (could roll-over on 13)
          hh = 0; // 0 for 24 hour clock, set to 1 for 12 hour clock
        }
      }
    }

    // Update digital time
    int xpos = 85;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm)
    { // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10)
        xpos += tft.drawChar('0', xpos, ypos, 4); // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, 4);  // Draw hours
      xcolon = xpos;                              // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos - 6, 4);
      if (mm < 10)
        xpos += tft.drawChar('0', xpos, ypos, 4); // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, 4);  // Draw minutes
      xsecs = xpos;                               // Sae seconds 'x' position for later display updates
    }
    if (oss != ss)
    { // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2)
      {                                             // Flash the colons on/off
        tft.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
        tft.drawChar(':', xcolon, ypos - 6, 4);     // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs, 2); // Seconds colon
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
      }
      else
      {
        tft.drawChar(':', xcolon, ypos - 8, 4);         // Hour:minute colon
        xpos += tft.drawChar(':', xsecs, ysecs - 2, 2); // Seconds colon
      }

      // Draw seconds
      if (ss < 10)
        xpos += tft.drawChar('0', xpos, ysecs, 2); // Add leading zero
      tft.drawNumber(ss, xpos, ysecs, 1);          // Draw seconds
    }
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
  tft.fillScreen(TFT_DARKGREEN);

  tft.setTextColor(TFT_WHITE);

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