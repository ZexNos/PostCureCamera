#pragma once
#include <Arduino.h>
#include "drivers/DisplayST7789.h"
#include "config/Colors.h"

class RunView {
public:
    void begin(DisplayST7789* tft);

    // Статическая отрисовка RUN VIEW (фон, кнопки, подписи)
    void drawStatic();

    // 0 = CURE, 1 = DRY, 2 = SETTINGS
    void setSelected(uint8_t index);
    uint8_t selected() const { return _selected; }

private:
    DisplayST7789* _tft = nullptr;
    uint8_t _selected = 0;

    void drawButtons();
    void drawButton(uint8_t index, bool active);

    void drawIconBitmap42(int16_t x, int16_t y,
                          const uint8_t* data,
                          uint16_t color);

    void drawLabels();
};
