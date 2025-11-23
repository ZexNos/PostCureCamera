    # Post‑Cure Camera — Network, Build Flags & Dev Infrastructure

    Этот файл объединяет BuildFlags, Wi‑Fi, MQTT, список защищённых файлов и методологию Freeze‑тестов.

    ---

    ## 1. Build Flags (BuildFlags.md)

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


    ---

    ## 2. Wi‑Fi Access Note

    (source: docs/README_Files/WiFi_Access_Note.md)

    # Wi‑Fi Access Note — Post‑Cure Camera
**Updated:** 2025-11-07 21:57

**Policy:** Wi‑Fi конфигурация **зашита в прошивку** (без экранов настройки и без NVS).  
`WiFiManager` остаётся как сервис для автоподключения и статуса в топ‑баре.

---

## Connection parameters (hardcoded)

| Параметр | Значение (пример) | Комментарий |
|---|---|---|
| **SSID** | `YOUR_SSID` | имя твоей единственной сети |
| **Password** | `YOUR_PASSWORD` | WPA2/WPA3 |
| **Mode** | `WIFI_STA` | только клиент, без AP |
| **Hostname** | `postcure-s3` | видно в роутере/DHCP |
| **IP** | DHCP | статический IP опционален |

> При смене сети просто правь константы в прошивке и прошивай заново.

---

## Spec.h (рекомендуемые константы)

```cpp
// Wi‑Fi (hardcoded)
#define WIFI_SSID      "YOUR_SSID"
#define WIFI_PASS      "YOUR_PASSWORD"
#define WIFI_HOSTNAME  "postcure-s3"
// Optional static IP (закомментировано — по умолчанию DHCP)
// #define WIFI_USE_STATIC_IP  1
// #define WIFI_STATIC_IP      IPAddress(10,0,0,120)
// #define WIFI_STATIC_GW      IPAddress(10,0,0,1)
// #define WIFI_STATIC_MASK    IPAddress(255,255,255,0)
// #define WIFI_STATIC_DNS     IPAddress(10,0,0,1)
```

---

## Service contract (WiFiManager)

- `begin()` — `WiFi.mode(WIFI_STA)`, hostname, `WiFi.setAutoReconnect(true)`, старт первой попытки.  
- `tick()` — неблокирующий FSM: попытка → ожидание → проверка → переподключение по таймеру.  
- `connected()` — `bool` для топ‑бара; `ip()` — отдаёт текущий IP (`WiFi.localIP()`).  
- Никакого UI/ввода SSID/PASS, только статус‑иконка в шапке.

Минимальный шаблон:
```cpp
void WiFiManager::begin(){
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setHostname(WIFI_HOSTNAME);
#ifdef WIFI_USE_STATIC_IP
  WiFi.config(WIFI_STATIC_IP, WIFI_STATIC_GW, WIFI_STATIC_MASK, WIFI_STATIC_DNS);
#endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void WiFiManager::tick(){
  // без delay(): проверяем статус и, если нужно, планируем повторное подключение по таймеру
  _connected = (WiFi.status() == WL_CONNECTED);
}
```

---

## UI / Build flags

- Топ‑бар показывает Wi‑Fi иконку из `AppState.wifiConnected` (перерисовка **только при изменении**).  
- `HAS_WIFI 1` (включено), **без** экранов настройки Wi‑Fi.

---

## Summary

Wi‑Fi подключение фиксировано в прошивке (SSID/PASS/hostname).  
Менять сеть — правкой `Spec.h` и перепрошивкой.  
Сервис Wi‑Fi остаётся для автоконнекта, статус идёт в топ‑бар, UI для ввода не нужен.


    ---

    ## 3. MQTT Access Note

    (source: docs/README_Files/MQTT_Access_Note.md)

    # MQTT Access Note — Post‑Cure Camera
**Updated:** 2025-11-07 21:49

**Broker:** встроенный Mosquitto‑аддон Home Assistant  
**Role:** Home Assistant acts as MQTT broker; Post‑Cure Camera acts as client  

---

## Connection parameters

| Параметр | Значение | Комментарий |
|-----------|-----------|--------------|
| **Host** | `10.0.0.50` | IP Home Assistant |
| **Port** | `1883` | без TLS (обычный MQTT) |
| **User / Pass** | `vlad` / `••••` | учётка MQTT‑аддона |
| **Client ID** | `postcure-s3` | уникальное имя ESP32 |
| **Discovery prefix** | `homeassistant` | для автоматического обнаружения |
| **Topic root** | `postcure/` | все темы устройства |

---

## Security / protocol notes

- Никаких **API‑токенов HA** или OAuth‑ключей не требуется — только логин/пароль Mosquitto.  
- **TLS/SSL (порт 8883)** не используется в локальной сети.  
- При необходимости допускается добавить Last Will topic `postcure/status/online`.

---

## Example topics

```
postcure/status/heater   → "ON" / "OFF"
postcure/status/uv       → "ON" / "OFF"
postcure/status/temp     → "43.1"
postcure/status/hum      → "27.8"
homeassistant/switch/postcure/heater/config  → { JSON для discovery }
```

---

## Summary

