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
