#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

typedef struct
{
    uint8_t MODE;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t E;
    uint8_t RS;
    uint8_t I2C_address;
} LCD;

void lcd_init_4bit(LCD *lcd, char port_name);
void lcd_write(LCD *lcd, bool is_data, uint8_t byte);

void lcd_init_I2C(LCD *lcd, uint8_t address);
void lcd_I2C_write(LCD *lcd, bool is_data, uint8_t byte);

void lcd_clear(LCD *lcd);
void lcd_print(LCD *lcd, char *string);
void lcd_goto(LCD *lcd, uint8_t row, uint8_t col);

#endif
