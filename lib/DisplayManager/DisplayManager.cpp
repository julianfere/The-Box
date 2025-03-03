#include <DisplayManager.h>
#include <vector>
#include <string>

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

void DisplayManager::printSplitString(String text, int maxLineSize, int yPos)
{
  std::vector<std::string> parts;
  int currentWordStart = 0;
  int spaceIndex = text.indexOf(" ");

  while (spaceIndex != -1)
  {
    parts.push_back(text.substring(currentWordStart, spaceIndex).c_str());
    currentWordStart = spaceIndex + 1;
    spaceIndex = text.indexOf(" ", currentWordStart);
  }

  // Última parte del texto
  parts.push_back(text.substring(currentWordStart).c_str());

  // Imprimir líneas con el máximo de caracteres permitidos
  String output = "";
  int currentLen = 0;

  for (const auto &word : parts)
  {
    if (currentLen + word.length() > maxLineSize)
    {
      // Imprimir la línea actual centrada
      tft.setCursor((tft.width() - tft.textWidth(output)) / 2, yPos);
      tft.println(output);
      yPos += 20; // Espacio entre líneas
      output = word.c_str();
      currentLen = word.length();
    }
    else
    {
      output += " " + String(word.c_str());
      currentLen += word.length() + 1;
    }
  }

  // Imprimir última línea si hay contenido
  if (output.length() > 0)
  {
    tft.setCursor((tft.width() - tft.textWidth(output)) / 2, yPos);
    tft.println(output);
  }
}

