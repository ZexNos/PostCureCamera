#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Adafruit_ST7789.h"
#include "config/Pins.h"
#include "config/Colors.h"

class DisplayST7789 {
public:
  DisplayST7789();

  void begin();
  void setInverted(bool inv);     // инверт цветов (true/false)

  void clear();
  void clearBody();

  // Текст/заливка
  void printSmall(int16_t x, int16_t y, const char* text, uint16_t color = COL_TEXT);
  void fill(uint16_t color);

  // Доступ к "сырым" графическим методам при необходимости
  Adafruit_ST7789& gfx();

  // Примитивы иконок для UI-слоёв (TopBar и т.п.)
  void drawWifiIcon(int16_t x, int16_t y, bool on);  // Wi-Fi: off=серые дуги+крест, on=оранжевые палки
  void drawDotIcon (int16_t x, int16_t y, bool on);  // MQTT/Heater/UV

private:
  Adafruit_ST7789 _tft;
};
