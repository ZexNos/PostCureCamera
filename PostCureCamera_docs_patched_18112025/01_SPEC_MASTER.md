
# Post‑Cure Camera — MASTER SPEC v0.2.1
> **Status:** authoritative single source of truth.  
> **Date:** 2025-11-01T01:35:31  
> **Covers:** UI/UX, Control Logic, MQTT (telemetry‑only), Safety, Hardware (logical), Bring‑up.  
> **Supersedes:** `postcure_camera_control_logic_MQTT_HA.md` §5/8/11 (old v0.1 MQTT & Preset), duplicates in `postcure_camera_UI_Spec_v0.2_*.md` §18/20.

---

## 0. Glossary & Versioning
- **MASTER SPEC** — этот документ. Все прочие документы считаются производными.
- **FW v0.2.x** — прошивка, реализующая MQTT‑телеметрию, два вентилятора и UV‑LOCK для DRY. OTA/Recovery в этой ветке не используется.
- **UI Spec** — экраны и потоки (RUN/OPTIONS/SETTINGS/ERROR), синхронизированы с этим мастер‑документом.
- **Control Logic** — FSM, датчики/актуаторы, PID/безопасность; правила и API тут — первичны.

---

## 1. Hardware (logical)
**MCU:** ESP32‑S3‑DevKitC‑1 **(N16R8)**, 16 MB Flash + 8 MB PSRAM.  
**Sensors:** SHT31×2 (T/RH air & exhaust), DS18B20 (T_ptc).  
**Actuators:** Heater (PWM), UV LED driver(s) (PWM/EN), **Fan_INT** (PWM + tach opt.), **Fan_EXH** (PWM + tach opt.), **Turntable motor** (relay/SSR ~5 rpm), **RGB Status LED**.  
GPIO‑разводка конкретизируется в `postcure_camera_wiring.md` (см. раздел «Pin Map» там).

---

## 2. Safety & Interlocks
- **SAFE_STATE (auto)**: взводится ошибками/дверью/сенсорами, снимается после исчезновения причины и/или ACK (для критичных).
- **LOCKOUT (manual)**: пользовательская блокировка (удержание 2 s), не снимается автоматически.
- Guard‑функции:
```
bool CanRunHeater(){ return !SAFE_STATE && !LOCKOUT && (T_ptc < T_ptc_max); }
bool CanRunUV(){ return !SAFE_STATE && !LOCKOUT && (Door == CLOSED); }
```
- **Sanity check + N‑strikes**: отбрасывать NaN/вне диапазона/нереальную скорость изменения; ≥3 плохих подряд → ERROR:sensor_unstable|missing.
- **T_ptc proportional backoff** (anti‑windup) при `T_ptc > T_ptc_warn`; полный стоп при `T_ptc >= T_ptc_max`.

---

## 3. Profiles (storage & semantics)
```
typedef struct {
  bool     isDry;        // DRY=true -> UV-LOCK (UV=0)
  uint8_t  fan_int;      // 0..100
  uint8_t  fan_exh;      // 0..100
  uint8_t  uv_pct;       // 0..100 (игнорируется при isDry)
  uint16_t targetT_c;    // °C
  uint16_t duration_min; // minutes
  char     name[24];     // e.g. "PLA_55C"
} Profile;
```
- DRY‑профили: UV принудительно 0 (UV‑LOCK).
- CURE‑профили: допустим нагрев (30–50 °C) + UV.
- Хранение/выбор — меню/UI и MQTT `action:start`.

---

## 4. FSM & UI (high‑level)

### 4.2 Runtime FSM (detailed)
- **idle** — камера свободна.
- **cure** — UV-засветка по профилю.
- **dry** — сушка филамента (UV-LOCK).
- **purge** — продувка/остужение после CURE/DRY/ERROR или ручной.
- **finish** — завершение цикла, краткий переходный этап.
- **error** — критическое состояние (OverTemp/SensorFault).

**Transitions:**
- IDLE → CURE / DRY / PURGE / ERROR
- CURE → FINISH / PURGE / ERROR / IDLE
- DRY → FINISH / PURGE / ERROR / IDLE
- PURGE → IDLE / ERROR
- FINISH → IDLE / PURGE
- ERROR → IDLE / PURGE

**Rules:**
- Safety имеет приоритет над FSM.
- MQTT публикует только: state, temp_main, heater_pwm, heater_corr.
- state публикуется только при изменении.
- после потери питания: IDLE + предложение продолжить.

**States:** `BOOT → HOME_DASH → RUN_VIEW → RUN_REPORT`,  
**Runtime modes:** `idle`, `cure`, `dry`, `purge`, `finish`, `error`.  
**Errors:** `ERROR_VIEW (S800)` с `[Acknowledge]`,  
**Wi‑Fi onboarding:** AP `PostCure_Setup‑XXXX` + **Captive Portal** на первом запуске/сбросе.  
RGB‑LED: стандартная палитра статусов (BOOT/Wi‑Fi/MQTT/ERROR).

### 4.1 Global Top Status Bar
- Всегда присутствует сверху (`y=0..15`) и не зависит от текущего экрана (`RUN_VIEW`, `OPTIONS`, `ERROR`, `OTA` и т.п.).
- Слева: иконки `Wi-Fi` и `MQTT`:
  - «точка» = не подключено,
  - Wi-Fi подключён — зелёные дуги,
  - MQTT подключён — фиолетовая точка.
