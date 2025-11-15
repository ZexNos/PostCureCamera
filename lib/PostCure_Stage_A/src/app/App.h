#pragma once
#include <Arduino.h>
#include "drivers/DisplayST7789.h"
#include "drivers/Encoder.h"
#include "ui/UiRouter.h"
#include "ui/screens/RunView.h"

class App {
public:
  void begin();
  void tick();

private:
  DisplayST7789 tft;
  EncoderDrv    enc;
  UiRouter      ui;
  RunView       screenRun;

  int       cursor     = 0;
  uint32_t  lastRedraw = 0;   // для периодического обновления статус-бара
};
