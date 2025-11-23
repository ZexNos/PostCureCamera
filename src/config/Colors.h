#pragma once
#include <Arduino.h>

// RGB565 helper
#define RGB565(r,g,b) ( ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b) >> 3) )

// Reference UI palette
static const uint16_t COL_BG      = RGB565(10, 12, 14);   // dark background
static const uint16_t COL_BAR     = RGB565(18, 20, 24);   // top bar
static const uint16_t COL_TEXT    = RGB565(220, 220, 220);
static const uint16_t COL_ACCENT  = RGB565(255, 160, 64); // #FFA040
static const uint16_t COL_WARN    = RGB565(255, 64, 64);
static const uint16_t COL_DRY     = RGB565(124, 187, 236);
static const uint16_t COL_LINE    = RGB565(100, 106, 108);
static const uint16_t COL_TILE    = RGB565(26, 28, 32);
static const uint16_t COL_WIFI = RGB565(0, 255, 64);   // зелёный Wi-Fi online
static const uint16_t COL_MQTT = RGB565(107, 8, 107);  // #6B086B
static const uint16_t COL_UV   = RGB565(96, 43, 201);  // фиолетово-синий UV
static const uint16_t COL_BTN_BG        = RGB565(20, 23, 28);
static const uint16_t COL_BTN_BG_ACTIVE = RGB565(51, 55, 59);

// UI metrics
static const uint8_t UI_ROTATION  = 1;   // landscape
static const uint16_t UI_W        = 160; // ST7789 128x160 in landscape => W=160, H=128
static const uint16_t UI_H        = 128;
static const uint8_t  UI_BAR_H    = 16;