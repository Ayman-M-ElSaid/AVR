# AVR ADC Driver

A lightweight ADC driver for AVR microcontrollers (ATmega series). Provides simple blocking reads from any ADC channel with AVcc as the reference voltage and optional runtime prescaler control.

## Files

| File | Description |
|------|-------------|
| `adc.h` | Public API — include this in your project |
| `adc.c` | Driver implementation |

## API

### `void adc_init()`

Initializes the ADC peripheral.

- Reference voltage: **AVcc** (REFS0 = 1)
- Default prescaler: **8** (ADPS0 + ADPS1) — suited for 1 MHz clock (ADC clock = 125 kHz, within the recommended 50–200 kHz range)
- Enables the ADC (ADEN = 1)
- Performs a dummy conversion to warm up the sample-and-hold capacitor, ensuring the first real read is accurate

Call once before any `adc_read()` or `set_prescaler()` calls.

---

### `void adc_set_prescaler(uint8_t prescaler)`

Changes the ADC clock prescaler at runtime. Call after `adc_init()` when running at a clock speed other than 1 MHz.

| `prescaler` | ADC clock @ 1 MHz | ADC clock @ 8 MHz | ADC clock @ 16 MHz |
|-------------|-------------------|-------------------|--------------------|
| 2           | 500 kHz ⚠️        | —                 | —                  |
| 4           | 250 kHz ⚠️        | 2 MHz ⚠️          | —                  |
| 8           | **125 kHz** ✅    | 1 MHz ⚠️          | —                  |
| 16          | —                 | **500 kHz** ✅    | 1 MHz ⚠️           |
| 32          | —                 | **250 kHz** ✅    | 500 kHz ✅         |
| 64          | —                 | **125 kHz** ✅    | 250 kHz ✅         |
| 128         | —                 | —                 | **125 kHz** ✅     |

> ✅ Within recommended range (50–200 kHz for full 10-bit accuracy). ⚠️ Out of range — reduced accuracy.

Invalid prescaler values (anything not in the table) are silently ignored.

---

### `uint16_t adc_read(uint8_t pin)`

Reads a single 10-bit ADC conversion from the specified channel.

| Parameter | Description |
|-----------|-------------|
| `pin` | ADC channel number (0–7 for ATmega32) |

**Returns:** Raw 10-bit result (0–1023).

- Configures only the selected pin as input (`DDRA &= ~(1 << pin)`), leaving other PORTA pins untouched
- Blocks until the conversion is complete (polling ADSC)

---

## Usage Example

The following displays a live voltage reading (e.g. `2.347V`) on a 16×2 I2C LCD using ADC channel 0.

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
    // adc_set_prescaler(64);   // uncomment if running at 8 MHz

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

## Hardware Notes

- Tested on **ATmega32** @ 1 MHz internal RC oscillator
- ADC input pins are **PA0–PA7**
- Ensure **AVCC is connected to VCC** with a 100 nF bypass capacitor to GND — a floating AVCC produces unstable results

## Dependencies

- `avr/io.h` (avr-libc)
- `stdbool.h`
