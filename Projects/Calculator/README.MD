# Calculator — AVR Mini Project

A basic calculator running on ATmega32, using the keypad for input and the HD44780 LCD (via I2C) for display.

---

## Features

- Supports `+`, `-`, `x`, `/` operators
- Handles chained expressions (e.g. `12+3x4`)
- Backspace key to correct input before submitting
- Result displayed right-aligned on the second row
- Resets automatically on any key press after a result

> **Note:** Operations are evaluated left to right with no operator precedence.
> `2+3x4` evaluates as `(2+3)x4 = 20`, not `2+(3x4) = 14`.

---

## Demo

[https://github.com/user-attachments/assets/e79fa459-f2bd-45b3-a9d6-1ee34b430070]

---

## Hardware

| Component | Details |
|-----------|---------|
| MCU | ATmega32 |
| Display | 16×2 LCD via PCF8574 I2C expander (`0x27`) |
| Input | 4×4 matrix keypad on Port A |

---

## File Structure

```
calculator/
├── main.c      # Calculator logic
├── lcd.h       # LCD driver interface (from LCD/)
├── lcd.c       # LCD driver implementation
├── keypad.h    # Keypad driver interface (from Keypad/)
└── keypad.c    # Keypad driver implementation
```

---

## Dependencies

This project uses both drivers from this repo:
- [`LCD/`](../../LCD/)
- [`Keypad/`](../../Keypad/)

Copy `lcd.c`, `lcd.h`, `keypad.c`, and `keypad.h` into this folder before building.

---

## How It Works

1. Keypresses are captured and echoed to the LCD with a blinking cursor
2. On `=`, the expression is parsed into numbers and operators
3. Operations are evaluated left to right
4. The result is displayed on row 2, right-aligned using `%g` formatting
5. Any key press after the result resets the display for a new expression

---

## Requirements

- AVR-GCC toolchain
- ATmega32
- PCF8574 I2C expander with 4.7kΩ pull-up resistors on SDA and SCL

---

## License

MIT
