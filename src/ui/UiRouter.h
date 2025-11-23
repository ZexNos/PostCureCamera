#pragma once
#include <Arduino.h>
class DisplayST7789;

class UiRouter {
public:
  void begin(DisplayST7789* disp) { _disp = disp; }
private:
  DisplayST7789* _disp = nullptr;
};
