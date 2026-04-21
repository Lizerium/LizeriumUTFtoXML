<p align="center">✨Dvurechensky✨</p>

<h1 align="center">🐟 UTF to XML Converter 🐟</h1>

<p align="center">
    <img src="https://shields.dvurechensky.pro/badge/Tool-UTFXML-blue">
    <img src="https://shields.dvurechensky.pro/badge/Freelancer-2003-green">
    <img src="https://shields.dvurechensky.pro/badge/C%2B%2B-Visual%20Studio%202008-purple">
    <img src="https://shields.dvurechensky.pro/badge/Format-UTF%20%2F%20CMP%20%2F%203DB-orange">
</p>

<p align="center">
    <img src="MEDIA/what.png">
</p>

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Язык: </strong>
  
  <span style="color: #F5F752; margin: 0 10px;">
    ✅ 🇷🇺 Русский (текущий)
  </span>
  | 
  <a href="./README.md" style="color: #0891b2; margin: 0 10px;">
    🇺🇸 English
  </a>
</div>

---

> [!NOTE]
> Этот проект является частью экосистемы **Lizerium** и относится к направлению:
>
> - [`Lizerium.Tools.Structs`](https://github.com/Lizerium/Lizerium.Tools.Structs)
>
> Если вы ищете связанные инженерные и вспомогательные инструменты — начните отсюда.

## Благодарности

> [!NOTE]
> Этот проект основан на работе сообщества Freelancer.  
> Переработан и интегрирован в экосистему Lizerium.
>
> Основано на работе [`Jason Hood (adoxa)`](https://adoxa.altervista.org/freelancer/index.html)

---

## Важно

> [!IMPORTANT]
> Я **полностью осознаю**, что оригинальный инструмент был создан около 15 лет назад (примерно в 2010 году).  
> Автор — **Jason Hood (Adoxa)**, а первоначальная реализация была сделана **Sir Lancelot**.

---

## Моя версия

> [!TIP]
> Это **кастомная переработанная версия**, созданная под мои собственные задачи.

- Добавлена поддержка экспорта блока `Animation` → `XML`
- Проверено на `.cmp` файлах
- Остальные форматы **потенциально поддерживаются**, но протестированы не полностью

---

## Назначение проекта

> [!NOTE]
> Этот инструмент был создан для проекта:
> [`Lizerium.DataValidation.Framework`](https://github.com/Lizerium/Lizerium.DataValidation.Framework)

Цели:

- Полная валидация игровых ресурсов
- Обнаружение **всех возможных ошибок**
- Совместимость со **всеми вариациями модов** для `Freelancer 2003`

---

## Описание

`UTFXML.exe` — инструмент для:

- распаковки `.cmp`, `.3db`, `.mat`, `.txm`
- конвертации в `XML`
- извлечения встроенных ресурсов (`.wav`, `.tga`, `.dds` и др.)

---

## Возможности

| Возможность      | Описание                                                            |
| ---------------- | ------------------------------------------------------------------- |
| Извлечение       | Извлекает структуру `UTF` файла в `XML`, включая встроенные ресурсы |
| Редактирование   | Полный контроль через любой текстовый редактор                      |
| Сборка обратно   | Сборка XML обратно в `.cmp`, `.mat`, `.txm`                         |
| Именованные Hash | Поддержка `hash="gcs_refer_fc_new_short"` → автоопределение ID      |
| Преобразования   | RGB → HEX, кватернионы → axis-angle, радианы → градусы              |
| Пакетный режим   | Обработка нескольких файлов из директории                           |

---

## Заметки

> [!TIP]
> Может использоваться как **анализатор структуры моделей**, а не только как конвертер.

> [!WARNING]
> Некоторые форматы **ещё не полностью протестированы**.

---

## Сборка

> [!IMPORTANT]
> Используется старый toolchain для максимальной совместимости.

- **IDE:** Visual Studio `2008`
- Конфигурация: `Release`

Шаги:

1. Открыть проект
2. Собрать
3. Готово

---

## Связанные направления

Этот слой связан с:

- [`Lizerium.DataValidation.Framework`](https://github.com/Lizerium/Lizerium.DataValidation.Framework)
