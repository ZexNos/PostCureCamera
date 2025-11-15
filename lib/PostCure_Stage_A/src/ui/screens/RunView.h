#pragma once
#include <Arduino.h>

class DisplayST7789;

class RunView {
public:
  void begin(DisplayST7789* disp);

  // Новый «без мерцаний» интерфейс
  void drawStatic();          // рисует неизменяемые элементы один раз
  void updateValue(int val);  // перерисовывает только строку со значением

  // Совместимость со старым кодом (если где-то ещё зовётся draw(cursor))
  void draw(int cursor);      // внутренно вызовет drawStatic() один раз и updateValue()

private:
  DisplayST7789* _disp = nullptr;
  int  _prev = INT16_MIN;
  bool _staticDrawn = false;

  // координаты «ячейки» значения
  static constexpr int VAL_X = 8;
  static constexpr int VAL_Y = 64;   // реальная Y = UI_BAR_H + VAL_Y
};
