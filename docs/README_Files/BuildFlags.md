# BuildFlags — Конфигурация сборки (v1.0)
**Updated:** 2025-11-09 11:04

Цель: единая таблица флагов, определяющих включённые подсистемы и дев‑режимы.  
Флаги задаются в `config/BuildFlags.h` (или через `platformio.ini`/`build_flags`).

---

## Основные флаги подсистем

| Флаг | Тип | По умолчанию | Что включает |
|---|---|---:|---|
| `HAS_DISPLAY` | bool | `1` | Драйвер ST7789 160×128 (landscape), top/bottom bar, CONTENT‑рендер |
| `HAS_ENCODER` | bool | `1` | Драйвер EncButton (вращение, клик, long/double) |
| `HAS_WIFI` | bool | `1` | Инициализацию Wi‑Fi STA, статус в топ‑баре |
| `WIFI_HARDCODED` | bool | `1` | Wi‑Fi SSID/PASS/hostname зашиты, без UI/NVS |
| `HAS_MQTT` | bool | `1` | Инициализацию MQTT клиента, статус в топ‑баре |
| `MQTT_HARDCODED` | bool | `1` | broker/ip/port/user/pass зашиты, без UI/NVS |
| `HAS_SENSORS_SHT31` | bool | `1` | Чтение SHT31 (air in/out) по I²C |
| `HAS_SENSOR_DS18B20` | bool | `1` | Чтение DS18B20 (heater/air) по 1‑Wire |
| `HAS_HEATER` | bool | `1` | SSR PWM управление нагревателем |
| `HAS_UV` | bool | `1` | Управление UV каналом(и) (через драйверы) |
| `HAS_FANS` | bool | `1` | Внутренний/выхлопной вентиляторы (LEDC PWM) |

> Примечание: при `WIFI_HARDCODED=1` и `MQTT_HARDCODED=1` любые экраны Wi‑Fi/MQTT должны быть **выключены** (см. DEV‑флаги).

---

## DEV/DEBUG флаги

| Флаг | Тип | По умолчанию | Назначение |
|---|---|---:|---|
| `DEV_UI_SETTINGS` | bool | `0` | Включает скрытые экраны Wi‑Fi/MQTT **только для дев‑режима** |
| `SCREEN_SANDBOX` | bool | `0` | Режим песочницы для изолированного теста одного экрана |
| `DBG_UI` | bool | `0` | Подробные логи UI/роутера |
| `DBG_RENDER` | bool | `0` | Счётчики diff/full draw, диагностика мерцаний |
| `DBG_ENC` | bool | `0` | Логи событий энкодера |
| `DISPLAY_SELFTEST` | bool | `0` | Баннер «DISPLAY OK» при старте, для bring‑up |

---

## Порядок инициализации сети (жёсткий)

1. Старт `WiFi` (STA) → подключение к SSID (hostname: `postcure-s3`).  
2. При `WL_CONNECTED` → запуск `MqttManager` (broker `10.0.0.50:1883`, user/pass).  
3. Статусы отображаются в **топ‑баре**; **UI‑настроек нет** при `*_HARDCODED=1`.

---

## Матрица этапов (рекомендация)

| Этап | DISPLAY | ENCODER | WIFI | MQTT | SENSORS | HEATER/UV/FANS |
|---|---:|---:|---:|---:|---:|---:|
| Stage A | 1 | 1 | 0 | 0 | 0 | 0 |
| Stage B | 1 | 1 | 1 | 0 | 0 | 0 |
| Stage C | 1 | 1 | 1 | 1 | 0 | 0 |
| Stage D+ | 1 | 1 | 1 | 1 | 1 | 1 |

---

## Правила консистентности
- При `*_HARDCODED=1` UI‑экраны конфигурации **скрыты** (или не компилируются).  
- В релиз‑сборке `SCREEN_SANDBOX=0`, `DEV_UI_SETTINGS=0`, `DBG_* = 0`.  
- Топ/боттом‑бар — часть драйвера Display; экраны рисуют **только CONTENT**.
