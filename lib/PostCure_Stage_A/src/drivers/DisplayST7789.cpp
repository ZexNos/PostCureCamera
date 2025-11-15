#include "drivers/DisplayST7789.h"

DisplayST7789::DisplayST7789()
: _tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST) {}

void DisplayST7789::begin() {
  SPI.begin(PIN_TFT_SCLK, PIN_TFT_MISO, PIN_TFT_MOSI);
  _tft.init(128, 160);
  _tft.setRotation(UI_ROTATION);
  _tft.fillScreen(COL_BG);

  // НИЧЕГО из UI здесь не рисуем — только инициализация.
  // Бар и иконки рисуются из App.
}

void DisplayST7789::clear() {
  _tft.fillScreen(COL_BG);
}

void DisplayST7789::clearBody() {
  _tft.fillRect(0, UI_BAR_H, UI_W, UI_H - UI_BAR_H, COL_BG);
}

void DisplayST7789::drawTopBarStatic() {
  _tft.fillRect(0, 0, UI_W, UI_BAR_H, COL_BAR);
  _tft.drawLine(0, UI_BAR_H - 1, UI_W, UI_BAR_H - 1, COL_LINE);

  _tft.setCursor(4, 4);
  _tft.setTextSize(1);
  _tft.setTextColor(COL_TEXT);
  _tft.print("POSTCURE");
}

void DisplayST7789::updateTopBar(bool wifi, bool mqtt, bool heater, bool uv) {
  if (wifi == _pwifi && mqtt == _pmqtt && heater == _pheater && uv == _puv) return;

  const int icons_w = 4 + 4 * (2 * 6 + 6);  // грубая ширина правой области
  _tft.fillRect(UI_W - icons_w, 0, icons_w, UI_BAR_H, COL_BAR);

  _drawIcons(wifi, mqtt, heater, uv);

  _pwifi = wifi; _pmqtt = mqtt; _pheater = heater; _puv = uv;
}

void DisplayST7789::_drawIcons(bool wifi, bool mqtt, bool heater, bool uv) {
  int16_t x = UI_W - 4;
  auto put = [&](const char* s, uint16_t col) {
    int16_t w = strlen(s) * 6;
    x -= w;
    _tft.setCursor(x, 4);
    _tft.setTextColor(col);
    _tft.print(s);
    x -= 6;
  };
  put(uv ? "UV" : "uv", uv ? COL_ACCENT : COL_TEXT);
  put(heater ? "H" : "h", heater ? COL_ACCENT : COL_TEXT);
  put(mqtt ? "M" : "m", mqtt ? COL_ACCENT : COL_TEXT);
  put(wifi ? "W" : "w", wifi ? COL_ACCENT : COL_TEXT);
}

void DisplayST7789::printSmall(int16_t x, int16_t y, const char* text, uint16_t color) {
  _tft.setTextSize(1);
  _tft.setTextColor(color);
  _tft.setCursor(x, y);
  _tft.print(text);
}

void DisplayST7789::fill(uint16_t color) {
  _tft.fillScreen(color);
}

Adafruit_ST7789& DisplayST7789::gfx() {
  return _tft;
}
