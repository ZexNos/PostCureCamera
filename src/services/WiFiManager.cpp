#include "services/WiFiManager.h"

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(WIFI_HOSTNAME);

    Serial.println(F("[WiFi] begin()"));
    Serial.print(F("[WiFi] SSID="));
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _lastAttemptMs = millis();
}

const char* WiFiManager::statusToStr(wl_status_t s) const {
    switch (s) {
        case WL_IDLE_STATUS:     return "IDLE";
        case WL_NO_SSID_AVAIL:   return "NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED:  return "SCAN_COMPLETED";
        case WL_CONNECTED:       return "CONNECTED";
        case WL_CONNECT_FAILED:  return "CONNECT_FAILED";
        case WL_CONNECTION_LOST: return "CONNECTION_LOST";
        case WL_DISCONNECTED:    return "DISCONNECTED";
        default:                 return "UNKNOWN";
    }
}

void WiFiManager::tick() {
    _status = WiFi.status();
    _connected = (_status == WL_CONNECTED);

    // логируем только изменение статуса
    if (_status != _lastStatus) {
        _lastStatus = _status;
        Serial.print(F("[WiFi] status="));
        Serial.println(statusToStr(_status));
        if (_status == WL_CONNECTED) {
            Serial.print(F("[WiFi] IP="));
            Serial.println(WiFi.localIP());
        }
    }

    // если уже подключены — ничего не делаем
    if (_connected) return;

    // попытка переподключения раз в RECONNECT_INTERVAL
    if (millis() - _lastAttemptMs >= RECONNECT_INTERVAL) {
        Serial.println(F("[WiFi] reconnect()"));
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        _lastAttemptMs = millis();
    }
}
