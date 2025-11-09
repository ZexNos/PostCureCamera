
# 🧠 Post‑Cure Camera Project — Master Index

> Все методические и технические документы находятся в папке [`docs/README_Files`](docs/README_Files).

Этот README связывает все ключевые файлы проекта и служит входной точкой для работы с логикой, схемами, тестами и прошивкой.

---

## 📎 Связанные документы проекта

| Раздел | Файл | Назначение |
|--------|------|------------|
| **1. BOM / Комплектующие** | [postcure_camera_BOM_20251026_222103.md](sandbox:/mnt/data/postcure_camera_BOM_20251026_222103.md) | Финальный перечень компонентов, ревизия v2.0, все купленные/подтверждённые элементы. |
| **2. Схемы и подключения** | [postcure_camera_wiring.md](sandbox:/mnt/data/postcure_camera_wiring.md) | Полная разводка: ESP32-S3, SSR, мотор, UV-диоды, питание, BC337-ключи, датчики. |
| **3. Логика и MQTT-интеграция** | [postcure_camera_control_logic_MQTT_HA.md](sandbox:/mnt/data/postcure_camera_control_logic_MQTT_HA.md) | Основная логика, MQTT-структура, Home Assistant интеграция, PID-контуры, пресеты. |
| **4. Bring-Up / Тестирование железа** | [postcure_camera_bringup_TEST_UI.md](sandbox:/mnt/data/postcure_camera_bringup_TEST_UI.md) | Режим отладки компонентов, макет интерфейса ST7735, self-test сценарии. |
| **5. Прошивка-шаблон** | [postcure_bringup_template.ino](sandbox:/mnt/data/postcure_bringup_template.ino) | Базовый Arduino-скетч для проверки компонентов и верстки интерфейса. |

---

## 📘 Краткое описание проекта

**Post‑Cure Camera** — универсальная камера для отверждения смол и сушки филамента.  
Базируется на **ESP32‑S3‑DevKitC‑1 (rev 2022 v1.3)** с датчиками **SHT31 × 2** и **DS18B20**, дисплеем **ST7735 1.8″ SPI**, RGB‑индикатором и управлением через **MQTT / Home Assistant**.

### Основные функции
- **UV‑отверждение**: 22 × 3 Вт / 400 нм светодиоды, мягкий драйв, SSR‑коммутация.
- **Сушка филамента**: температурный диапазон 45–90 °C, внутренний и выхлопной вентиляторы, без УФ (режим UV‑LOCK).
- **PID контроль температуры** с feed‑forward компенсацией и автотестами для подбора Kp/Ki/Kd.
- **Wi‑Fi + MQTT интеграция** (Home Assistant Discovery, телеметрия 1 Гц).
- **Безопасность:** концевик двери, термопредохранители, SAFE‑режим блокировки УФ/нагрева.
- **Режим Bring‑Up:** визуальный интерфейс для проверки модулей (дисплей, светодиоды, мотор, SSR).

---

## ⚙️ Текущее состояние

✅ Все ключевые компоненты закуплены и протестированы по BOM v2.0.  
🟢 Начата интеграция RGB‑индикатора и экрана ST7735.  
🟠 В работе: реализация логики Bring‑Up UI и дальнейшее объединение с MQTT‑контроллером.  

---

© 2025 Post‑Cure Camera Project

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
