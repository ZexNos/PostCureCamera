#pragma once
#include <Arduino.h>
#include <EncButton.h>
#include "config/Pins.h"

class EncoderDrv {
public:
  void begin();
  void tick();

  bool left();
  bool right();
  bool click();
  bool hold();

private:
  EncButton _enc = EncButton(PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_BTN);
  bool _evLeft  = false;
  bool _evRight = false;
  bool _evClick = false;
  bool _evHold  = false;
};