Post‑Cure Camera подключается к брокеру по IP **10.0.0.50** на **1883/tcp**,  
аутентификация — **vlad + пароль**, без TLS.  
Для Home Assistant действует префикс обнаружения `homeassistant/`;  
все собственные темы камеры начинаются с `postcure/`.  
Никаких API‑токенов не нужно.


    ---

    ## 4. Protected Files — Frozen UI Hardware Layer

    (source: docs/PROTECTED_FILES.md)

    # PROTECTED FILES — FROZEN UI HARDWARE LAYER

Этот файл фиксирует список исходников, которые считаются **замороженным ядром** UI-железа
(Post-Cure Camera, Stage A). Их поведение считается эталонным и проверяется отдельными
FREEZE-тестами. Любые изменения возможны только через явную процедуру UNFREEZE
с обновлением FREEZE-тестов и документации.

## 1. Драйверы дисплея (ST7789, 128×160)

Локальный стек драйверов под нестандартное разрешение и раскладку зон экрана.

- `src/drivers/Adafruit_ST77xx.h`
- `src/drivers/Adafruit_ST77xx.cpp`
- `src/drivers/Adafruit_ST7789.h`
- `src/drivers/Adafruit_ST7789.cpp`
- `src/drivers/DisplayST7789.h`
- `src/drivers/DisplayST7789.cpp`

Заморожено:

- режим работы дисплея (128×160, альбомная ориентация),
- разделение экрана на зоны (TOP / BODY / BOTTOM),
- API отрисовки системной шапки и нижней подсказки,
- политика частичной перерисовки без полного `fillScreen()`.

Разрешены только косметические изменения, не влияющие на поведение:

- правки комментариев,
- форматирование кода,
- незначительные рефакторы include-ов при условии идентичного результата.

## 2. Драйвер энкодера

- `src/drivers/Encoder.h`
- `src/drivers/Encoder.cpp`

Заморожено:

- использование EncButton 3.7.3,
- логика обработки шагов, кликов, удержаний,
- соглашения по возвращаемым событиям (`left()`, `right()`, `click()`, `hold()`).

Любые функциональные изменения требуют UNFREEZE.

## 3. Конфигурация пинов и цветов UI

- `src/config/Pins.h`
- `src/config/Colors.h`

Заморожено:

- назначение пинов для дисплея, энкодера и RGB-индикации,
- базовая цветовая схема UI (тёмный фон, акцентный оранжевый),
- геометрия зон экрана (высота верхней/нижней полосы и т.п.).

Изменения возможны только при смене железа или утверждённого UI-гайда
и должны сопровождаться обновлением соответствующих документов.

## 4. Что не заморожено

Следующие файлы являются **каноническими по структуре**, но **не заморожены
по значениям**:

- `src/config/BuildFlags.h`

Структура и имена флагов должны сохраняться; значения флагов могут
меняться от стадии к стадии (Stage A/B/C…) и между различными прошивками
(например, DEV/PROD), без процедуры UNFREEZE.


    ---

    ## 5. Freeze Test Methodology

    (source: docs/README_Files/FREEZE_TESTS_PostCureCamera_v1.0.md)

    # Post‑Cure Camera — Freeze Test Methodology (v1.0)
**Updated:** 2025-11-08 08:06

Цель: после выполнения этих тестов модуль считается «закрытым/замороженным» (больше не меняем код без явного unfreeze).  
Область: DisplayST7789, EncoderDrv, Pins.h, Colors.h, а также слой UiRenderer (top/bottom bar + CONTENT diff).

---

## 0) Общие условия (Fixture)
- Плата: ESP32‑S3 DevKitC‑1 (N16R8); прошивка = текущий билд Stage A/B/C с RUN VIEW.
- Экран: ST7789 128×160, ориентация landscape.
- Энкодер: подключён согласно Pins.h; EncButton 3.7.3.
- Питание: USB‑C (комп) и 24V PSU через DC‑DC (оба сценария протестированы).
- Serial 115200 для логов.
- **Собрать с флагами диагностики** (например):  
  `#define DBG_UI 1`, `#define DBG_ENC 1`, `#define DBG_RENDER 1`.

---

## 1) DisplayST7789 — Render Stability & Diff Drawing
**Цель:** фикс‑шапка/низ‑бар, частичная перерисовка CONTENT, отсутствие мерцаний.

### 1.1 Каркас зон
**Шаги:**
1) Загрузить RUN VIEW.  
2) Через Serial вывести геометрию зон, которая использует драйвер:  
   `TOP=0..13, CONTENT=14..113, BOTTOM=114..127`.

**Критерий PASS:** логи совпадают с эталоном; визуально бары ровно по краям.

### 1.2 Отсутствие полного клира
**Шаги:** 30 сек листать меню (вниз/вверх) и переключать экраны (3–4 раза/сек).  
**Метрика:** счётчик полных очисток экрана (`full_clear_count`).  
**Критерий PASS:** `full_clear_count == 0` (допускается клир ТОЛЬКО CONTENT).

