# AVR ADC Driver

A lightweight ADC driver for AVR microcontrollers (ATmega series). Provides simple blocking reads from any ADC channel with AVcc as the reference voltage.

## Files

| File | Description |
|------|-------------|
| `adc.h` | Public API — include this in your project |
| `adc.c` | Driver implementation |

## API

### `void adc_init()`

Initializes the ADC peripheral.

- Reference voltage: **AVcc** (REFS0 = 1)
- Prescaler: **8** (ADPS0 + ADPS1) — suited for 1 MHz clock (ADC clock = 125 kHz, within the 50–200 kHz recommended range)
- Sets PORTA as input (DDRA = 0)
- Enables the ADC (ADEN = 1)

Call once before any `adc_read()` calls.

### `uint16_t adc_read(uint8_t pin)`

Reads a single 10-bit ADC conversion from the specified channel.

| Parameter | Description |
|-----------|-------------|
| `pin` | ADC channel number (0–7 for ATmega32) |

**Returns:** Raw 10-bit result (0–1023).

Blocks until the conversion is complete (polling ADSC).

## Usage Example

```c
#include "adc.h"
#include "lcd.h"

int main()
{
    LCD lcd;
    uint32_t voltage_mv;
    int integer, fraction;

    lcd_init_I2C(&lcd, 0x27);
    adc_init();
    lcd_print(&lcd, "Voltage: ");
    while (1)
    {
        voltage_mv = (uint32_t)adc_read(0) * 5000UL / 1023;
        lcd_goto(&lcd, 0, 9);
        integer = voltage_mv / 1000;
        lcd_I2C_write(&lcd, true, integer + 0x30);
        lcd_I2C_write(&lcd, true, '.');
        fraction = voltage_mv % 1000;
        lcd_I2C_write(&lcd, true, (fraction / 100) + 0x30);
        lcd_I2C_write(&lcd, true, ((fraction % 100) / 10) + 0x30);
        lcd_I2C_write(&lcd, true, (fraction % 10) + 0x30);
        lcd_I2C_write(&lcd, true, 'V');
    }
}
```

This displays a live voltage reading (e.g. `2.347V`) on a 16×2 I2C LCD using ADC channel 0.

## Hardware Notes

- Tested on **ATmega32** @ 1 MHz internal RC oscillator
- ADC input pins are **PA0–PA7**
- Connect the signal to the appropriate `PA` pin
- Ensure **AVCC is connected to VCC** (with a 100nF bypass capacitor to GND) for accurate readings; a floating AVCC produces unstable results
- The prescaler of 8 is correct for 1 MHz. If you run at a higher clock speed, increase the prescaler accordingly:

| F_CPU | Prescaler | ADPS bits |
|-------|-----------|-----------|
| 1 MHz | 8 | ADPS1, ADPS0 |
| 8 MHz | 64 | ADPS2, ADPS1 |
| 16 MHz | 128 | ADPS2, ADPS1, ADPS0 |

## Dependencies

- `avr/io.h` (avr-libc)
- No other dependencies
