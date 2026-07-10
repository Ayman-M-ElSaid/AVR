# Smart Home — AVR Home Automation System

A password-protected home automation controller running on ATmega32. Combines an OLED
menu system, an I2C LCD for secure entry, a DHT11 sensor, and relay/light/motor control
into a single embedded UI.

---

## Features

- Password-protected system arm/disarm via 4×4 keypad, with masked LCD entry and backspace
- 3 failed attempts trigger a lockout alarm (buzzer + RED LED pulsing)
- OLED main menu (Lighting / Fan / Change Password / Exit) navigated with two push buttons — **Next** to move the cursor, **Confirm** to select
- Live temperature & humidity from DHT11, refreshed every ~2 seconds via a Timer1 overflow interrupt, alongside a weather-icon bitmap (sun / cloudy / cactus) that changes with the readings
- Independent on/off control for 3 lights from the OLED menu
- Fan/motor control: manual toggle, or **Auto** mode driven by an adjustable temperature threshold
- In-menu password change flow with old/new/confirm verification and masked entry
- RED/GREEN status LEDs indicate armed/disarmed state; buzzer gives keypress and alarm feedback

> **Note:** Timer1's preload value and the DHT11 read timing are calibrated for the
> current `F_CPU`. If the clock source changes (e.g. moving to a 16 MHz external
> crystal), recalculate the Timer1 overflow interval and re-verify DHT11 timing.

---

## Preview
<img width="970" height="682" alt="Image" src="https://github.com/user-attachments/assets/ed7d8570-365d-4f38-9e54-a84767dc52fe" />

---

## Hardware

| Component        | Details                                      |
|-------------------|-----------------------------------------------|
| MCU               | ATmega32                                      |
| Display 1         | 16×2 LCD via PCF8574 I2C expander (`0x27`)    |
| Display 2         | 128×64 OLED (SSD1306) via I2C (`0x3C`)        |
| Input             | 4×4 matrix keypad                             |
| Sensor            | DHT11, data pin on Port C pin 2               |
| Nav buttons       | Next → PD2, Confirm → PD3 (active-low, pulled up) |
| Buzzer            | PD4                                            |
| Status LEDs       | RED → PD5, GREEN → PD6                        |
| Lights            | Light 1–3 → PB0, PB1, PB2                     |
| Fan / Motor       | PB3 (via transistor driver)                   |

---

## File Structure

```
smart-home/
├── main.c       # State machine, menu logic, I/O handling
├── lcd.h        # LCD driver interface (from LCD/)
├── lcd.c        # LCD driver implementation
├── keypad.h     # Keypad driver interface (from Keypad/)
├── keypad.c     # Keypad driver implementation
├── oled.h       # OLED driver interface (from OLED/)
├── oled.c       # OLED driver implementation
├── dht.11h      # DHT11 driver interface (from DHT11/)
└── dht11.c      # DHT11 driver implementation
```

---

## Dependencies

This project uses four drivers from this repo:
- [`LCD/`](../../LCD/)
- [`Keypad/`](../../Keypad/)
- [`OLED/`](../../OLED/)
- [`DHT11/`](../../DHT11/)

Copy `lcd.c`, `lcd.h`, `keypad.c`, `keypad.h`, `oled.c`, `oled.h`, `dht.11h`, and
`dht11.c` into this folder before building. The driver folders stay the source of
truth for docs/usage — this README only covers what's specific to the Smart Home build.

---

## How It Works

1. On boot, the system starts **locked** (RED LED on) and prompts for a password on the LCD
2. Correct entry unlocks the system (GREEN LED on) and enters the `TEMP` home screen; 3 wrong entries trigger the alarm loop
3. A Timer1 overflow interrupt sets a flag roughly every 2 seconds, triggering a DHT11 read and OLED refresh while on the home screen
4. **Next** opens the main menu from the home screen, or moves the `>` cursor between menu items; **Confirm** selects the highlighted item
5. From the menu: **Lighting** toggles individual lights, **Fan** toggles the motor / Auto mode / temperature threshold, **Change Password** walks through old → new → confirm entry with masked OLED input
6. In **Auto** fan mode, the motor is driven purely by comparing the last DHT11 reading against the configurable `set_temp` threshold

---

## Requirements

- AVR-GCC toolchain
- ATmega32
- PCF8574 I2C expander with 4.7 kΩ pull-ups on SDA/SCL (shared bus with the OLED)
- SSD1306 128×64 OLED module
- DHT11 sensor
- Transistor driver stage for the fan/motor
- 4×4 matrix keypad

---

## License

MIT
