# HD44780 LCD Driver — AVR (4-bit mode)

A lightweight, struct-based HD44780 LCD driver for AVR microcontrollers, written in C.  
The entire port configuration is resolved at runtime via a single `char` port name — no hardcoded registers in your application code.

---

## Demo

https://github.com/Ayman-M-ElSaid/LCD/raw/main/Simulation.mp4

> Simulated in Proteus on an ATmega32. The demo scrolls **"Eid Mubarak!"** across both rows of a 16×2 LCD.

---

## Features

- 4-bit interface mode — only 4 data lines required
- Single-call initialization: just pass the port name (`'A'`–`'D'`)
- Struct-based design — all port/pin state lives in the `LCD` struct
- Full upper nibble + lower nibble transfer with enable pulse
- Cursor positioning by row and column

---

## File Structure

```
LCD/
├── lcd.h               # Driver interface & LCD struct definition
├── lcd.c               # Driver implementation
├── main.c              # Demo / usage example
├── Simulation.pdsprj   # Proteus simulation project
└── Simulation.mp4      # Simulation output video
```

---

## Pin Mapping

The driver assumes **a full port** is dedicated to the LCD, wired as follows:

| Port Pin | LCD Pin         |
|----------|-----------------|
| Pin 2    | E (Enable)      |
| Pin 3    | RS (Reg. Select)|
| Pin 4–7  | D4–D7 (data bus)|

> Pins 0 and 1 are unused by the driver but are driven as outputs since the full DDR is set to `0xFF`.

---

## API

```c
// Initialize the LCD on the given AVR port ('A', 'B', 'C', or 'D')
void lcd_init_4bit(LCD *lcd, char port_name);

// Send a command byte to the LCD
void lcd_command(LCD *lcd, unsigned char cmd);

// Send a single data byte (character) to the LCD
void lcd_data(LCD *lcd, unsigned char data);

// Clear the display (also delays 2ms for the LCD to settle)
void lcd_clear(LCD *lcd);

// Print a null-terminated string at the current cursor position
void lcd_print(LCD *lcd, char *string);

// Move the cursor to a specific row (0 or 1) and column (0–15)
void lcd_goto(LCD *lcd, uint8_t row, uint8_t col);
```

---

## Usage Example

```c
#include "lcd.h"

int main(void)
{
    LCD lcd;
    lcd_init_4bit(&lcd, 'A');   // Use PORTA

    lcd_goto(&lcd, 0, 0);
    lcd_print(&lcd, "Hello,");
    lcd_goto(&lcd, 1, 0);
    lcd_print(&lcd, "World!");

    while (1);
    return 0;
}
```

---

## How It Works

### Initialization

`lcd_init_4bit()` resolves the port and DDR pointers from the `port_name` argument, sets the full port as output, then runs the standard HD44780 4-bit initialization sequence:

```
0x33 → 0x32  (force 4-bit mode)
0x28         (2-line, 5×8 font)
0x08         (display off)
0x01         (clear display)
0x06         (entry mode: increment cursor)
0x0C         (display on, cursor off)
```

### Data Transfer

Each byte is sent in two nibbles — upper first, then lower — each followed by an enable pulse:

```c
*(lcd->port) = (*(lcd->port) & 0x0F) | (cmd & 0xF0);  // upper nibble
pulse(lcd, lcd->E);

*(lcd->port) = (*(lcd->port) & 0x0F) | (cmd << 4);    // lower nibble
pulse(lcd, lcd->E);
```

The lower 4 bits of the port (including E and RS on pins 2–3) are preserved with the `0x0F` mask.

---

## Requirements

- AVR-GCC toolchain
- AVR microcontroller (developed and tested on ATmega32)
- Proteus 8+ (optional, for simulation)

---

## Roadmap

- [x] 4-bit mode
- [ ] 8-bit mode
- [ ] Custom character support (CGRAM)
- [ ] I2C adapter support (PCF8574)

---

## License

MIT

