#include <Arduino.h>
#include "LcdDisplay.h"

byte Termometer[8] = {
    B01110,
    B01010,
    B01010,
    B01010,
    B01010,
    B01010,
    B10001,
    B11111};

byte Sun[8] = {
    B00100,
    B01110,
    B11111,
    B01110,
    B01110,
    B11111,
    B01110,
    B00100};

byte RainCloud[8] = {
    B00000,
    B00100,
    B01110,
    B11111,
    B00010,
    B01000,
    B00000,
    B00100,
};

byte Cloud[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B01110,
    B11111,
    B01110};

byte Mist[8] = {
    B01101,
    B10010,
    B00110,
    B11001,
    B00010,
    B01100,
    B10011,
    B01000};

byte Thunder[8] = {
    B00000,
    B00000,
    B01110,
    B11111,
    B01110,
    B00010,
    B00100,
    B01000};

byte Humidity[8] = {
    B00100,
    B01010,
    B10001,
    B10001,
    B10001,
    B10001,
    B01110,
    B00000};

byte BlueDollar[8] = {
    B00000,
    B11110,
    B10101,
    B10101,
    B11110,
    B10101,
    B10101,
    B11110};

enum WeatherIcons
{
  TERMOMETER = 0,
  SUN = 1,
  RAIN_CLOUD = 2,
  CLOUD = 3,
  MIST = 4,
  THUNDER = 5,
  HUMIDITY = 6,
};

enum DollarIcons
{
  BLUE_DOLLAR = 7,
};

LcdDisplay::LcdDisplay() : _display(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS)
{
  this->suspense_count = 0;
};

void LcdDisplay::clear()
{
  _display.clear();
};

void LcdDisplay::init()
{
  _display.init();
  _display.backlight();
  _display.createChar(0, Termometer);
  _display.createChar(1, Sun);
  _display.createChar(2, RainCloud);
  _display.createChar(3, Cloud);
  _display.createChar(4, Mist);
  _display.createChar(5, Thunder);
  _display.createChar(6, Humidity);
  _display.createChar(7, BlueDollar);
}

void LcdDisplay::printWithSuspense(String line)
{
  _display.clear();
  String new_line = line.c_str();

  for (int i = 0; i < this->suspense_count; i++)
  {
    new_line.concat(".");
  }

  _display.print(new_line);
  _display.print(".");
  this->suspense_count++;

  if (this->suspense_count >= 3)
  {
    this->suspense_count = 0;
  }
}

void LcdDisplay::print(String line)
{
  _display.clear();
  _display.setCursor(0, 0);
  _display.print(line);
}

void LcdDisplay::printMultiline(String firstLine, String secondLine)
{
  _display.clear();
  _display.print(firstLine);
  _display.setCursor(0, 1);
  _display.print(secondLine);
}

void LcdDisplay::printweather(WeatherInfo info)
{
  _display.clear();
  _display.setCursor(0, 0);
  _display.write((byte)WeatherIcons::TERMOMETER);
  _display.print(String(info.temp) + "c");
  _display.print("-");
  _display.write((byte)WeatherIcons::HUMIDITY);
  _display.print(String(info.hum) + "%");
  _display.setCursor(0, 1);
  _display.write((byte)WeatherIcons::RAIN_CLOUD);
  _display.print(" " + String(info.rain) + "% ");
  if (info.status == "Limpio")
  {
    _display.write((byte)WeatherIcons::SUN);
  }
  else if (info.status == "Niebla")
  {
    _display.write((byte)WeatherIcons::MIST);
  }
  else if (info.status == "Llovizna")
  {
    _display.write((byte)WeatherIcons::RAIN_CLOUD);
  }
  else if (info.status == "Lluvia")
  {
    _display.write((byte)WeatherIcons::RAIN_CLOUD);
  }
  else if (info.status == "Nieve")
  {
    _display.write((byte)WeatherIcons::CLOUD);
  }
  else if (info.status == "Tormenta")
  {
    _display.write((byte)WeatherIcons::THUNDER);
  }
  _display.print(info.status);
}

void LcdDisplay::printDollar(DollarInfo info)
{
  _display.clear();
  _display.setCursor(0, 0);
  _display.print("$ " + String(info.oficial));
  _display.setCursor(0, 1);
  _display.write((byte)DollarIcons::BLUE_DOLLAR);
  _display.print(" " + String(info.blue));
}