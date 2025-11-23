    # Post‑Cure Camera — Bill of Materials (BOM)

    Исходный BOM сохранён без изменений.

    (source: docs/postcure_camera_BOM_20251026_222103.md)

    # Post‑Cure / Filament Dryer Camera – BOM

_Generated: 2025-10-26 22:21:03.596158_

| Status   | Category   | Component              | Model                      | Specs                             | Voltage           | Control/Interface      | Qty       | Notes                           |
|:---------|:-----------|:-----------------------|:---------------------------|:----------------------------------|:------------------|:-----------------------|:----------|:--------------------------------|
| HAVE     | Enclosure  | Chamber body           | Microwave oven body        | ~20 L reflective chamber          |                   |                        |           |                                 |
| HAVE     | Safety     | Door switches (limit)  | Microswitches              | 1–2 pcs                           | —                 | NC/NO                  | 1–2       | Interlock UV/motor              |
| HAVE     | UV light   | UV LEDs                | 3 W on star PCBs           | 395–400 nm; If=200 mA (soft mode) | 2–3 V/LED         | Series string (×20)    | 20        | Two packs of 10                 |
| HAVE     | UV driver  | LED driver (soft)      | 75 V / 200 mA + IR remote  | All 20 LEDs in series             | AC 220 V in       | On/Off via SSR         | 1         | For gentle cure                 |
| HAVE     | Heater     | PTC heater             | 24 V PTC module            | ≈200 W peak, self‑regulating      | 24 V DC           | PWM via SSR‑40DD       | 1         | Main chamber heater             |
| HAVE     | Airflow    | PTC+Fan combo          | Fan + PTC module           | Circulation & heating             | 24 V DC           | To be PWM/ONOFF        | 1         | Internal air movement           |
| HAVE     | Rotation   | Turntable motor        | 49TYZ‑A2                   | 5–6 rpm, 4 W                      | AC 220–240 V      | On/Off via SSR         | 1         | For uniform exposure            |
| HAVE     | Power      | PSU 24 V               | Slim LED PSU               | 200 W (24 V × 8.3 A)              | AC 220 V in       | —                      | 1         | Feeds PTC, fans, logic via buck |
| HAVE     | Display    | OLED display           | 1.3" 128×64 I²C (SSD1306)  | White color                       | 3.3/5 V           | I²C                    | 1         | UI screen                       |
| HAVE     | Input      | Rotary encoder w/ push | —                          | Detent encoder + button           | —                 | GPIO                   | 1         | Menu navigation                 |
| HAVE     | Relay AC   | Dual AC SSR            | 2× Omron G3MB‑202P module  | Zero‑cross AC 2 A/240 V           | 5 V input         | GPIO/OC                | 1         | CH1 UV driver, CH2 motor        |
| HAVE     | Relay DC   | DC SSR for heater      | FQFER SSR‑40DD             | DC‑DC 40 A                        | 3–32 V in         | Heater PWM (low freq)  | 1         | PTC 24 V switching              |
| HAVE     | Fan PWM    | MOSFET PWM module      | AOD4184 (single‑channel)   | Up to 15 A, PWM ≤20 kHz           | 5–36 V out        | 3.3–20 V in            | 2         | For internal & exhaust fans     |
| NEED     | Controller | MCU board              | STM32 Blue Pill (F103C8)   | 3.3 V logic, multiple timers      | 5 V in            | I²C, 1‑Wire, PWM, GPIO | 1         | Main controller                 |
| NEED     | Sensors    | Temp sensors           | DS18B20                    | Air + near PTC                    | 3.3/5 V           | 1‑Wire                 | 2         | Thermal control/monitoring      |
| NEED     | Sensors    | Humidity sensor        | SHT31 (or SHT30)           | —                                 | 3.3/5 V           | I²C                    | 1         | Drying mode control             |
| NEED     | Protection | Thermostat             | KSD9700 NC 70–75°C         | Series with PTC                   | —                 | —                      | 1         | Hardware overtemp cutoff        |
| NEED     | Protection | Thermal fuse           | 72–84°C (one‑shot)         | Series with PTC                   | —                 | —                      | 1         | Secondary safety                |
| NEED     | Protection | AC inlet & fuse        | IEC C14 with switch        | AC input module                   | AC 220 V          | —                      | 1         | Mains entry                     |
| NEED     | Protection | Fuses                  | 2–4 A (AC), 10 A (24 V DC) | Inline/holder                     | —                 | —                      | 2–3       | Per lines                       |
| NEED     | Protection | Varistor (MOV)         | MOV‑14D471 (AC)            | Surge clamp                       | —                 | —                      | 1         | Across AC line                  |
| NEED     | Protection | TVS diode              | SMBJ33A (24 V bus)         | Transient clamp                   | —                 | —                      | 1–2       | Near fan modules                |
| NEED     | Power      | Buck converter         | 24→5 V, ≥3 A               | For logic & SSR inputs            | 24 V in / 5 V out | —                      | 1         | Feeds MCU, OLED, sensors        |
| NEED     | Thermal    | LED heatsink plate     | Aluminum plate 2–3 mm      | ~200–300 cm²                      | —                 | —                      | 1         | With thermal pads/adhesive      |
| NEED     | Optics     | Front cover/diffuser   | Borosilicate/quartz glass  | —                                 | —                 | —                      | 1         | Uniform light & protection      |
| NEED     | Wiring     | Connectors & wire      | XT30/XT60, screw terminals | 1.0–1.5 mm² for PTC               | —                 | —                      | as needed | Wiring & service                |

