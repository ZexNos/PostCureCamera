    # Post‑Cure Camera — Architecture & Wiring

    Этот файл объединяет модульную карту проекта и текущие документы по проводке/коммутации.

    ---

    ## 1. Modular Architecture Map (v1.0)

    (source: docs/PostCure_Modular_Map_v1.0.md)

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
    actuators/Actuators.h/.cpp  // реализация Heater/Fan по контракту 07_HEATER_FAN_CONTROL.md
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
Actuators — UV, Heater, Fans (с Safety), реализация контракта Heater/Fan (см. `07_HEATER_FAN_CONTROL.md`).  
Telemetry — JSON state → MQTT.

### 2.4 ui/
UiRouter — переключение экранов и колбэки.  
Screens — RunView, Options, WiFiSetup, MqttSettings и т.д. (enter/draw/onInput/exit).

### 2.5 config/
Pins — GPIO/LEDC.  
BuildFlags — HAS_WIFI, HAS_MQTT и др.  
Colors — тёмная тема, #FFA040.  
Spec — частоты, пороги, топики.

### 2.6 Heater/Fan Control (контракт)

Логика управления PTC-нагревателем и вентилятором теплообдува вынесена в отдельный документ  
`07_HEATER_FAN_CONTROL.md`.  
Actuators и Safety должны рассматриваться как реализация этого контракта: PWM-таблицы, поведение по состояниям,
ограничения скорости изменения (slew-rate) и правила after-cool.

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


    ---

    ## 2. Wiring & Control Connections (Legacy AC Version)

    Этот раздел содержит исходный документ по проводке с использованием AC‑SSR.
    Он сохраняется как референс старой ревизии, даже если актуальная аппаратная конфигурация отличается.

    (source: docs/postcure_camera_wiring.md)

    # Post‑Cure Camera — Wiring & Control Connections

## 🧠 Общая структура

Контроллер: **ESP32‑S3‑DevKitC‑1 (N16R8)**
Питание логики: **5 В / 3.3 В (от USB или внешнего DC‑DC)**
Силовое питание: **220 В AC**
Модули коммутации: **2‑канальный SSR‑модуль (G3MB‑202P‑5 V)**

* **CH1** → управляет **драйвером UV‑LED** (через 5 В вход реле → 220 В выход к LED‑драйверу).
* **CH2** → управляет **мотором вращения тарелки (AC 220 В)**.

## ⚙️ Логика управления

ESP32 выдаёт **3.3 В логические сигналы**, а вход SSR требует **5 В**.
Поэтому каждый канал управляется через **транзистор‑ключ на BC337**.

### 🔸 Ключ BC337 (для каждого канала)

```
ESP32 GPIO ── 6.8kΩ ──┐
                     │B
                 ┌───┴───┐
CHx (IN SSR) ────┤   C   │ BC337
                 └───┬───┘
                     │E
GND (общая) ─────────┴──────────

DC+ (SSR) = +5 V
DC− (SSR) = GND (общая с ESP32)
```

### 🔹 Принцип работы

* GPIO = **HIGH (3.3 В)** → транзистор открыт → CHx замыкается на GND → канал SSR активен.
* GPIO = **LOW (0 В)** → транзистор закрыт → канал выключен.

Каждый SSR управляет 220 В линией через внутренний оптрон и симистор G3MB‑202P.

---

## 🔌 Канал 1 — UV‑LED Driver Chain

* **Выход SSR1 (AC)** → питание **LED‑драйвера**.
* **LED‑драйвер** → 20 × 3 Вт / 400 нм диоды последовательно (600 мА постоянный ток).
* **Назначение**: включение/выключение УФ‑подсветки.

```
ESP32 GPIO_1 → BC337 → SSR CH1 → AC 220 В → LED Driver → 20 × LEDs
```

---

## 🔌 Канал 2 — Rotation Motor

* **Выход SSR2 (AC)** → питание **моторчика поворота тарелки (220 В)**.
* **Назначение**: включение вращения во время сушки/отверждения.

```
ESP32 GPIO_2 → BC337 → SSR CH2 → AC 220 В → Motor
```

---

## ⚡ Питания и общие линии

| Линия    | Напряжение             | Источник                | Комментарий                                 |
| -------- | ---------------------- | ----------------------- | ------------------------------------------- |
| 3.3 В    | Логика ESP32           | Встроенный стабилизатор | Только сигналы GPIO                         |
| 5 В      | Питание SSR и датчиков | DC‑DC или USB 5 В       | Общая земля с 3.3 В                         |
| 220 В AC | Силовая часть          | Сеть                    | Через SSR коммутируется LED‑драйвер и мотор |

---

## 🔒 Безопасность

* Все линии 220 В должны идти через клеммники и изоляцию (термоусадка, пайка в коробке).
* **SSR‑модуль на 5 В** должен иметь теплоотвод при длительной нагрузке.
* Общая GND между ESP32 и модулем SSR обязательна, но **не соединять** с сетевой землёй 220 В.

---

## 🔧 Следующие узлы (будут добавлены позже)

* Подключение датчиков **SHT31 × 2** и **DS18B20**.
* Подключение **нагревателя 200 Вт (AC)** через отдельное SSR.
* Схемы вентиляторов (внутренний и выхлопной, 24 В DC).
* Контроль влажности и температуры.

---

## 📘 Примечание

Для визуализации электрических связей (в формате SVG или ASCII) в следующих версиях файла будут добавлены простые схемы от ESP32 до реле и нагрузки, а также группировка узлов по зонам (логика / силовая / сенсорная).


