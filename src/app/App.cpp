#include "app/App.h"

namespace {
    constexpr uint32_t TOPBAR_UPDATE_MS = 250;
}

void App::begin()
{
	Serial.begin(115200);
    delay(200);
    // --- init HW ---
    tft.begin();
    tft.setInverted(false);

    enc.begin();
    ui.begin(&tft);

    // --- init top bar ---
    topBar.begin(&tft);
    topBar.drawStatic("RUN");   // название текущего экрана

    // --- init Run View ---
    runView.begin(&tft);
    runSelected = 0;
    runView.drawStatic();
    runView.setSelected(runSelected);

    lastTopbarUpdate = millis();

#if HAS_WIFI
    wifi.begin();
#endif

#if HAS_MQTT
    mqtt.begin();
#endif
}

void App::tick()
{
    enc.tick();

    // --- Wi-Fi / MQTT ---
    bool wifiOK = false;
    bool mqttOK = false;

#if HAS_WIFI
    wifi.tick();
    wifiOK = wifi.isConnected();
#endif

#if HAS_MQTT
    mqtt.tick(wifiOK);          // MQTT пытаемся крутить только при наличии Wi-Fi
    mqttOK = mqtt.isAlive();    // «работает» только если реально есть входящие
#endif

    // --- RUN VIEW навигация ---
    bool changed = false;

    if (enc.left()) {
        if (runSelected > 0) {
            runSelected--;
            changed = true;
        }
    }

    if (enc.right()) {
        if (runSelected < 2) {
            runSelected++;
            changed = true;
        }
    }

    if (changed) {
        runView.setSelected(runSelected);
    }

    if (enc.click()) {
        // переходы будут позже
    }

    // --- TopBar periodic update ---
    uint32_t now = millis();
    if (now - lastTopbarUpdate >= TOPBAR_UPDATE_MS) {

        bool heaterOn = false;  // будут привязаны к реальным сервисам позже
        bool uvOn     = false;

        topBar.update(wifiOK, mqttOK, heaterOn, uvOn);
        lastTopbarUpdate = now;
    }
}
