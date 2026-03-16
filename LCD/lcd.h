#ifndef LCD_H_
#define LCD_H_

#include <avr\io.h>
#include <avr\delay.h>

typedef struct
{
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t E;
    uint8_t RS;

} LCD;

void lcd_init_4bit(LCD *lcd, char port_name);
void lcd_command(LCD *lcd, unsigned char cmd);
void lcd_data(LCD *lcd,unsigned char data);
void lcd_clear(LCD *lcd);
void lcd_print(LCD *lcd,char *string);
void lcd_goto(LCD *lcd, uint8_t row, uint8_t col);

#endif
