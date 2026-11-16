# iptables UI demo

Минимальное desktop-приложение на Qt 6 Widgets для просмотра правил iptables.
Сейчас таблица заполняется демонстрационными данными и не изменяет системный firewall.

## Сборка

```bash
cmake -S . -B build
cmake --build build
./build/iptables-ui
```

В интерфейсе доступны список правил, фильтр по цепочкам и кнопка обновления.

