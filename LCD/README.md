# HD44780 LCD Driver — AVR

A lightweight, struct-based HD44780 LCD driver for AVR microcontrollers, written in C.
Supports both **4-bit parallel** and **I2C** (via PCF8574) interfaces through a unified API —
the same `lcd_print()`, `lcd_goto()`, and `lcd_clear()` calls work regardless of which mode you use.

---

## Demo

### 4-bit Mode — Scrolling Text
[https://github.com/Ayman-M-ElSaid/LCD/raw/main/Simulation.mp4](https://github.com/user-attachments/assets/4f25705f-32dc-4303-a0b9-793ef32a9b25)
> ATmega32 driving a 16×2 LCD in 4-bit parallel mode. Scrolls "Eid Mubarak!" across both rows.

### I2C Mode — PCF8574 Schematic
<img width="1573" height="1436" alt="Image" src="https://github.com/user-attachments/assets/204cc496-e3d4-47a0-b25d-cddeb9ebb7d2" />
> ATmega32 driving a 16×2 LCD through a PCF8574 I2C I/O expander.
> Pull-up resistors (4.7kΩ) on SDA and SCL. A0–A2 tied to VCC sets the PCF8574 address to `0x27`.

---

## Features

- **4-bit parallel mode** — uses a full AVR port, no external hardware
- **I2C mode** — drives the LCD via a PCF8574 expander over just 2 wires (SDA + SCL)
- **Unified API** — `lcd_print()`, `lcd_goto()`, `lcd_clear()` work for both modes
- Single-call initialization for both modes
- Struct-based design — all state lives in the `LCD` struct, no globals
- Multiple I2C LCDs supported simultaneously (each with a unique PCF8574 address)

---

## File Structure

```
LCD/
├── lcd.h               # Driver interface & LCD struct
├── lcd.c               # Driver implementation (4-bit + I2C)
└── main.c              # Demo / usage example
```

---

## 4-bit Mode

### Pin Mapping

The driver assumes a **full AVR port** is dedicated to the LCD:

| Port Pin | LCD Signal        |
|----------|-------------------|
| Pin 2    | E (Enable)        |
| Pin 3    | RS (Reg. Select)  |
| Pin 4–7  | D4–D7 (data bus)  |
| Pin 0–1  | Unused (output)   |

### Initialization

```c
LCD lcd;
lcd_init_4bit(&lcd, 'A');   // 'A', 'B', 'C', or 'D'
```

---

## I2C Mode (PCF8574)

Drives the LCD over I2C using a PCF8574 I/O expander — only 2 wires needed (SDA + SCL).

### Wiring

- Connect **SDA** → PC1, **SCL** → PC0 on the ATmega32
- Add **4.7kΩ pull-up resistors** on both SDA and SCL to VCC
- The I2C address depends on A0–A2 pin states on the PCF8574:

| A2 | A1 | A0 | Address |
|----|----|----|---------|
| 0  | 0  | 0  | `0x20`  |
| 0  | 0  | 1  | `0x21`  |
| 1  | 1  | 1  | `0x27`  |

### Initialization

```c
LCD lcd;
lcd_init_I2C(&lcd, 0x27);   // pass the PCF8574 I2C address
```

> Multiple I2C LCDs can be used simultaneously by declaring separate `LCD` structs, each initialized with a different address.

---

## Unified API

Once initialized, all three functions work identically for both modes:

```c
// Clear the display
void lcd_clear(LCD *lcd);

// Print a null-terminated string at the current cursor position
void lcd_print(LCD *lcd, char *string);

// Move cursor to row (0–1) and column (0–15)
void lcd_goto(LCD *lcd, uint8_t row, uint8_t col);
```

### Low-level API

```c
// 4-bit: send a command or data byte
void lcd_write(LCD *lcd, bool is_data, uint8_t byte);

// I2C: send a command or data byte over I2C
void lcd_I2C_write(LCD *lcd, bool is_data, uint8_t byte);
```

---

## Usage Example

```c
#include "lcd.h"

int main(void)
{
    LCD lcd;
    lcd_init_I2C(&lcd, 0x27);   // or lcd_init_4bit(&lcd, 'A')

    lcd_goto(&lcd, 0, 0);
    lcd_print(&lcd, "Hello,");
    lcd_goto(&lcd, 1, 0);
    lcd_print(&lcd, "World!");

    while (1);
    return 0;
}
```

---

## Initialization Sequence

Both modes run the same standard HD44780 startup sequence:

| Command          | Description                       |
|------------------|-----------------------------------|
| `0x33` → `0x32`  | Force 4-bit mode                  |
| `0x28`           | 2-line display, 5×8 font          |
| `0x08`           | Display off                       |
| `0x01`           | Clear display                     |
| `0x06`           | Entry mode: increment, no shift   |
| `0x0C`           | Display on, cursor off, blink off |

---

## Requirements

- AVR-GCC toolchain
- ATmega32 (or compatible AVR)
- PCF8574 I/O expander *(I2C mode only)*
- Proteus 8+ *(optional, for simulation)*

---

## Roadmap

- [x] 4-bit parallel mode
- [x] I2C mode (PCF8574)
- [ ] 8-bit parallel mode
- [ ] Custom character support (CGRAM)

---

## License

MIT
