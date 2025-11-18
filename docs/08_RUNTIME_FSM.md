# Runtime FSM — состояния и поведение

## Состояния
- IDLE
- CURE
- DRY
- PURGE
- FINISH
- ERROR

## Переходы
IDLE  → CURE / DRY / PURGE / ERROR  
CURE  → FINISH / PURGE / ERROR / IDLE  
DRY   → FINISH / PURGE / ERROR / IDLE  
PURGE → IDLE / ERROR  
FINISH → IDLE / PURGE  
ERROR → IDLE / PURGE  

## Правила
- Управление только через UI.
- MQTT публикует только:
  - temp_main
  - heater_pwm
  - heater_corr
  - state
- state публикуется только при изменении.
- Safety имеет приоритет над FSM.
- Power loss → IDLE + предложение «продолжить».
