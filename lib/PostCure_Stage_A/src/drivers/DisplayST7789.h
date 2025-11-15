#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "config/Pins.h"
#include "config/Colors.h"

class DisplayST7789 {
public:
  DisplayST7789();                      // есть явный конструктор

  void begin();                         // init дисплея
  void clear();
  void clearBody();

  // Верхний бар: статика + обновление иконок
  void drawTopBarStatic();
  void updateTopBar(bool wifi, bool mqtt, bool heater, bool uv);

  void printSmall(int16_t x, int16_t y, const char* text, uint16_t color = COL_TEXT);
  void fill(uint16_t color);

  Adafruit_ST7789& gfx();

private:
  Adafruit_ST7789 _tft;

  void _drawIcons(bool wifi, bool mqtt, bool heater, bool uv);

  // Кэш предыдущего состояния для избежания лишней перерисовки
  bool _pwifi   = 2;
  bool _pmqtt   = 2;
  bool _pheater = 2;
  bool _puv     = 2;
};
