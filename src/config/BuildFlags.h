#pragma once

// ======================================================
//  BuildFlags — единый источник фич-флагов прошивки
//  Значения меняются по стадиям (Stage A/B/C…),
//  состав и имена флагов остаются постоянными.
// ======================================================

// --- Базовое железо ---
#define HAS_DISPLAY          1
#define HAS_ENCODER          1

// --- Сеть ---
#define HAS_WIFI             1    // Stage A: Wi-Fi выключен
#define WIFI_HARDCODED       1    // при включении Wi-Fi — только жёстко прошитый SSID/пароль

#define HAS_MQTT             1    // Stage A: MQTT выключен
#define MQTT_HARDCODED       1    // при включении MQTT — только жёстко прошитый брокер/топики

// --- Датчики ---
#define HAS_SENSORS_SHT31    0
#define HAS_SENSOR_DS18B20   0

// --- Исполнительные устройства ---
#define HAS_HEATER           0
#define HAS_UV               0
#define HAS_FANS             0

// --- Dev / sandbox ---
#define DEV_UI_SETTINGS      0    // никаких Wi-Fi/MQTT меню в UI
#define SCREEN_SANDBOX       0    // отдельный билд, если когда-нибудь понадобится

// --- Отладка ---
#define DBG_UI               0
#define DBG_RENDER           0
#define DBG_ENC              0
#define DISPLAY_SELFTEST     0
