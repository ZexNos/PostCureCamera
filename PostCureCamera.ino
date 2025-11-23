#include <Arduino.h>
#include <WiFiClient.h> 

// 1. Драйверы и виджеты
#include "drivers/DisplayST7789.h"
#include "drivers/Encoder.h"
#include "config/Pins.h"
#include "ui/widgets/TextEdit.h" // Включает extern TextEditOverlay TextEdit;

// 2. Главное Приложение
#include "app/App.h"

// ====================================================================
// ГЛОБАЛЬНЫЕ ОБЪЕКТЫ (Драйверы и Клиенты)
// ====================================================================

// Драйвер дисплея (создаётся с пинами из Pins.h)
DisplayST7789 display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST, PIN_TFT_SCLK, PIN_TFT_MOSI);

// Драйвер энкодера (использует пины из Pins.h)
EncoderDrv encoder;

// Общий объект для текстового редактора
// TextEditOverlay TextEdit; // <-- УДАЛЕНО: Определение глобального объекта TextEdit 
                            //     происходит ТОЛЬКО в src/ui/widgets/TextEdit.cpp.
                            //     Здесь мы используем его, благодаря extern-декларации в TextEdit.h.

// Сетевой клиент (требование PubSubClient/MqttManager)
WiFiClient wifiClient; 

// Главный класс приложения (инъекция зависимостей)
// Объект TextEdit теперь доступен как глобальная переменная.
App app(display, encoder, wifiClient, TextEdit);

// ====================================================================
// ARDUINO Ядро
// ====================================================================

void setup() {
  Serial.begin(115200);
  app.begin();
}

void loop() {
  app.tick();
}