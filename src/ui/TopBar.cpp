#include "ui/TopBar.h"
#include "config/Colors.h"

// -----------------------------------------------------------------------------
//  Константы макета (только свои, без UI_W/UI_BAR_H)
// -----------------------------------------------------------------------------

namespace
{
    constexpr int ICON_W    = 12;
    constexpr int ICON_H    = 12;

    // Позиции WiFi / MQTT слева
    constexpr int WIFI_X    = 4;
    constexpr int WIFI_Y    = (UI_BAR_H - ICON_H) / 2;

    constexpr int MQTT_X    = WIFI_X + ICON_W + 4;
    constexpr int MQTT_Y    = WIFI_Y;

    // Индикаторы HEAT / UV справа (маленькие квадраты)
    constexpr int STATE_SIZE = 6;
    constexpr int STATE_Y    = (UI_BAR_H - STATE_SIZE) / 2;
    constexpr int UV_X       = UI_W - 4 - STATE_SIZE;
    constexpr int HEAT_X     = UV_X - 4 - STATE_SIZE;

    // Цвета (через уже существующие из Colors.h)
    constexpr uint16_t WIFI_COLOR_OFF  = COL_LINE;  // серый
    constexpr uint16_t WIFI_COLOR_ON   = COL_WIFI;  // как заголовок

    constexpr uint16_t MQTT_COLOR_OFF  = COL_LINE;  // серый

    #ifdef COL_MQTT
    constexpr uint16_t MQTT_COLOR_ON   = COL_MQTT;  // фиолетовый, если задан
    #else
    constexpr uint16_t MQTT_COLOR_ON   = COL_MQTT;
    #endif

    #ifdef COL_HEAT
    constexpr uint16_t HEAT_COLOR_ON   = COL_HEAT;
    #else
    constexpr uint16_t HEAT_COLOR_ON   = COL_TEXT;
    #endif

    constexpr uint16_t HEAT_COLOR_OFF  = COL_LINE;

    #ifdef COL_UV
    constexpr uint16_t UV_COLOR_ON     = COL_UV;
    #else
    constexpr uint16_t UV_COLOR_ON     = COL_TEXT;
    #endif

    constexpr uint16_t UV_COLOR_OFF    = COL_LINE;
}

// -----------------------------------------------------------------------------
//  Bitmaps 12x12 (WiFi / MQTT)
// -----------------------------------------------------------------------------

// WiFi 12x12
static const uint8_t PROGMEM ICON_WIFI_12[] = {
    // '35ff1a35-dccc-4e02-80f4-2bb8806c24ee', 12x12px
	0x00, 0x00, 0x1e, 0x00, 0x07, 0x00, 0x01, 0x80, 0x70, 0xc0, 0x1c, 0x60, 0x06, 0x60, 0x43, 0x20, 0xf3, 0x30, 0xf1, 0x10, 0xf9, 0x90, 0x78, 0x80
};

// MQTT 12x12
static const uint8_t PROGMEM ICON_MQTT_12[] = {
    // '35ff1a35-dccc-4e02-80f4-2bb8806c24ee', 12x12px
	0x00, 0x00, 0x1e, 0x00, 0x07, 0x00, 0x01, 0x80, 0x70, 0xc0, 0x1c, 0x60, 0x06, 0x60, 0x43, 0x20, 0xf3, 0x30, 0xf1, 0x10, 0xf9, 0x90, 0x78, 0x80
};

// -----------------------------------------------------------------------------
//  UiTopBar
// -----------------------------------------------------------------------------

void UiTopBar::begin(DisplayST7789* tft)
{
    _tft   = tft;
    _wifi  = false;
    _mqtt  = false;
    _heat  = false;
    _uv    = false;
}

void UiTopBar::drawStatic(const char* title)
{
    if (!_tft) return;

    auto& g = _tft->gfx();

    // фон бара
    g.fillRect(0, 0, UI_W, UI_BAR_H, COL_BAR);
    // нижняя линия
    g.drawLine(0, UI_BAR_H - 1, UI_W, UI_BAR_H - 1, COL_LINE);

    // заголовок по центру
    if (title) {
        g.setTextSize(1);
        g.setTextColor(COL_TEXT);

        uint8_t len   = strlen(title);
        int16_t textW = len * 6;              // стандартный шрифт 6px
        int16_t x     = (UI_W - textW) / 2;
        int16_t y     = (UI_BAR_H - 8) / 2;   // примерно по центру строки

        g.setCursor(x, y);
        g.print(title);
    }

    // Стартовые иконки WiFi/MQTT (выключены — серым)
    g.fillRect(WIFI_X, WIFI_Y, ICON_W, ICON_H, COL_BAR);
    g.drawBitmap(WIFI_X, WIFI_Y, ICON_WIFI_12, ICON_W, ICON_H, WIFI_COLOR_OFF);

    g.fillRect(MQTT_X, MQTT_Y, ICON_W, ICON_H, COL_BAR);
    g.drawBitmap(MQTT_X, MQTT_Y, ICON_MQTT_12, ICON_W, ICON_H, MQTT_COLOR_OFF);

    // Стартовые индикаторы HEAT/UV (выключены, только рамка)
    g.fillRect(HEAT_X, STATE_Y, STATE_SIZE, STATE_SIZE, COL_BAR);
    g.drawRect(HEAT_X, STATE_Y, STATE_SIZE, STATE_SIZE, HEAT_COLOR_OFF);

    g.fillRect(UV_X, STATE_Y, STATE_SIZE, STATE_SIZE, COL_BAR);
    g.drawRect(UV_X, STATE_Y, STATE_SIZE, STATE_SIZE, UV_COLOR_OFF);
}

void UiTopBar::update(bool wifiOK, bool mqttOK, bool heaterOn, bool uvOn)
{
    if (!_tft) return;

    auto& g = _tft->gfx();

    // WiFi: серый / светлый как заголовок
    if (wifiOK != _wifi) {
        _wifi = wifiOK;
        g.fillRect(WIFI_X, WIFI_Y, ICON_W, ICON_H, COL_BAR);
        g.drawBitmap(
            WIFI_X,
            WIFI_Y,
            ICON_WIFI_12,
            ICON_W,
            ICON_H,
            _wifi ? WIFI_COLOR_ON : WIFI_COLOR_OFF
        );
    }

    // MQTT: серый / фиолетовый
    if (mqttOK != _mqtt) {
        _mqtt = mqttOK;
        g.fillRect(MQTT_X, MQTT_Y, ICON_W, ICON_H, COL_BAR);
        g.drawBitmap(
            MQTT_X,
            MQTT_Y,
            ICON_MQTT_12,
            ICON_W,
            ICON_H,
            _mqtt ? MQTT_COLOR_ON : MQTT_COLOR_OFF
        );
    }

    // HEAT
    if (heaterOn != _heat) {
        _heat = heaterOn;
        g.fillRect(HEAT_X, STATE_Y, STATE_SIZE, STATE_SIZE, COL_BAR);
        uint16_t c = _heat ? HEAT_COLOR_ON : HEAT_COLOR_OFF;
        g.drawRect(HEAT_X, STATE_Y, STATE_SIZE, STATE_SIZE, c);
    }

    // UV
    if (uvOn != _uv) {
        _uv = uvOn;
        g.fillRect(UV_X, STATE_Y, STATE_SIZE, STATE_SIZE, COL_BAR);
        uint16_t c = _uv ? UV_COLOR_ON : UV_COLOR_OFF;
        g.drawRect(UV_X, STATE_Y, STATE_SIZE, STATE_SIZE, c);
    }
}
