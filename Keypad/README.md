# Matrix Keypad Driver — AVR

A struct-based 4×4 matrix keypad driver for AVR microcontrollers, written in C for ATmega32.

---

## File Structure

```
Keypad/
├── keypad.h    # Driver interface & Keypad struct
└── keypad.c    # Driver implementation
```

---

## Initialization

```c
Keypad keypad;
keypad_init(&keypad, 'A');  // 'A', 'B', 'C', or 'D' — the AVR port connected to the keypad
```

---

## API

```c
// Returns the ASCII value of the pressed key, or 0 if none pressed
uint8_t keypad_get_pressed(Keypad *keypad);
```

---

## Key Mapping

| Key | ASCII |
|-----|-------|
| `0`–`9` | `0x30`–`0x39` |
| `=` | `0x3D` |
| Backspace | `0x08` |
| `+` `-` `x` `/` | Standard ASCII |

---

## Requirements

- AVR-GCC toolchain
- ATmega32 (or compatible AVR)

---

## License

MIT
