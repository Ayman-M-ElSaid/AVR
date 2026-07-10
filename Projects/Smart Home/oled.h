
/**
 * @file  oled.h
 * @brief SSD1306 128×64 I2C OLED driver — bufferless, direct-write.
 *
 * ── HARDWARE OVERVIEW ────────────────────────────────────────────────────────
 *
 *  Controller : SSD1306
 *  Interface  : I2C (TWI on AVR)
 *  Resolution : 128 columns × 64 rows
 *  I2C address: 0x3C (SA0 pin = GND) or 0x3D (SA0 pin = VCC)
 *
 *  ATmega32 pins used:
 *    PC0 → SCL   (I2C clock, hardware TWI)
 *    PC1 → SDA   (I2C data,  hardware TWI)
 *
 *  Both lines need pull-up resistors to VCC (typically 4.7 kΩ).
 *  The SSD1306 module usually has them on-board; check your specific module.
 *
 * ── DISPLAY MEMORY MODEL (GDDRAM) ───────────────────────────────────────────
 *
 *  The SSD1306 organises its 1024-byte Graphics Display Data RAM (GDDRAM)
 *  into 8 horizontal "pages". Each page is 8 pixel rows tall:
 *
 *    Page 0 → pixel rows  0– 7
 *    Page 1 → pixel rows  8–15
 *    ...
 *    Page 7 → pixel rows 56–63
 *
 *  Within each page there are 128 column bytes (one per column).
 *  Each byte is a vertical 8-pixel slice: bit 0 = topmost pixel of the page,
 *  bit 7 = bottommost pixel of the page.
 *
 *  Visual layout (one column byte inside a page):
 *
 *    bit 0 ──► pixel row (page_n × 8) + 0   ← top of page
 *    bit 1 ──► pixel row (page_n × 8) + 1
 *    ...
 *    bit 7 ──► pixel row (page_n × 8) + 7   ← bottom of page
 *
 * ── WHY NO RAM BUFFER ────────────────────────────────────────────────────────
 *
 *  A full frame-buffer would cost 128 × 8 = 1024 bytes — half of the
 *  ATmega32's 2 KB SRAM.  For this quiz application that is unnecessary
 *  because:
 *
 *    1. The question and answer text is static once drawn per question.
 *    2. The only dynamic element is the ">" cursor, which occupies exactly
 *       one known column byte on a known page; it can be erased and redrawn
 *       without reading back anything from the display.
 *    3. The SSD1306 holds its own GDDRAM between writes, so the display
 *       remembers its contents without any MCU-side copy.
 *
 *  The OLED struct therefore stores only 3 bytes of state.
 *
 * ── ADDRESSING MODE ──────────────────────────────────────────────────────────
 *
 *  This driver uses PAGE addressing mode (command 0x20, value 0x02).
 *  In this mode you set a page (0–7) and a starting column (0–127) once,
 *  then every data byte you send fills the next column in that page
 *  automatically. The column pointer wraps at 127 back to 0; the page does
 *  NOT advance automatically — you must set it explicitly for the next line.
 *
 *  This is the most efficient mode for text: set page + column once per
 *  string, then stream all glyph bytes without re-issuing address commands
 *  between columns.
 *
 * ── FONT ─────────────────────────────────────────────────────────────────────
 *
 *  Built-in 5×7 pixel font stored in Flash (PROGMEM) — costs 0 RAM.
 *  Each character is 5 column bytes wide + 1 blank spacing column = 6 pixels.
 *  Maximum characters per line: floor(128 / 6) = 21 characters.
 *  Supported range: printable ASCII 0x20 (' ') through 0x7E ('~').
 *
 * ── BITMAPS ──────────────────────────────────────────────────────────────────
 *
 *  OLED_DrawBitmap() streams a full-screen 1024-byte bitmap directly from
 *  Flash to GDDRAM — no RAM copy is ever made.
 *
 *  Declare your bitmap like this:
 *    static const uint8_t my_bmp[OLED_PAGES][OLED_WIDTH] PROGMEM = { ... };
 *
 *  Layout must match GDDRAM exactly: 8 pages × 128 columns, each byte is a
 *  vertical 8-pixel slice, bit 0 = top pixel of the page.
 *
 *  Use image2cpp (https://javl.github.io/image2cpp/) to convert images:
 *    · Canvas size   : 128 × 64
 *    · Code output   : plain bytes
 *    · Draw mode     : Horizontal, 1 bit per pixel
 *
 * ── USAGE EXAMPLE ────────────────────────────────────────────────────────────
 *
 *  #define F_CPU 1000000UL
 *  #include "oled.h"
 *
 *  OLED oled;
 *  OLED_Init(&oled, 0x3C);
 *
 *  // Draw question on pages 0–1
 *  OLED_WriteString(&oled, 0, 0, "What is 1 + 1?");
 *
 *  // Draw four answers on pages 2–5, starting at col 6 to leave room for ">"
 *  OLED_WriteString(&oled, 2, 6, "1");
 *  OLED_WriteString(&oled, 3, 6, "2");
 *  OLED_WriteString(&oled, 4, 6, "3");
 *  OLED_WriteString(&oled, 5, 6, "4");
 *
 *  // Show cursor on first answer
 *  OLED_MoveCursor(&oled, 2);
 *
 *  // User presses DOWN — cursor moves to next answer
 *  OLED_MoveCursor(&oled, 3);
 */

#ifndef OLED_H
#define OLED_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define OLED_WIDTH    128u
#define OLED_HEIGHT    64u
#define OLED_PAGES      8u

typedef struct
{
    uint8_t address;     
} OLED;

void oled_init(OLED *oled, uint8_t address);
void oled_command(OLED *oled, uint8_t cmd);
void oled_write_char(OLED *oled, uint8_t page, uint8_t col, char c);
void oled_write_string(OLED *oled, uint8_t page, uint8_t col, const char *string);
void oled_clear(OLED *oled);
void oled_clear_page(OLED *oled, uint8_t page);
void oled_draw_bitmap(OLED *oled, uint8_t page, uint8_t col, uint8_t width, uint8_t height_pages, const uint8_t *bitmap);

#endif 