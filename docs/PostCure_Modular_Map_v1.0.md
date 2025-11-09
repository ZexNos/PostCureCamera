# Post‑Cure Camera — Modular Architecture Map (v1.0)

**Target HW:** ESP32‑S3‑DevKitC‑1 (N16R8) · **Display:** ST7789 128×160 (landscape) · **Encoder:** EncButton 3.7.3  
**Style:** Dark UI, accent #FFA040, fixed top status bar (Wi‑Fi, MQTT, Heater, UV)

> Этот файл — краткая, однозначная карта проекта, чтобы мы оба не теряли нить между ветками.

---

## 1) Жёстко фиксированная структура каталогов

```
/PostCureCamera/
  PostCureCamera.ino           // только setup()/loop() → app.begin()/app.tick()

  /app                         // координация модулей и маршрутизация экранов
    App.h
    App.cpp

  /drivers                     // «железные» абстракции (тонкие, независимые)
    DisplayST7789.h/.cpp
    Encoder.h/.cpp
    PwmLedc.h/.cpp
    SSR.h/.cpp
    FanPwm.h/.cpp
    RGB.h/.cpp
    Sht31Pair.h/.cpp
    Ds18b20.h/.cpp

  /services                    // логика уровня устройств/правил
    safety/Safety.h/.cpp
    wifi/WiFiManager.h/.cpp
    mqtt/MqttClientEx.h/.cpp
    storage/Storage.h/.cpp
    sensors/Sensors.h/.cpp
    actuators/Actuators.h/.cpp
    telemetry/Telemetry.h/.cpp

  /ui                          // визуальные экраны, роутер, без бизнес‑логики
    UiRouter.h/.cpp
    screens/RunView.cpp
    screens/Options.cpp
    screens/WiFiSetup.cpp
    screens/MqttSettings.cpp
    screens/…

  /config                      // константы и флаги сборки
    Pins.h
    BuildFlags.h
    Colors.h
    Spec.h
```

**Правила:**  
- Каждый модуль имеет `begin()` и неблокирующий `tick()`.  
- Межмодульные связи только через `App` и `UiRouter` (UI не знает «железо» напрямую).  
- Конфиги и флаги — только в `config/` (без «магических чисел» в коде).

---

## 2) Роли модулей (кто за что отвечает)

### 2.1 app/
**App** — центральный координатор.  
Владеет ссылками на все services/*, на UiRouter, DisplayST7789 и EncoderDrv.  
Экспорт: `begin()`, `tick()`.  
Задачи: FSM экранов, порядок вызова `tick()` модулей, агрегация в `AppState`, проксирование действий из UI в services.

**AppState** — включает: wifi/mqtt/safety статусы, цели, прогресс, сенсоры.

### 2.2 drivers/
Тонкие обёртки над железом. Без бизнес‑логики.  
DisplayST7789 — инициализация, примитивы, топ‑бар.  
Encoder — шаги/клики.  
PwmLedc — универсальный ШИМ.  
SSR — управление нагревателем.  
FanPwm — вентиляторы 20‑25 кГц.  
RGB — статус.  
Sht31Pair / Ds18b20 — сенсоры T/RH.

### 2.3 services/
Логика и правила:  
Safety — UV‑LOCK, T°, дверь.  
WiFiManager — STA, scan, connect, auto‑reconnect.  
MqttClientEx — PubSubClient, publish/subscribe.  
Storage — NVS (Wi‑Fi, MQTT, presets).  
Sensors — чтение и усреднение данных.  
Actuators — UV, Heater, Fans (с Safety).  
Telemetry — JSON state → MQTT.

### 2.4 ui/
UiRouter — переключение экранов и колбэки.  
Screens — RunView, Options, WiFiSetup, MqttSettings и т.д. (enter/draw/onInput/exit).

### 2.5 config/
Pins — GPIO/LEDC.  
BuildFlags — HAS_WIFI, HAS_MQTT и др.  
Colors — тёмная тема, #FFA040.  
Spec — частоты, пороги, топики.

---

## 3) Контракты интерфейсов (минимум)

```cpp
struct AppState { /* ... */ };
class App { public: void begin(); void tick(); const AppState& state() const; };
```

```cpp
class WiFiManager {
 public:
  void begin(); void tick();
  int scan(); const char* ssidAt(int i) const; int rssiAt(int i) const;
  bool connect(const char* ssid,const char* pass);
  bool connected() const; uint32_t ip() const;
};
```

```cpp
class MqttClientEx {
 public:
  void begin(); void tick();
  void setBroker(const char* ip,int port,const char* user,const char* pass,const char* id);
  bool connected() const;
  bool publish(const char* topic,const char* payload,bool retain=false);
  void onMessage(void (*cb)(const char*,const char*));
};
```

---

## 4) Потоки данных

Encoder → App → UiRouter.onInput().  
services → App → UI (через AppState).  
UI → services (действия).  
Telemetry ↔ MQTT (публикации и команды).

---

## 5) Порядок сборки и тестов (A–F)

A — Display + Encoder.  
B — Wi‑Fi.  
C — MQTT.  
D — Sensors.  
E — Actuators + Safety.  
F — Telemetry + HA.

На каждом шаге включаем только нужные флаги в BuildFlags.h.

---

## 6) Жёсткие запреты

- delay() и прочие блокировки UI.  
- «Магические» числа вне config/.  
- Вызовы экран → драйвер напрямую.  
- Временные строки из `c_str()` в UI.

---

Файл подготовлен: 2025-11-05 12:51
