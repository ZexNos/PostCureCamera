# Post‑Cure Camera — UI Feature Sandbox & Modular Testing Methodology
**Updated:** 2025-11-08 16:46

---

## 🎯 Цель
Обеспечить изолированное тестирование **каждого UI‑экрана или функции** без влияния остальных модулей.  
Экран или фича считается полностью отлаженной, если она проходит песочницу и может быть подключена к UiRouter без изменений драйверов.

---

## 1️⃣ Базовые условия
- Экран ST7789 и энкодер EncButton **заморожены** и работают стабильно.
- Драйверы DisplayST7789 / EncoderDrv / Colors.h / Pins.h не изменяются.
- Песочница запускается в режиме `SCREEN_SANDBOX` (см. ниже).

---

## 2️⃣ Универсальный интерфейс экрана
Каждый экран реализует общий контракт `IScreen`:

```cpp
struct IScreen {
  virtual void begin(DisplayST7789* d, EncoderDrv* e) = 0;
  virtual void drawStatic() = 0;      // фон/рамки, один раз
  virtual void drawDynamic() = 0;     // только изменившиеся элементы
  virtual void onEvent(EncEvent ev) = 0;
  virtual void tick() = 0;            // таймеры/анимации без delay()
  virtual ~IScreen() {}
};
```
Все экраны рисуют **только CONTENT‑зону** (y=14..113). Верхняя/нижняя полосы принадлежат Display.

---

## 3️⃣ Песочница (Screen Harness)
Используется для теста отдельного экрана.

```cpp
#define SCREEN_SANDBOX 1
#define SCREEN_UNDER_TEST  WiFiSetupScreen   // ← меняем для каждой фичи

#if SCREEN_SANDBOX
static SCREEN_UNDER_TEST scr;
void App::begin(){ display.begin(); encoder.begin(); scr.begin(&display,&encoder); scr.drawStatic(); }
void App::tick(){ encoder.tick(); scr.tick(); scr.drawDynamic();
  if (auto ev = encoder.pollEvent(); ev!=EncEvent::None) scr.onEvent(ev);
}
#endif
```
Меняешь `SCREEN_UNDER_TEST` → можно тестить любой экран из `ui/screens/*` без правок роутера.

---

## 4️⃣ Инъекция состояния (моки)

Для проверки отображения без сервисов:
```cpp
AppState mock{ .wifiConnected=true, .mqttConnected=false, .heaterOn=false, .uvOn=false };
display.drawTopBar(mock);              // кэш сам решит, перерисовывать ли
```

---

## 5️⃣ Политика отрисовки
- `drawStatic()` — один раз при создании/обновлении макета.
- `drawDynamic()` — только изменившиеся данные; никаких `fillScreen()`.
- Иконки статусов — обновляются только по delta (через кэш Display).

---

## 6️⃣ Definition of Done (для экрана/фичи)
- ✅ Реализованы все методы интерфейса.
- ✅ CONTENT‑only; без прямых вызовов top/bottom‑bar.
- ✅ Нет полного клира; дифф‑отрисовка работает.
- ✅ Навигация через энкодер корректна.
- ✅ Цвета/шрифты из Colors.h и style‑гайда.
- ✅ Песочница прогнана (вращение, клики, стресс ≥60 с).

---

## 7️⃣ Интеграция в систему
После успешного теста в песочнице:
- Добавить маршрут в `UiRouter`:
  ```cpp
  case Route::WiFiSetup: current = &wifiSetupScreen; break;
  ```
- Добавить при необходимости переходы в `App`.
- Выключить песочницу (`#undef SCREEN_SANDBOX`).
- Система автоматически подхватит экран в основном цикле.

---

## 8️⃣ Преимущества методики
- Можно разрабатывать и отлаживать любой экран независимо от MQTT, Wi‑Fi и датчиков.  
- Гарантируется единообразие интерфейса.  
- После успешного теста интеграция в UiRouter/App — чисто декларативная.  
- Позволяет делать **регрессионные тесты экранов** (песочница как unit‑runner).

---

## 📘 Рекомендуемая структура
```
ui/
  screens/
    RunView.cpp
    Options.cpp
    WiFiSetup.cpp
    MqttSettings.cpp
    ...
  sandbox/
    ScreenSandbox.h/.cpp     ← шаблон песочницы
```

---

## 9️⃣ Чек‑лист перед «заморозкой» экрана
- [ ] drawStatic()/drawDynamic()/onEvent()/tick() реализованы.
- [ ] Все цвета/шрифты из Colors.h.
- [ ] Навигация и события соответствуют ТЗ.
- [ ] Прошёл тест песочницы без ошибок 60 секунд.
- [ ] Визуально нет артефактов, flicker и дублирующих draw.
- [ ] Экран интегрирован в UiRouter и App без конфликтов.

---

**После выполнения чек‑листа экран считается готовым к заморозке и не требует изменений при дальнейшем росте проекта.**
