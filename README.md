
# 🧠 Post‑Cure Camera Project — Master Index

> Все методические и технические документы находятся в папке [`docs/README_Files`](docs/README_Files).

Этот README связывает все ключевые файлы проекта и служит входной точкой для работы с логикой, схемами, тестами и прошивкой.

---

## 📎 Связанные документы проекта

| Раздел | Файл | Назначение |
|---|---|---|
| 1. BOM / Комплектующие | [postcure_camera_BOM_20251026_222103.md](docs/postcure_camera_BOM_20251026_222103.md) | Финальный перечень компонентов, ревизия v2.0. |
| 2. Схемы и подключения | [postcure_camera_wiring.md](docs/postcure_camera_wiring.md) | Полная разводка: ESP32-S3, SSR, мотор, UV-диоды, питание, датчики. |
| 3. Спецификация логики | [PostCure_Camera_SPEC_MASTER_v0.2.1.md](docs/PostCure_Camera_SPEC_MASTER_v0.2.1.md) | Единая спецификация (заменяет старые control_logic / bringup docs). |
| 4. Архитектурная карта | [PostCure_Modular_Map_v1.0.md](docs/PostCure_Modular_Map_v1.0.md) | Модули и связи (Wi-Fi/MQTT UI — deprecated). |
| 5. Методики / Ноты | см. раздел ниже | Все методики и ноты: BuildFlags, UI Sandbox, Freeze, Wi-Fi/MQTT, Presets и др. |

---

## 📄 Краткое описание проекта

Post-Cure Camera — камера для UV-отверждения смол и сушки филамента.  
Основана на ESP32-S3-DevKitC-1 (rev 2022 v1.3) с датчиками SHT31 ×2 и DS18B20,  
дисплеем **ST7789 160×128 (landscape)**, статус-баром и навигацией энкодером.  
Интеграция с Home Assistant по MQTT.

**Основные функции**
- UV-отверждение: 22–30 × 3 Вт (405/365 нм), мягкий драйв, SSR-коммутация.
- Сушка филамента: 45–90 °C, внутренний/выхлопной вентиляторы, **UV-LOCK** (без УФ).
- PID-контроль нагрева; пресеты CURE/DRY.
- Wi-Fi + MQTT (HARDCODED: без экранов настройки).
- Безопасность: дверь, перегрев, SensorFault; RGB-индикатор состояний.

---

## ⚙️ Текущее состояние

- 🟢 **Stage A**: экран ST7789 + энкодер — **заморожены** (см. FREEZE_TESTS).  
- 🟢 **Stage B**: Wi-Fi — **заморожен**, HARDCODED, статус в топ-баре.  
- 🟠 **Stage C**: MQTT — в работе; статус в топ-баре; интеграция с HA.  
- ⚙️ Safety / RGB / Presets — ведётся по спецификации (см. соответствующие ноты).

> Все методические файлы находятся в [`docs/README_Files`](docs/README_Files).

---
## 📘 Документы и методики (обновлено 2025-11-09 11:14)

### 🔧 Основные конфигурационные файлы
- [BuildFlags.md](sandbox:/mnt/data/BuildFlags.md) — описание флагов сборки, DEV/DBG‑режимов и матрицы этапов (A–F).  
- [PostCure_Modular_Map_v1.0.md](docs/PostCure_Modular_Map_v1.0.md) — архитектурная карта модулей (обновлена: Wi‑Fi/MQTT UI помечены deprecated).  
- [PostCure_Camera_SPEC_MASTER_v0.2.1.md](docs/PostCure_Camera_SPEC_MASTER_v0.2.1.md) — мастер‑спецификация, заменяющая старые control_logic и test_UI.

### 🧩 UI и визуальная логика
- [UI_Lists_and_Diff_Render.md](sandbox:/mnt/data/UI_Lists_and_Diff_Render.md) — принципы отрисовки, структура ListCtx, diff‑обновление CONTENT.  
- [UI_Feature_Sandbox_Methodology.md](sandbox:/mnt/data/UI_Feature_Sandbox_Methodology.md) — песочница для изолированного тестирования экранов.  
- [postcure_camera_UI_style_guide.md](docs/postcure_camera_UI_style_guide.md) — визуальный стиль и структура экранов (TOP/BOTTOM/CONTENT).

### ☢️ Безопасность, индикация, процессы
- [RGB_Safety_UVLOCK.md](sandbox:/mnt/data/RGB_Safety_UVLOCK.md) — поведение RGB‑индикатора, SAFE_STATE, UV‑LOCK.  
- [Presets.md](sandbox:/mnt/data/Presets.md) — структура и стартовые профили режимов CURE/DRY.  
- [FREEZE_TESTS_PostCureCamera_v1.0.md](sandbox:/mnt/data/FREEZE_TESTS_PostCureCamera_v1.0.md) — чек‑лист заморозки модулей Display/Encoder/Pins/Colors.

### 🌐 Сеть и интеграция
- [WiFi_Access_Note.md](sandbox:/mnt/data/WiFi_Access_Note.md) — жёсткая конфигурация Wi‑Fi (SSID, PASS, hostname).  
- [MQTT_Access_Note.md](sandbox:/mnt/data/MQTT_Access_Note.md) — параметры брокера Home Assistant (MQTT).  

> **Примечание:** Wi‑Fi и MQTT теперь инициализируются жёстко из кода (см. BuildFlags.md, WIFI_HARDCODED/MQTT_HARDCODED). UI‑экраны настройки удалены из меню.

---
### 🧱 Статус модулей (Freeze)
| Модуль | Статус | Примечание |
|--------|:--------:|------------|
| DisplayST7789 | 🟢 Frozen | Инициализация, diff‑отрисовка и зоны TOP/BOTTOM/CONTENT прошли freeze‑тесты. |
| EncoderDrv | 🟢 Frozen | Стабильные события вращения/клика/долгого нажатия. |
| Wi‑Fi Subsystem | 🟢 Frozen | Жёсткая конфигурация, стабильное подключение. |
| MQTT Subsystem | 🟠 In progress | Подключение к HA, статусы в топ‑баре. |
| Safety / RGB / Presets | ⚙️ В работе | Поведение описано в RGB_Safety_UVLOCK.md и Presets.md. |

---
