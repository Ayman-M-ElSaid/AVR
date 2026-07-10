# AVR SSD1306 OLED Driver

A lightweight I2C driver for SSD1306-based 128×64 OLED displays on AVR microcontrollers. Supports text rendering with a built-in 5×7 font and PROGMEM bitmap drawing, addressed over the hardware TWI (I2C) peripheral.

## Files

| File | Description |
|------|-------------|
| `oled.h` | Public API, `OLED` struct, and display dimension constants |
| `oled.c` | Driver implementation — init sequence, font table, and I2C transport |

## API

### `void oled_init(OLED *oled, uint8_t address)`

Initializes the hardware TWI peripheral and the display.

| Parameter | Description |
|-----------|--------------|
| `address` | 7-bit I2C address of the module (typically `0x3C`) |

- Sets `TWBR`/`TWSR` for I2C — `32` if `F_CPU > 100 kHz`, otherwise `2`. This mirrors the LCD driver's I2C-mode setup, so both can share the same bus without conflicting clock settings. **Recalculate if you change `F_CPU`** — the value isn't derived automatically.
- Runs the SSD1306 init sequence: charge pump enabled, page-addressing mode, 128×64 multiplex, segment re-map + COM scan direction remapped (flips the image so text isn't mirrored/upside-down on most modules)
- Clears the display at the end of init

Call once before any other `oled_*()` function.

---

### `void oled_command(OLED *oled, uint8_t cmd)`

Sends a single raw command byte to the controller (control byte `0x00`). Used internally, and available for custom commands such as contrast (`0x81`) or display on/off (`0xAE`/`0xAF`).

---

### `void oled_write_char(OLED *oled, uint8_t page, uint8_t col, char c)`

Draws one ASCII character (`0x20`–`0x7E`) using the built-in 5×7 font.

| Parameter | Description |
|-----------|--------------|
| `page` | Row in 8-pixel-tall pages (0–7) |
| `col` | Starting column in pixels (0–127) |
| `c` | Character to draw |

Each glyph is 5 columns wide, followed by 1 blank spacing column (6 px total). Characters outside `0x20`–`0x7E` are not bounds-checked against the font table.

---

### `void oled_write_string(OLED *oled, uint8_t page, uint8_t col, const char *string)`

Draws a null-terminated string starting at `page`/`col`, advancing 6 px per character. No word-wrapping — text that runs past column 127 is not automatically continued on the next page.

---

### `void oled_clear(OLED *oled)`

Clears the entire display (all 8 pages).

---

### `void oled_clear_page(OLED *oled, uint8_t page)`

Clears a single page — cheaper than a full `oled_clear()` when only part of the screen needs refreshing. `page >= OLED_PAGES` is checked and ignored.

---

### `void oled_draw_bitmap(OLED *oled, uint8_t page, uint8_t col, uint8_t width, uint8_t height, const uint8_t *bitmap)`

Draws a PROGMEM bitmap.

| Parameter | Description |
|-----------|--------------|
| `page` | Starting page (0–7) |
| `col` | Starting column in pixels |
| `width` | Bitmap width in pixels |
| `height` | Bitmap height in pixels — **must be a multiple of 8** (internally divided by 8 to get page count) |
| `bitmap` | `PROGMEM` byte array in row-major, horizontal format: `bitmap[page_row * width + col]` — matches [image2cpp](https://javl.github.io/image2cpp/)'s default horizontal output |

Re-asserts page-addressing mode (`0x20`, `0x02`) before drawing, in case addressing mode was changed elsewhere.

---

## Usage Example

```c
#include "oled.h"

int main(void)
{
    OLED oled;
    oled_init(&oled, 0x3C);

    oled_write_string(&oled, 0, 0, "Hello, OLED!");

    while (1);
}
```

## Hardware Notes

- Uses the ATmega32's hardware **TWI peripheral** — same SDA/SCL pins (PC1/PC0) as any other I2C device, so it can share the bus with the LCD driver's I2C mode (PCF8574) as long as addresses differ
- `OLED_WIDTH` (128), `OLED_HEIGHT` (64), and `OLED_PAGES` (8) in `oled.h` assume a 128×64 module — a 128×32 module would need `OLED_PAGES` and the init sequence's multiplex ratio (`0xA8`) adjusted
- If text/graphics appear mirrored or upside-down on your specific module, flip the `0xA1` (segment re-map) / `0xC8` (COM scan direction) bytes in `OLED_INIT_SEQ`
- **Simulation note:** OLED rendering in Proteus can overload at full MCU speed — verify bitmap/text output on real hardware

## Dependencies

- `avr/io.h` (avr-libc)
- `avr/pgmspace.h` (avr-libc) — font table and init sequence are stored in flash, not RAM
- `util/delay.h` (avr-libc)

## License

MIT
