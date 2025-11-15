#include "ui/screens/RunView.h"
#include "drivers/DisplayST7789.h"
#include "config/Colors.h"

void RunView::begin(DisplayST7789* disp) {
  _disp = disp;
}

void RunView::drawStatic() {
  auto& t = _disp->gfx();

  // фон рабочей области (один раз)
  _disp->clearBody();

  // заголовок экрана
  t.setTextSize(2);
  t.setTextColor(COL_ACCENT);
  t.setCursor(8, UI_BAR_H + 8);
  t.print("RUN VIEW");

  // подсказки
  _disp->printSmall(8, UI_BAR_H + 36, "Rotate: change", COL_TEXT);
  _disp->printSmall(8, UI_BAR_H + 48, "Click: reset",   COL_TEXT);

  // подготовим «ячейку» под значение
  t.fillRect(VAL_X, UI_BAR_H + VAL_Y, 90, 12, COL_BG);

  _staticDrawn = true;
}

void RunView::updateValue(int val) {
  if (!_staticDrawn) drawStatic();
  if (val == _prev) return;

  auto& t = _disp->gfx();
  char buf[24];
  snprintf(buf, sizeof(buf), "Value: %d", val);

  // Перерисовываем только строку значения
  t.setTextSize(1);
  t.setTextColor(COL_TEXT, COL_BG);      // печать с фоном
  t.setCursor(VAL_X, UI_BAR_H + VAL_Y);
  t.print(buf);
  t.print("   "); // на случай, если новая строка короче старой

  _prev = val;
}

// Совместимость: старый метод просто делегирует к новому интерфейсу
void RunView::draw(int cursor) {
  if (!_staticDrawn) drawStatic();
  updateValue(cursor);
}