void DisplayManager::drawSongDetails(SongDetails *currentSong, bool fullRefresh, bool likeRefresh)
{
  this->drawed = false;
  tft.fillScreen(TFT_BLACK); // Limpiar la pantalla
  yield();

  // Tamaño máximo de la imagen (ajustar según preferencia)
  int maxImgWidth = tft.width() / 2;   // Ancho máximo de la imagen (50% de la pantalla)
  int maxImgHeight = tft.height() / 2; // Alto máximo de la imagen (50% de la pantalla)

  // Posición de la imagen (centrada)
  int imgX = (tft.width() - maxImgWidth) / 2; // Centrar en el eje X
  int imgY = 20;                              // Margen superior

  // Verificar si la imagen existe
  if (SPIFFS.exists("/albumArt.jpg"))
  {
    // Obtener el tamaño original de la imagen
    File imgFile = SPIFFS.open("/albumArt.jpg", "r");
    uint16_t imgOrigWidth;
    uint16_t imgOrigHeight;
    if (imgFile)
    {
      TJpgDec.getFsJpgSize(&imgOrigWidth, &imgOrigHeight, imgFile);
      imgFile.close();

      // Calcular el aspect ratio
      float aspectRatio = (float)imgOrigWidth / (float)imgOrigHeight;

      // Calcular el tamaño de la imagen manteniendo el aspect ratio
      int imgWidth = maxImgWidth;
      int imgHeight = imgWidth / aspectRatio;

      // Si la altura calculada excede el máximo permitido, ajustar
      if (imgHeight > maxImgHeight)
      {
        imgHeight = maxImgHeight;
        imgWidth = imgHeight * aspectRatio;
      }

      // Ajustar la posición para mantener la imagen centrada
      imgX = (tft.width() - imgWidth) / 2;
      imgY = 20;              // Margen superior
      TJpgDec.setJpgScale(1); // Escala de imagen 1:1
      TJpgDec.setSwapBytes(true);
      // TJpgDec.setJpgScale(4);
      TJpgDec.drawFsJpg(imgX, 0, "/Angry.jpg");
    }
  }
  else
  {
    // Dibujar una imagen por defecto si no se encuentra la original
    TJpgDec.drawFsJpg(imgX, 0, "/Angry.jpg");
  }

  // Mostrar el título y la banda en formato [Título] - [Artista]
  tft.setTextDatum(TC_DATUM); // Centrar texto horizontalmente
  tft.setTextWrap(true);      // Habilitar text wrapping

  // Título de la canción
  int textY = imgY + maxImgHeight + 20; // Posición debajo de la imagen
  tft.setCursor(10, textY);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);           // Fuente más pequeña
  tft.print(currentSong->song); // Título

  // Artista
  textY += 15; // Espacio entre el título y el artista
  tft.setCursor(10, textY);
  tft.setTextColor(0xBDF7, TFT_BLACK); // Gris claro
  tft.setTextSize(1);                  // Fuente más pequeña
  tft.print(currentSong->artist);      // Artista

  // ❤️ Like (mostrar corazón si está "me gusta")
  if (fullRefresh || likeRefresh)
  {
    if (currentSong->isLiked)
    {
      TJpgDec.setJpgScale(1);
      TJpgDec.drawFsJpg(tft.width() - 30, imgY + maxImgHeight + 20, "/heart.jpg"); // Corazón al lado del texto
    }
    else
    {
      tft.fillRect(tft.width() - 30, imgY + maxImgHeight + 20, 21, 21, TFT_BLACK); // Borrar el corazón si no está marcado
    }
  }

  // ⏸️ ▶️ Estado de reproducción
  int iconSize = 20;                        // Tamaño del ícono de play/pausa
  int iconX = (tft.width() - iconSize) / 2; // Centrar el ícono
  int iconY = textY + 25;                   // Posición debajo del texto

  if (currentSong->isPlaying)
  {
    // Icono de pausa (⏸️)
    tft.fillRect(iconX, iconY, 6, iconSize, TFT_WHITE);      // Barra izquierda
    tft.fillRect(iconX + 10, iconY, 6, iconSize, TFT_WHITE); // Barra derecha
  }
  else
  {
    // Icono de play (▶️)
    tft.fillTriangle(iconX, iconY, iconX, iconY + iconSize, iconX + iconSize, iconY + (iconSize / 2), TFT_WHITE);
  }

  // 🔄 Dibujar barra de progreso
  int barWidth = tft.width() - 40; // Ancho total de la barra de progreso
  int barHeight = 4;               // Altura de la barra de progreso (más delgada)
  int barX = 20;                   // Margen izquierdo
  int barY = tft.height() - 20;    // Posición en la parte inferior

  tft.fillRoundRect(barX, barY, barWidth, barHeight, 2, 0x7BEF); // Gris claro de fondo
  int progressWidth = (barWidth * currentSong->progressMs) / currentSong->durationMs;
  tft.fillRoundRect(barX, barY, progressWidth, barHeight, 2, TFT_GREEN); // Barra de progreso verde

  // Mostrar tiempo transcurrido y tiempo total
  tft.setTextDatum(BL_DATUM); // Alinear texto a la izquierda
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(barX, barY - 15);
  tft.print(formatTime(currentSong->progressMs)); // Tiempo transcurrido

  tft.setTextDatum(BR_DATUM); // Alinear texto a la derecha
  tft.setCursor(barX + barWidth - 15, barY - 10);
  tft.print(formatTime(currentSong->durationMs)); // Tiempo total
}

// Función para formatear el tiempo en minutos:segundos
String DisplayManager::formatTime(int ms)
{
  int totalSeconds = ms / 1000;
  int minutes = totalSeconds / 60;
  int seconds = totalSeconds % 60;
  return String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
}

void DisplayManager::drawNoSongPlaying()
{
  if (this->drawed)
  {
    return;
  }

  tft.fillScreen(TFT_BLACK); // Limpiar la pantalla
  yield();

  int imgX = (tft.width() - 100) / 2;  // Centrar la imagen
  int imgY = (tft.height() - 100) / 2; // Centrar la imagen
  TJpgDec.setJpgScale(1);
  TJpgDec.drawFsJpg(imgX, imgY, "/Angry.jpg");
  this->drawed = true;
}
