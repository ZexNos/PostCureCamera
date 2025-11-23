#pragma once

#include <Arduino.h>
#include "config/BuildFlags.h"

#include "drivers/DisplayST7789.h"
#include "drivers/Encoder.h"

#include "ui/UiRouter.h"
#include "ui/TopBar.h"
#include "ui/screens/RunView.h"

#if HAS_WIFI
  #include "services/WiFiManager.h"
#endif

#if HAS_MQTT
  #include "services/MqttManager.h"
#endif

class App {
public:
    void begin();
    void tick();

private:
    DisplayST7789 tft;
    EncoderDrv    enc;

    UiRouter      ui;
    UiTopBar      topBar;
    RunView       runView;

#if HAS_WIFI
    WiFiManager   wifi;
#endif

#if HAS_MQTT
    MqttManager   mqtt;
#endif

    uint8_t  runSelected      = 0;
    uint32_t lastTopbarUpdate = 0;
};
