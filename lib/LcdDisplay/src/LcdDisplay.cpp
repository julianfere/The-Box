#include <Arduino.h>
#include "LcdDisplay.h"

byte Termometer[] = {
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B11111,
};

byte Sun[] = {
    B00100,
    B01110,
    B11111,
    B01110,
    B01110,
    B11111,
    B01110,
    B00100};

LcdDisplay::LcdDisplay() : _display(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS)
{
  this->suspense_count = 0;

  _display.createChar(0, Termometer);
  _display.createChar(1, Sun);
};

void LcdDisplay::clear()
{
  _display.clear();
};

void LcdDisplay::init()
{
  _display.init();
  _display.backlight();
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