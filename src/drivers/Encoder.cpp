#include "drivers/Encoder.h"

void EncoderDrv::begin() {
  pinMode(PIN_ENCODER_BTN, INPUT_PULLUP);

  // Тип энкодера: стабильный вариант для большинства модулей
  _enc.setEncType(EB_STEP4_LOW);

  // Тайминги из библиотеки (не мои фильтры)
  _enc.setHoldTimeout(600);
  _enc.setClickTimeout(350);
  _enc.setBtnLevel(LOW);          // кнопка с PULLUP
}

void EncoderDrv::tick() {
  _evLeft = _evRight = _evClick = _evHold = false;

  _enc.tick();

  if (_enc.right()) _evRight = true;
  if (_enc.left())  _evLeft  = true;
  if (_enc.click()) _evClick = true;
  if (_enc.hold())  _evHold  = true;
}

bool EncoderDrv::left()  { bool f=_evLeft;  _evLeft=false;  return f; }
bool EncoderDrv::right() { bool f=_evRight; _evRight=false; return f; }
bool EncoderDrv::click() { bool f=_evClick; _evClick=false; return f; }
bool EncoderDrv::hold()  { bool f=_evHold;  _evHold=false;  return f; }
