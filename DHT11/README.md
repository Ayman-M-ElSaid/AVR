# AVR DHT11 Driver

A bit-banged, single-wire driver for the DHT11 temperature/humidity sensor on AVR microcontrollers (ATmega series). Reads whole-number humidity and temperature values with checksum verification.

## Files

| File | Description |
|------|-------------|
| `dht11.h` | Public API — include this in your project |
| `dht11.c` | Driver implementation |

## API

### `void dht11_init(DHT11 *dht, char port, uint8_t pin)`

Configures the data line and prepares the sensor for reading.

| Parameter | Description |
|-----------|--------------|
| `port` | AVR port the sensor is wired to — `'A'`, `'B'`, `'C'`, or `'D'` |
| `pin` | Pin number on that port (0–7) |

- Sets the pin as input with the internal pull-up enabled, matching the sensor's idle-high line state
- Blocks for **1 second** — the DHT11 datasheet requires this stabilization time before the first read
- An invalid `pin` (>7) or `port` is silently ignored (no configuration is performed)

Call once at startup, before any `dht11_read()`.

---

### `uint8_t dht11_read(DHT11 *dht, DHT11_Data *data)`

Performs a full read cycle: host start signal → sensor response → 40 data bits → checksum.

**Returns:** `0` on success, `1` on timeout or checksum failure.

| Step | What happens |
|------|--------------|
| Host start | Line pulled low 18 ms, then released for 30 µs |
| Sensor response | Sensor pulls low 80 µs, then high 80 µs |
| Data (40 bits) | Each bit: 50 µs low, then a high pulse — ~26–28 µs = `0`, ~70 µs = `1` |
| Checksum | Sum of the first 4 bytes must equal the 5th byte, or the read is discarded |

- `data->humidity` and `data->temperature` are populated with the **integer** bytes only — the DHT11 has no fractional resolution (unlike the DHT22), so the decimal bytes in the payload are read but not exposed
- Every bit-level wait is timeout-guarded (~200 µs via `wait_for_level()`), so a disconnected or stuck sensor returns `1` rather than hanging the MCU
- The DHT11 needs **≥1 second between reads** — this isn't enforced by the driver itself, so throttle calls to `dht11_read()` in your application (e.g. via a timer interrupt)

---

## Usage Example

```c
#include "dht11.h"
#include "lcd.h"

int main(void)
{
    DHT11 dht;
    DHT11_Data data;
    LCD lcd;

    lcd_init_I2C(&lcd, 0x27);
    dht11_init(&dht, 'C', 2);   // sensor on PC2

    while (1)
    {
        if (dht11_read(&dht, &data) == 0)
        {
            lcd_goto(&lcd, 0, 0);
            lcd_print(&lcd, "Temp:");
            // format data.temperature / data.humidity onto the LCD here
        }
        _delay_ms(2000);   // respect the sensor's minimum sample interval
    }
}
```

## Hardware Notes

- A **4.7 kΩ–10 kΩ external pull-up** to VCC on the data line is recommended per the datasheet, in addition to the internal pull-up the driver enables — especially over longer wire runs
- Timing is entirely `_delay_us()`/`_delay_ms()`-based, so it depends on `F_CPU` being defined correctly for your build — re-verify timing after any clock speed change
- **Simulation note:** DHT11 timing simulation in Proteus is known to be unreliable — verify readings on real hardware, not just in simulation
- Each `DHT11` struct is independent, so multiple sensors on different pins can be handled with separate instances

## Dependencies

- `avr/io.h` (avr-libc)
- `util/delay.h` (avr-libc)

## License

MIT
