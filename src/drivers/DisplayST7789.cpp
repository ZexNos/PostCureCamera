#include "drivers/DisplayST7789.h"
#include "config/Colors.h"
#include "config/Pins.h"

DisplayST7789::DisplayST7789()
: _tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST) {}

void DisplayST7789::begin() {
  #ifdef PIN_TFT_MISO
    SPI.begin(PIN_TFT_SCLK, PIN_TFT_MISO, PIN_TFT_MOSI);
  #else
    SPI.begin(PIN_TFT_SCLK, -1, PIN_TFT_MOSI);
  #endif
  _tft.init(128, 160);
  _tft.setRotation(UI_ROTATION);
  _tft.fillScreen(COL_BG);
}

void DisplayST7789::setInverted(bool inv) { _tft.invertDisplay(inv); }

void DisplayST7789::clear()     { _tft.fillScreen(COL_BG); }
void DisplayST7789::clearBody() { _tft.fillRect(0, UI_BAR_H, UI_W, UI_H - UI_BAR_H, COL_BG); }

// ── Иконки ────────────────────────────────────────────────────────────────

void DisplayST7789::drawWifiIcon(int16_t x, int16_t y, bool on) {
  uint16_t fgOff = COL_LINE;
  uint16_t fgOn  = COL_ACCENT;
  uint16_t bg    = COL_BAR;

  _tft.fillRect(x-8, y-1, 12, 12, bg);

  if (!on) {
    // тонкие дуги + крестик (off)
    _tft.drawFastHLine(x-7, y+6, 8, fgOff);
    _tft.drawFastHLine(x-6, y+4, 6, fgOff);
    _tft.drawFastHLine(x-5, y+2, 4, fgOff);
    _tft.drawRect (x-1, y+7, 2, 2, fgOff);
    _tft.drawLine(x-8, y-1, x+3, y+10, fgOff);
    _tft.drawLine(x+3, y-1, x-8, y+10, fgOff);
  } else {
    // залитые «палочки» (on) — контрастно
    _tft.fillRect(x-7, y+5, 2, 3, fgOn);
    _tft.fillRect(x-4, y+3, 2, 5, fgOn);
    _tft.fillRect(x-1, y+1, 2, 7, fgOn);
    _tft.fillRect(x+2, y-1, 2, 9, fgOn);
    _tft.fillRect(x-1, y+7, 2, 2, fgOn);
  }
}

void DisplayST7789::drawDotIcon(int16_t x, int16_t y, bool on) {
  uint16_t c  = on ? COL_ACCENT : COL_LINE;
  uint16_t bg = COL_BAR;
  _tft.fillRect(x-2, y-2, 6, 6, bg);
  if (on) _tft.fillCircle(x, y, 2, c);
  else    _tft.drawCircle(x, y, 2, c);
}

// ── Текст / утилиты ──────────────────────────────────────────────────────

void DisplayST7789::printSmall(int16_t x, int16_t y, const char* text, uint16_t color) {
  _tft.setTextSize(1);
  _tft.setTextColor(color);
  _tft.setCursor(x, y);
  _tft.print(text);
}

void DisplayST7789::fill(uint16_t color) { _tft.fillScreen(color); }
Adafruit_ST7789& DisplayST7789::gfx()    { return _tft; }
