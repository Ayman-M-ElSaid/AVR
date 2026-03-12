#include "lcd.h"

void lcd_init_4bit(LCD *lcd, char port_name)
{
    switch (port_name)
    {
    case 'A':
        lcd->port = &PORTA;
        lcd->ddr = &DDRA;
        break;
    case 'B':
        lcd->port = &PORTB;
        lcd->ddr = &DDRB;
        break;
    case 'C':
        lcd->port = &PORTC;
        lcd->ddr = &DDRC;
        break;
    case 'D':
        lcd->port = &PORTD;
        lcd->ddr = &DDRD;
        break;
    default:
        return;
    }

    *(lcd->ddr) = 0xFF;
    lcd->E = 2;
    lcd->RS = 3;
    _delay_ms(30);
    // Function Set
    lcd_command(lcd, 0x20);
    lcd_command(lcd, 0x20);
    lcd_command(lcd, 0x28);
    // Display
    lcd_command(lcd, 0x0C);
    lcd_clear(lcd);
    // Entry Mode Set
    lcd_command(lcd, 0x06);
}

void lcd_command(LCD *lcd, unsigned char cmd)
{
    clear(lcd, lcd->RS);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (cmd & 0xF0);
    pulse(lcd, lcd->E);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (cmd << 4);
    pulse(lcd, lcd->E);

    _delay_ms(5);
}

void lcd_data(LCD *lcd, unsigned char data)
{
    set(lcd, lcd->RS);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (data & 0xF0);
    pulse(lcd, lcd->E);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (data << 4);
    pulse(lcd, lcd->E);

    _delay_ms(5);
}

void lcd_clear(LCD *lcd)
{
    lcd_command(lcd, 0x01);
    _delay_ms(2);
}

void lcd_print(LCD *lcd, char *string)
{
    while (*string)
    {
        lcd_data(lcd, *string++);
    }
}

void lcd_goto(LCD *lcd, uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? 0x00 + col : 0x40 + col;
    lcd_command(lcd, 0x80 | address);
}

void set(LCD *lcd, uint8_t pin)
{
    *(lcd->port) |= (1 << pin);
}

void clear(LCD *lcd, uint8_t pin)
{
    *(lcd->port) &= ~(1 << pin);
}

void pulse(LCD *lcd, uint8_t pin)
{
    set(lcd, pin);
    _delay_ms(5);
    clear(lcd, pin);
    _delay_ms(5);
}