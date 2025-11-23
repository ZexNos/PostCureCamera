#pragma once
#include <Arduino.h>

// ============================================================================
//  NETWORK HARD-CODED CONSTANTS (Wi-Fi + MQTT)
//  Всё, что нужно менять — находится здесь. Никаких строк в cpp.
// ============================================================================

// ---------------------------
// Wi-Fi settings
// ---------------------------
static const char* WIFI_SSID     = "ZexNet";
static const char* WIFI_PASSWORD = "f2DP4Dvb";
static const char* WIFI_HOSTNAME = "PostCureCamera";

// ---------------------------
// MQTT settings
// ---------------------------
static const char* MQTT_HOST     = "10.0.0.50";  // брокер
static const uint16_t MQTT_PORT  = 1883;
// MQTT AUTH
static const char* MQTT_USER = "vlad";
static const char* MQTT_PASS = "121086";   // прописываешь свой

// Home Assistant discovery prefix
static const char* MQTT_DISCOVERY_PREFIX = "homeassistant";


// Topics
static const char* MQTT_SUB_COMMAND = "postcure/command";   // входящие команды
static const char* MQTT_PUB_STATUS  = "postcure/status";    // онлайн/температура и т.п.
static const char* MQTT_PUB_PING    = "postcure/ping";      // heartbeat (по желанию)
static const char* MQTT_SELFTEST_TOPIC = "postcure/selftest";  // self-test loopback
