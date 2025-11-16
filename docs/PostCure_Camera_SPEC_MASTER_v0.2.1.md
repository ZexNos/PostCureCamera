
# Post‑Cure Camera — MASTER SPEC v0.2.1
> **Status:** authoritative single source of truth.  
> **Date:** 2025-11-01T01:35:31  
> **Covers:** UI/UX, Control Logic, MQTT, Safety, OTA/Recovery, Hardware (logical), Bring‑up.  
> **Supersedes:** `postcure_camera_control_logic_MQTT_HA.md` §5/8/11 (old v0.1 MQTT & Preset), duplicates in `postcure_camera_UI_Spec_v0.2_*.md` §18/20.

---

## 0. Glossary & Versioning
- **MASTER SPEC** — этот документ. Все прочие документы считаются производными.
- **FW v0.2.x** — прошивка, реализующая Unified MQTT, два вентилятора, UV‑LOCK для DRY, OTA/Recovery.
- **UI Spec** — экраны и потоки (RUN/OPTIONS/SETTINGS/ERROR/OTA), синхронизированы с этим мастер‑документом.
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
**States:** `BOOT → (RECOVERY_PROMPT?) → HOME_DASH → RUN_VIEW → RUN_REPORT`,  
**Errors:** `ERROR_VIEW (S800)` с `[Acknowledge]`,  
**OTA:** `OTA_VIEW (S995)` — блокирующий прогресс, «Updating… Do not power off».  
**Wi‑Fi onboarding:** AP `PostCure_Setup‑XXXX` + **Captive Portal** на первом запуске/сбросе.  
RGB‑LED: стандартная палитра статусов, включает `OTA=blue breathing`, `RECOVERY=orange/violet`.

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

## 5. MQTT — Unified v0.2
**Base prefix:** `postcure/{device_id}/`

### 5.1 Topics
- `.../state` — **retain JSON**, источник правды.
- `.../tele/now` — быстрая телеметрия (subset), неретейн.
- `.../cmd` — команды JSON.
- `.../log` — логи/события.
- `.../ota/state` — статусы OTA (retain).
- `.../ota/progress` — прогресс OTA (0–100, неретейн).

### 5.2 State (retain)
```
{
  "fw":"0.2.x",
  "mode":"idle|dry|cure|service|error",
  "phase":"idle|preheat|run|hold|cooldown|stopped",
  "lockout": false,
  "safe": false,
  "profile": {
    "name":"PLA_55C","isDry":true,"targetT_c":55,"uv_pct":0,"fan_int":40,"fan_exh":20,"duration_min":240
  },
  "actuators": {
    "heater_pct":35,"uv_pct":0,
    "fan":{"int_pct":40,"exh_pct":20,"int_rpm":1180,"exh_rpm":980},
    "motor_rpm":5
  },
  "sensors": {
    "t_air_c":43.2,"t_exh_c":38.5,"t_ptc_c":88.1,"rh_air_pct":12.4,"rh_exh_pct":14.0
  },
  "err":""
}
```

### 5.3 Commands (`.../cmd`)
```
{"action":"start","mode":"dry|cure","profile":"PLA_55C"}
{"action":"stop"}
{"set":{"uv":30,"fan_int":50,"fan_exh":20}}
{"lockout":true}
{"ota":{"url":"https://host/fw/cam.bin","sha256":"<hex>"}}
```
Правила: `set` — немедленная установка (с проверками SAFE/LOCKOUT/границ). Ответ отражается в `.../state`.

### 5.4 Telemetry (`.../tele/now`)
Подмножество `state` (температуры/влажности/RPM/проценты), неретейн.

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
- DRY UV‑LOCK: прошивка игнорирует любые UV‑команды, если `profile.isDry==true`.
- Все проценты ограничивать 0..100 на устройстве.
- Любые команды меняют `.../state` и публикуются не реже 500 ms (debounce).
- При OTA — немедленный STOP и блокировка UI до завершения.

---

## 10. Document Relationships
- Этот MASTER SPEC **заменяет** противоречащие разделы в:
  - `postcure_camera_control_logic_MQTT_HA.md` (§5, §8, §11, OTA/Recovery уточнения),
  - `postcure_camera_UI_Spec_v0.2_*.md` (§18, §20 — считать синхронизированными).  
- `README_PostCure_Camera.md` — держит Roadmap и ссылки; не источник требований.
