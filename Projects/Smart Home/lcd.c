#include "lcd.h"

static void set(LCD *lcd, uint8_t pin);
static void clear(LCD *lcd, uint8_t pin);
static void pulse(LCD *lcd, uint8_t pin);
static void lcd_I2C_start(LCD *lcd);
static void lcd_I2C_stop();

/************************** 4-bit Mode **************************/
void lcd_init_4bit(LCD *lcd, char port_name)
{
    lcd->MODE = 4;

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

    lcd_write(lcd, false, 0x33);
    lcd_write(lcd, false, 0x32);
    lcd_write(lcd, false, 0x28);
    lcd_write(lcd, false, 0x08);
    lcd_clear(lcd);
    lcd_write(lcd, false, 0x06);
    lcd_write(lcd, false, 0x0C);
}

void lcd_write(LCD *lcd, bool is_data, uint8_t byte)
{
    if (is_data)
        set(lcd, lcd->RS);
    else
        clear(lcd, lcd->RS);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (byte & 0xF0);
    pulse(lcd, lcd->E);

    *(lcd->port) = (*(lcd->port) & 0x0F) | (byte << 4);
    pulse(lcd, lcd->E);

    _delay_ms(1);
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

/***************************** I2C *****************************/
void lcd_init_I2C(LCD *lcd, uint8_t address)
{
    lcd->MODE = 2;
    _delay_ms(50);

    TWSR = 0x00;
    TWBR = F_CPU > 100000UL ? 32 : 2;
    TWCR = 1 << TWEN;
    lcd->I2C_address = address;

    lcd_I2C_write(lcd, false, 0x33);
    lcd_I2C_write(lcd, false, 0x32);
    lcd_I2C_write(lcd, false, 0x28);
    lcd_I2C_write(lcd, false, 0x08);
    lcd_clear(lcd);
    lcd_I2C_write(lcd, false, 0x06);
    lcd_I2C_write(lcd, false, 0x0C);
}

static void I2C_start(uint8_t address)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    TWDR = address << 1;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    if ((TWSR & 0xF8) != 0x18)
        return;
}

static void I2C_stop()
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
    _delay_us(10);
}

void lcd_I2C_write(LCD *lcd, bool is_data, uint8_t byte)
{
    I2C_start(lcd->I2C_address);
    uint8_t data = 0;

    data |= (byte & 0xF0);
    data |= is_data;
    data |= (1 << 3);
    TWDR = data | (1 << 2);
    _delay_us(1);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    TWDR = data & ~(1 << 2);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    data = (data & 0x0F) | ((byte & 0x0F) << 4);
    TWDR = data | (1 << 2);
    _delay_us(1);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    TWDR = data & ~(1 << 2);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;

    I2C_stop();
    _delay_ms(1);
}

/***************************** APIs *****************************/

void lcd_clear(LCD *lcd)
{
    if (lcd->MODE == 4)
        lcd_write(lcd, false, 0x01);
    else if (lcd->MODE == 2)
        lcd_I2C_write(lcd, false, 0x01);

    _delay_ms(2);
}

void lcd_print(LCD *lcd, char *string)
{
    while (*string)
    {
        if (lcd->MODE == 4)
            lcd_write(lcd, true, *string++);
        else if (lcd->MODE == 2)
            lcd_I2C_write(lcd, true, *string++);
    }
}

void lcd_goto(LCD *lcd, uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? 0x00 + col : 0x40 + col;
    if (lcd->MODE == 4)
        lcd_write(lcd, false, 0x80 | address);
    else if (lcd->MODE == 2)
        lcd_I2C_write(lcd, false, 0x80 | address);
}
