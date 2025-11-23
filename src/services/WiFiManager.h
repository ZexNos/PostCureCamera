#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "services/NetConfig.h"

class WiFiManager {
public:
    void begin();
    void tick();

    bool isConnected() const { return _connected; }

    // для диагностики
    wl_status_t status() const { return _status; }

private:
    bool _connected = false;
    wl_status_t _status = WL_IDLE_STATUS;
    wl_status_t _lastStatus = (wl_status_t)0xFF;

    uint32_t _lastAttemptMs = 0;
    static constexpr uint32_t RECONNECT_INTERVAL = 5000; // мс

    const char* statusToStr(wl_status_t s) const;
};
