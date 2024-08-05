#ifndef LcdDisplay_h
#define LcdDisplay_h
#include <LiquidCrystal_I2C.h>
#include <WString.h>

#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_I2C_ADDR 0x27

class LcdDisplay
{
public:
  LcdDisplay();
  void init();
  void print(String line);
  void clear();
  void printWithSuspense(String line);
  void printMultiline(String firstLine, String secondLine);

private:
  LiquidCrystal_I2C _display;
  int suspense_count;
};
#endif