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
