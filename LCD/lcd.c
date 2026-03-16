#include "lcd.h"

static void set(LCD *lcd, uint8_t pin);
static void clear(LCD *lcd, uint8_t pin);
static void pulse(LCD *lcd, uint8_t pin);

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
    _delay_ms(50);

    lcd_command(lcd, 0x33);
    lcd_command(lcd, 0x32);
    lcd_command(lcd, 0x28);
    lcd_command(lcd, 0x08);
    lcd_clear(lcd);
    lcd_command(lcd, 0x06);
    lcd_command(lcd, 0x0C);
}

void lcd_command(LCD *lcd, unsigned char cmd)
{
    clear(lcd, lcd->RS);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (cmd & 0xF0);
    pulse(lcd, lcd->E);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (cmd << 4);
    pulse(lcd, lcd->E);

    _delay_ms(1);
}

void lcd_data(LCD *lcd, unsigned char data)
{
    set(lcd, lcd->RS);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (data & 0xF0);
    pulse(lcd, lcd->E);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (data << 4);
    pulse(lcd, lcd->E);

    _delay_ms(1);
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

static void set(LCD *lcd, uint8_t pin)
{
    *(lcd->port) |= (1 << pin);
}

static void clear(LCD *lcd, uint8_t pin)
{
    *(lcd->port) &= ~(1 << pin);
}

static void pulse(LCD *lcd, uint8_t pin)
{
    set(lcd, pin);
    _delay_us(1);
    clear(lcd, pin);
    _delay_us(50);
}
