#include <DisplayManager.h>

DisplayManager *DisplayManager::instance = nullptr;

DisplayManager &DisplayManager::getInstance()
{
  if (instance == nullptr)
  {
    instance = new DisplayManager();
  }
  return *instance;
}

void DisplayManager::drawHeader(const char *title)
{
  tft.setTextColor(COLOR_BLUE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.print(title);

  tft.drawLine(0, 40, 250, 40, COLOR_BLUE);
  tft.drawLine(210, 0, 210, 40, COLOR_BLUE);
  tft.drawLine(250, 40, 320, 40, COLOR_BLUE);

  tft.setCursor(220, 0);
  tft.setTextColor(tft.color565(255, 102, 67));
  tft.print("TheBox");
  tft.setTextColor(COLOR_WHITE);
}

DisplayManager::DisplayManager() : tft(TFT_eSPI()) {};

void DisplayManager::init()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextSize(2);
  tft.setSwapBytes(true); // Habilita el intercambio de bytes en comunicación SPI
}
void DisplayManager::drawInitPage()
{
  tft.fillScreen(TFT_BLACK);
  yield();
  drawHeader("");

  // Título centrado
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2); // Fuente pequeña y moderna
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Inicializando...", tft.width() / 2, tft.height() / 2);
};

void DisplayManager::drawWifiDiscoveryPage()
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

void DisplayManager::drawWifiConnectionProgress()
{
  // Dibujar el encabezado solo si no se ha dibujado antes
  static bool headerDrawn = false;
  if (!headerDrawn)
  {
    drawHeader("WiFi");
    headerDrawn = true;
  }

  // Dibujar el título solo si no se ha dibujado antes
  static bool titleDrawn = false;
  if (!titleDrawn)
  {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Conectando WiFi", tft.width() / 2, 30);
    titleDrawn = true;
  }

  // Actualizar solo el texto de progreso
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(1);
  tft.setCursor(30, tft.height() / 2);
  tft.println("Esperando...");
}
void DisplayManager::drawDolarPage(DollarInfo data, bool loading)
{
  tft.fillScreen(TFT_BLACK);
  yield();
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
  tft.println("Blue: " + String(data.blue));

  tft.setCursor(20, 110);
  tft.println("Oficial: " + String(data.oficial));
}

void DisplayManager::drawWeatherPage(WeatherInfo data, bool loading)
{
  tft.fillScreen(TFT_BLACK);
  yield();
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
  tft.println("Cielo: " + data.status);

  tft.setCursor(20, 110);
  tft.println("Temp: " + String(data.temp) + "°C");

  tft.setCursor(20, 140);
  tft.println("Humedad: " + String(data.hum) + "%");

  tft.setCursor(20, 170);
  tft.println("Lluvia: " + String(data.rain) + "%");
}

void DisplayManager::drawMenu(int numOptions, int selectedOption, const char *menuOptions[])
{
  int verticalSpacing = tft.fontHeight() + 4; // Vertical spacing between options, increased for readability
  tft.fillScreen(COLOR_BLACK);                // Clear screen with black background
  yield();

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

void DisplayManager::drawClockFace(int x, int y)
{
  tft.fillCircle(x, y, 110, TFT_DARKGREY); // Fondo del reloj
  tft.fillCircle(x, y, 100, TFT_BLACK);    // Borde interno
  tft.drawCircle(x, y, 110, TFT_WHITE);    // Borde externo
}

// Dibuja la hora y minutos
void DisplayManager::drawTime(int x, int y, int hh, int mm)
{
  char timeBuffer[6]; // Formato HH:MM
  int xpos = x - 5;
  sprintf(timeBuffer, "%02d:%02d", hh, mm);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);             // Centra el texto
  tft.drawString(timeBuffer, xpos, y, 7); // Fuente 7 para hora
}

// Dibuja los segundos
void DisplayManager::drawSeconds(int x, int y, int ss)
{
  char secondsBuffer[3]; // Formato SS
  sprintf(secondsBuffer, "%02d", ss);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);             // Centra el texto
  tft.drawString(secondsBuffer, x, y, 2); // Fuente 4 para segundos
}

void DisplayManager::drawClockPage(NTPClient &timeClient)
{
  static unsigned long lastUpdate = 0;
  static byte omm = 99, oss = 99; // Valores iniciales inválidos para forzar la primera actualización

  unsigned long currentMillis = millis();

  // Solo actualiza cada segundo
  if (currentMillis - lastUpdate >= 1000)
  {
    lastUpdate = currentMillis;
    timeClient.update(); // Sincroniza la hora NTP

    int hh = timeClient.getHours();
    int mm = timeClient.getMinutes();
    int ss = timeClient.getSeconds();

    int centerX = tft.width() / 2;
    int centerY = tft.height() / 2;

    // Solo redibuja si hay cambios
    if (omm != mm)
    {
      omm = mm;
      drawClockFace(centerX, centerY);    // Fondo del reloj
      drawTime(centerX, centerY, hh, mm); // Actualiza la hora y minutos
    }

    if (oss != ss)
    {
      oss = ss;
      drawSeconds(centerX, centerY + 60, ss); // Solo redibuja los segundos
    }
  }
}
