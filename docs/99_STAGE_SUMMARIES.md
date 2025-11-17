    # Post‑Cure Camera — Stage Summaries

    Этот файл агрегирует краткие отчёты по стадиям A и B.

    ---

    ## Stage A — Display + Encoder (Basic UI Framework)

    (source: docs/postcure_stageA_summary.md)

    # 🧩 PostCure Camera — Stage A Summary  
**Этап:** Stage A — Display + Encoder (Basic UI Framework)  
**Дата завершения:** 04 ноября 2025  
**Версия библиотеки:** `PostCureCore v0.1.1`  
**Платформа:** ESP32-S3-DevKitC-1 (N16R8)  
**Экран:** ST7789 128×160  
**Энкодер:** EncButton 3.7.3  

---

## ✅ Результаты и достижения

| Компонент | Описание | Статус |
|------------|-----------|:------:|
| **Display Driver** | Реализован `DisplayST7789` с частичной перерисовкой, без мерцаний и подрагиваний верхней строки. | ✅ |
| **Encoder Driver** | На основе EncButton 3.7.3 — устойчивое распознавание шагов и кликов, без дребезга. | ✅ |
| **App Core** | Класс `App` — модульный контроллер приложения с методами `begin()` и `tick()`. | ✅ |
| **UI System** | Экран `RUN VIEW` в эталонном стиле: верхний бар + контентная зона + динамическое обновление значений. | ✅ |
| **UI Top Bar** | Кэшированные иконки (Wi-Fi, MQTT, Heater, UV) — обновление только при изменении состояния. | ✅ |
| **Project Structure** | Библиотека `PostCureCore` с директориями `app/`, `drivers/`, `ui/`, `config/` + пример StageA_RunView. | ✅ |

---

## 📁 Структура библиотеки PostCureCore v0.1.1
```
PostCureCore/
  ├─ src/
  │   ├─ app/           (App.h, App.cpp)
  │   ├─ drivers/       (DisplayST7789.*, Encoder.*)
  │   ├─ ui/            (UiRouter.*, screens/RunView.*)
  │   └─ config/        (Pins.h, Colors.h, BuildFlags.h)
  ├─ examples/
  │   └─ StageA_RunView/StageA_RunView.ino
  └─ library.properties  version=0.1.1
```

---

## ⚙️ Технические особенности
- Частичная перерисовка («dirty region») для строки значения — без полного обновления экрана.  
- Верхний бар разделён на статичную и динамическую части (POSTCURE + иконки).  
- Все вызовы отрисовки вынесены в `App`, драйвер не управляет UI.  
- Полная совместимость с Adafruit GFX/ST7789 и EncButton 3.7.3.  
- Производительность: ~60 FPS при минимальной нагрузке на CPU.

---

## 🔖 Версия и метаданные
- **PostCureCore:** v0.1.1  
- **Arduino Core ESP32:** 3.3.2  
- **Adafruit GFX:** 1.12.3  
- **Adafruit ST7735/ST7789:** 1.11.0  
- **EncButton:** 3.7.3  
- **GyverIO:** 1.3.12  

---

## 🚀 План Stage B (Wi-Fi Subsystem)
**Цель:** добавить подсистему Wi-Fi и экран настройки сети.  
**Ключевые задачи:**
1. Создать `drivers/WifiManager.cpp/.h` (инициализация, автоподключение, сохранение SSID/PASS).  
2. Создать `ui/screens/WiFiMenu.cpp/.h` с вводом текста через энкодер.  
3. Реализовать взаимодействие `App ↔ UiRouter ↔ WiFiMenu`.  
4. Сохранять настройки в NVS.  

---

**Заключение:** Stage A официально завершён 04.11.2025.  
Эта версия является базовой платформой для дальнейших этапов ( Stage B–D ).


    ---

    ## Stage B — Wi‑Fi Subsystem (Connectivity Layer)

    (source: docs/POSTCURE_STAGEB_SUMMARY.md)

    # Post-Cure Camera — Stage B Summary  
📦 **Subsystem:** Wi-Fi (Connectivity Layer)  
📅 **Completed:** 2025-11-05  
🧩 **Version:** PostCureCore v0.1.1-B

---

## ✅ Stage B Completed

**Goal:** Implement a fully modular Wi-Fi subsystem with persistent connection handling, Home Assistant readiness, and UI integration on ST7789 (128×160).

### Achieved Objectives
| Component | Status | Description |
|------------|---------|-------------|
| **WiFiManager** | ✅ Done | Non-blocking async manager for STA mode. Supports auto-reconnect, event callbacks via `ARDUINO_EVENT_*`, network scanning, connection state machine, and NVS erase on *Forget*. |
| **WiFiSetup UI** | ✅ Done | Interactive UI for network selection, scrolling list, password entry, and connection feedback with `[OK]` / `[Forget]` options. |
| **Top Bar Integration** | ✅ Done | Real-time Wi-Fi icon update reflecting link state. |
| **App Router** | ✅ Done | Unified tick loop; route between `RunView` and `WiFiSetup`; proper encoder management. |
| **Persistence (NVS)** | ✅ Verified | Network credentials survive reboot; erased correctly after *Forget*. |
| **Architecture** | ✅ Modular | Clean separation between service and UI layers; no cyclic includes. |
| **EncoderDrv** | ✅ Stable | Uses EncButton 3.7.3 (`EB_STEP4_LOW`); 1 click = 1 step, no bounce artifacts. |

---

## 🧱 File Structure (Stage B Snapshot)

```
PostCureCore/
 ├─ src/
 │   ├─ app/
 │   │   ├─ App.h
 │   │   └─ App.cpp
 │   ├─ drivers/
 │   │   ├─ Encoder.h / Encoder.cpp
 │   │   └─ DisplayST7789.h / DisplayST7789.cpp
 │   ├─ services/
 │   │   └─ wifi/
 │   │       ├─ WiFiManager.h
 │   │       └─ WiFiManager.cpp
 │   └─ ui/
 │       └─ screens/
 │           ├─ RunView.h / RunView.cpp
 │           ├─ WiFiSetup.h
 │           └─ WiFiSetup.cpp
 ├─ config/
 │   ├─ Colors.h
 │   ├─ Pins.h
 │   └─ ...
 └─ examples/
     └─ StageB_WiFiTest/
         └─ StageB_WiFiTest.ino
```

---

## 🔬 Verified Behaviors
- Asynchronous scan & connect  
- Real-time Wi-Fi icon state update  
- Reconnect on link loss / router block  
- Persistent credentials (NVS)  
- Clean *Forget* operation → NVS erase + restart  
- Smooth encoder navigation (scrolling window of 5 entries)

---

## 🔄 Next Stage — Stage C (MQTT Subsystem)
- Implement `MqttManager` (async client)  
- Handle broker credentials via UI menu  
- Reflect MQTT status icon in top bar  
- Integrate with Home Assistant topics  
- Persist broker data → NVS  

---

### 🔒 Archival Note
Archive this folder as:

```
PostCureCore_StageB_2025-11-05.zip
```

and place it in:

```
/archive/PostCureCamera/
```

This archive marks the **official completion of Stage B (Wi-Fi Subsystem)**.

---

**Maintainer:** Zex Nos  
**Hardware Target:** ESP32-S3-DevKitC-1 (N16R8)  
**Display:** ST7789 (128×160, landscape)  
**Firmware Branch:** PostCureCore v0.1.1 Stage B

