#include "app/App.h"
#include "config/Colors.h"

void App::begin() {
  tft.begin();
  enc.begin();
  ui.begin(&tft);
  screenRun.begin(&tft);

  // 1) Верхний бар: сначала фон и надпись
  tft.drawTopBarStatic();
  // 2) Затем иконки (вернут «символы» справа)
  tft.updateTopBar(false, false, false, false);

  // 3) Тело экрана
  screenRun.drawStatic();
  screenRun.updateValue(cursor);
}

void App::tick() {
  enc.tick();
  bool valueChanged = false;

  if (enc.left())  { cursor--; valueChanged = true; }
  if (enc.right()) { cursor++; valueChanged = true; }
  if (enc.click()) { cursor = 0; valueChanged = true; }

  if (cursor < -99) cursor = -99;
  if (cursor >  99) cursor =  99;

  if (valueChanged) {
    screenRun.updateValue(cursor);
  }

  // Пока статусы фиктивные — обновление бар-а будет noop из-за кэша
  tft.updateTopBar(false, false, false, false);
}