- По центру — текстовый заголовок текущего экрана (`RUN`, `OPTIONS`, `TEST`, `AUTOTEST`, `UV`, `DRY_...`).
- Справа — индикаторы подсистем:
  - HEAT (оранжевый индикатор) при активном нагреве,
  - UV (фиолетовый индикатор) при активном УФ.
- Нижнего бара (soft-keys / подсказки по управлению) **нет** — навигация подразумевается (энкодер + клик/лонг-клик), но не рисуется текстом.

---

## 5. MQTT — Telemetry v0.2
**Base prefix:** `postcure/{device_id}/`

### 5.1 Topics
- `.../state` — **retain JSON**, включает текущее состояние и последние значения телеметрии.
- `.../tele/now` — периодическая телеметрия (тот же набор полей, неретейн).
- `.../log` — логи/события (ERROR, start/stop и т.п.).

### 5.2 State (retain)
```json
{
  "fw": "0.2.x",
  "state": "idle|cure|dry|purge|finish|error",
  "temp_main": 38.7,
  "heater_pwm": 42,
  "heater_corr": 0.87
}
```

### 5.3 Commands
В текущей версии прошивки **команды по MQTT не поддерживаются**.
MQTT используется только как канал телеметрии и логов, управление выполняется локально через UI.

### 5.4 Telemetry (`.../tele/now`)
Периодическая телеметрия, дублирующая ключевые поля `state` (прежде всего `temp_main`, `heater_pwm`, `heater_corr`), неретейн.
Частота публикации: порядка 1 Гц в режимах CURE/DRY, реже или по событию в состояниях IDLE/FINISH.

### 5.5 Wi-Fi / MQTT configuration model
- Параметры Wi-Fi (SSID/пароль) и MQTT (broker, port, base topic) задаются через NVS / компиляционные значения.
- В основной UI-структуре **нет** экранов `Wi-Fi Setup` или `MQTT Settings`.
- Разрешён только:
  - первый запуск / reset через отдельный Wi-Fi onboarding (AP + captive portal),
  - фоновое автоподключение к заранее заданным параметрам без участия пользователя.


---

## 6. OTA (dual‑slot) & Recovery
**Partitions (example):**
```
# Name, Type, SubType, Offset,  Size
nvs, data, nvs,     0x9000,  0x6000,
otadata, data, ota, 0xF000,  0x2000,
app0, app,  ota_0,  0x11000, 0x190000,
app1, app,  ota_1,  0x1A1000,0x190000,
spiffs,data,spiffs, 0x331000,0x4F000,
```
Процесс: URL → скачивание в неактивный слот → проверка → переключение → ребут → автооткат при сбое.  
UX: остановить RUN, `OTA_VIEW`, RGB blue‑breath.  
**Recovery (NVS `session_recovery`)**: сохранять снимок каждые 30–60 s; при BOOT — `RECOVERY_PROMPT` с предложением возобновить; очищать при `RUN_REPORT`/`ACT_STOP`.

---

## 7. Logging
Кольцевой буфер в RAM (50–100 записей `LogEntry{ts,msg}`), отображение в SERVICE и дублирование в `.../log`.

---

## 8. Mapping UI ↔ MQTT (каноническая таблица)
| UI Field                   | MQTT JSON path                          | Notes |
|---                         |---                                       |---|
| Mode (Dry/Cure/Idle)       | `mode`                                   | enum |
| Phase                      | `phase`                                  | enum |
| Target T (°C)              | `profile.targetT_c`                      | int |
| Duration (min)             | `profile.duration_min`                   | int |
| UV %                       | `actuators.uv_pct` / `set.uv`            | DRY=0 (UV‑LOCK) |
| Fan INT %                  | `actuators.fan.int_pct` / `set.fan_int`  | 0..100 |
| Fan EXH %                  | `actuators.fan.exh_pct` / `set.fan_exh`  | 0..100 |
| Fan INT RPM                | `actuators.fan.int_rpm`                  | optional |
| Fan EXH RPM                | `actuators.fan.exh_rpm`                  | optional |
| Motor RPM                  | `actuators.motor_rpm`                    | optional |
| Error text                 | `err`                                    | "" or "ERROR:..." |
| Lockout                    | `lockout` / `cmd.lockout`                | bool |
| SAFE                       | `safe`                                   | bool (auto) |

---

## 9. Implementation Notes
- DRY UV‑LOCK: прошивка принудительно устанавливает `uv_pct=0`, если профиль относится к типу DRY.
- Все проценты ограничивать 0..100 на устройстве.
- Публикация MQTT-состояния (`state`) выполняется по событию (при изменении), телеметрия (`tele/now`) — с разумной частотой для настройки PID (порядка 1 Гц в режиме работы).

---

## 10. Document Relationships
- Этот MASTER SPEC **заменяет** противоречащие разделы в:
  - `postcure_camera_control_logic_MQTT_HA.md` (§5, §8, §11, OTA/Recovery уточнения),
  - `postcure_camera_UI_Spec_v0.2_*.md` (§18, §20 — считать синхронизированными).  
- `README_PostCure_Camera.md` — держит Roadmap и ссылки; не источник требований.
