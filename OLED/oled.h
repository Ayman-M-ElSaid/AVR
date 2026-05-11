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
