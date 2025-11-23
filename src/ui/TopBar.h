#pragma once
#include <Arduino.h>
#include "drivers/DisplayST7789.h"

class UiTopBar {
public:
    void begin(DisplayST7789* tft);

    // title — название текущего экрана ("RUN", "OPTIONS", ...)
    void drawStatic(const char* title);
    void update(bool wifi, bool mqtt, bool heater, bool uv);

private:
    DisplayST7789* _tft = nullptr;

    bool _wifi  = false;
    bool _mqtt  = false;
    bool _heat  = false;
    bool _uv    = false;

    const char* _title = nullptr;

    void drawIcons(bool wifi, bool mqtt, bool heater, bool uv);
    void drawTitle(const char* title);
};
