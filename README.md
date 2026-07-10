# AVR — Embedded Drivers & Projects

A collection of peripheral drivers and full embedded projects for the **ATmega32**, written in C with the AVR-GCC toolchain. Each driver is struct-based, dependency-free (beyond `avr-libc`), and kept generic — no application logic baked into peripheral code. Projects combine multiple drivers into complete, working systems.

---

## Repository Structure

```
AVR/
├── ADC/                  # ADC driver
├── DHT11/                # DHT11 temperature/humidity driver
├── Keypad/                # 4x4 matrix keypad driver
├── LCD/                   # HD44780 LCD driver (4-bit + I2C)
├── OLED/                  # SSD1306 OLED driver
└── Projects/
    ├── Calculator/         # Keypad + LCD calculator
    └── Smart Home/         # Password-protected home automation system
```

---

## Drivers

| Driver | Description | Docs |
|--------|-------------|------|
| [`ADC/`](./ADC) | Blocking single-channel ADC reads with AVcc reference and runtime-configurable prescaler | [README](./ADC/README.MD) |
| [`DHT11/`](./DHT11) | Bit-banged single-wire driver for the DHT11 temperature/humidity sensor | — |
| [`Keypad/`](./Keypad) | Struct-based scanner for 4×4 matrix keypads, returns ASCII key codes | [README](./Keypad/README.md) |
| [`LCD/`](./LCD) | HD44780 character LCD driver — 4-bit parallel and I2C (PCF8574) modes behind one unified API | [README](./LCD/README.md) |
| [`OLED/`](./OLED) | SSD1306 128×64 I2C OLED driver — text, per-page clearing, and PROGMEM bitmap rendering | — |

> Each driver folder is self-contained: a `.h`/`.c` pair (plus a demo `main.c`) that can be dropped into any project.

---

## Projects

| Project | Description | Docs |
|---------|-------------|------|
| [`Projects/Calculator/`](./Projects/Calculator) | Basic 4-function calculator — keypad input, result on an I2C LCD | [README](./Projects/Calculator/README.MD) |
| [`Projects/Smart Home/`](./Projects/Smart%20Home) | Password-protected home automation controller — OLED menu, I2C LCD, DHT11 sensing, light/fan control | [README](./Projects/Smart%20Home/README.md) |

Projects don't import drivers by reference — each project folder carries its own copies of the driver files it needs, so it builds standalone. The driver folders above remain the source of truth for API docs and usage examples.

---

## Toolchain & Hardware

- **MCU:** ATmega32 (drivers use standard `avr/io.h` register access, so they're portable to similar AVR parts with adjustments)
- **Toolchain:** AVR-GCC / avr-libc, built via [PlatformIO](https://platformio.org/)
- **Simulation:** Proteus 8+ (optional) — useful for logic and wiring checks, but timing-sensitive peripherals (DHT11, OLED at full speed) are verified against real hardware, since Proteus doesn't model their timing perfectly
- **Programmer:** USBASP (or compatible)

## Getting Started

1. Pick the driver(s) you need and copy the `.c`/`.h` files into your project's source directory (see any `Projects/*/README.md` for a concrete example of this layout).
2. Include the header(s) and call the `_init()` function for each peripheral before use.
3. Build and upload with PlatformIO, e.g.:

```ini
[env:atmega32]
platform = atmelavr
board = ATmega32
board_build.f_cpu = 8000000UL
upload_protocol = usbasp
upload_flags = -Pusb -B 128
```

---

## License

MIT — see individual project/driver READMEs; the same license applies repo-wide.