### 1.3 Дифф‑отрисовка списков
**Шаги:** на длинном списке прокрутить 300 «щёлков» вниз и 300 вверх.  
**Метрика:** `diff_draw_calls` растут; `full_draw_calls` не растут.  
**Критерий PASS:** `full_draw_calls == 0` на серии; FPS визуально стабильный.

### 1.4 Кэш топ‑/боттом‑баров
**Шаги:** включать/выключать Wi‑Fi/MQTT индикаторы (симуляция через AppState).  
**Метрика:** `topbar_redraw_count` и `bottombar_redraw_count` увеличиваются ТОЛЬКО при смене значений.  
**Критерий PASS:** нет «лишних» перерисовок на каждом `tick()`.

### 1.5 Артефакты/мерцание
**Шаги:** 60 сек агрессивно крутить энкодер + кликать.  
**Критерий PASS:** нет белых вспышек, смазывания, «дрожания» текста.

---

## 2) EncoderDrv — Event Integrity
**Цель:** корректные события без дребезга и потерь.

### 2.1 Базовая матрица событий
**Шаги:** выполнить последовательно: Left×10, Right×10, Click×5, LongPress×3, DoubleClick×3.  
**Метрика:** лог событий `EncEvent` должен содержать точные последовательности.  
**Критерий PASS:** 100% совпадение последовательностей, без «лишних» событий.

### 2.2 Высокая скорость
**Шаги:** прокрутить быстро 5× по 20 «щёлков» (в обе стороны).  
**Метрика:** счётчик шагов UI == количеству детентов.  
**Критерий PASS:** потерь 0; нет «дублей» на один детент.

### 2.3 Дребезг кнопки
**Шаги:** 20 быстрых кликов подряд.  
**Критерий PASS:** 20 Click, не больше/не меньше; LongPress/DoubleClick не возникают ложно.

---

## 3) Pins.h — Final Pinout Validation
**Цель:** пины соответствуют проводке и не конфликтуют.

### 3.1 Экран/SPI
**Шаги:** подтянуть частоту SPI до эталонной; проверить, нет ли полос/битых пикселей.  
**Критерий PASS:** картинка чистая; SPI ошибки отсутствуют.

### 3.2 I²C (SHT31×2)
**Шаги:** сканер шины — два адреса видны стабильно.  
**Критерий PASS:** оба датчика читаются, без NACK/LOST на длительном логе (1 мин).

### 3.3 1‑Wire (DS18B20)
**Шаги:** 60 измерений подряд.  
**Критерий PASS:** 60/60 успешных чтений; время ответа стабильно.

### 3.4 PWM/LEDC (Fans/SSR)
**Шаги:** установить 10%, 50%, 90% duty на каждом канале; слушать шум/гул.  
**Критерий PASS:** частота стабильна, вентиляторы не свистят, SSR ШИМится ожидаемо.

---

## 4) Colors.h — Style Consistency
**Цель:** UI использует только константы палитры, контраст корректный.

### 4.1 Палитра в коде
**Шаги:** статический анализ — поиск «магических» RGB вне Colors.h.  
**Критерий PASS:** 0 вхождений.

### 4.2 Контраст и читаемость
**Шаги:** на RUN VIEW проверить текст/иконки на тёмном фоне.  
**Критерий PASS:** все надписи читаемы под разными углами; иконки статусов различимы.

---

## 5) Инструментирование (референсные счётчики/логи)

Добавить в `UiRenderer/DisplayST7789` (на время тестов, флагом `DBG_RENDER`):
```cpp
volatile uint32_t full_clear_count = 0;
volatile uint32_t full_draw_calls = 0;
volatile uint32_t diff_draw_calls = 0;
volatile uint32_t topbar_redraw_count = 0;
volatile uint32_t bottombar_redraw_count = 0;
```

В `clearContent()` не трогаем TOP/BOTTOM. Полный клир экрана — запрещён.
В Serial раз в 5 сек печатать сводку счётчиков.

В `EncoderDrv` (флаг `DBG_ENC`) печатать событие по `pollEvent()`.

---

## 6) Критерии «заморозки» файла
Файл объявляется **frozen**, когда все относящиеся к нему тесты выше прошли одновременно:
- DisplayST7789.* → раздел 1 (1.1–1.5) — все PASS.  
- Encoder.* → раздел 2 (2.1–2.3) — все PASS.  
- Pins.h → раздел 3 (3.1–3.4) — все PASS.  
- Colors.h → раздел 4 (4.1–4.2) — все PASS.

**После этого**:
- Добавить запись в `PROTECTED_FILES.md` с датой и ссылкой на лог тестов.
- В README отметить «Frozen as of <date>».

---

## 7) Быстрый Smoke‑script (опционально)
Псевдокод (вызывается из App.tick() под флагом теста):
```cpp
static uint32_t t0 = millis();
if (millis() - t0 > 5000) {  // каждые 5 сек
  Serial.printf("R:{fc=%lu, fd=%lu, dd=%lu, tb=%lu, bb=%lu}\n",
    full_clear_count, full_draw_calls, diff_draw_calls,
    topbar_redraw_count, bottombar_redraw_count);
  t0 = millis();
}
```
Снять флаги `DBG_*` после успешной «заморозки».

