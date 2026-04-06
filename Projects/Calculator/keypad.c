#include "keypad.h"
const uint8_t default_keys[4][4] = {{0x31, 0x32, 0x33, 0x2B},
                                    {0x34, 0x35, 0x36, 0x2D},
                                    {0x37, 0x38, 0x39, 0x78},
                                    {0x08, 0x30, 0x3D, 0x2F}};

void keypad_init(Keypad *keypad, char port_name)
{
    switch (port_name)
    {
    case 'A':
        ADCSRA &= ~(1 << ADEN);
        keypad->port = &PORTA;
        keypad->ddr = &DDRA;
        keypad->pins = &PINA;
        break;
    case 'B':
        keypad->port = &PORTB;
        keypad->ddr = &DDRB;
        keypad->pins = &PINB;
        break;
    case 'C':
        keypad->port = &PORTC;
        keypad->ddr = &DDRC;
        keypad->pins = &PINC;
        break;
    case 'D':
        keypad->port = &PORTD;
        keypad->ddr = &DDRD;
        keypad->pins = &PIND;
        break;
    default:
        return;
    }

    *(keypad->ddr) = 0x0F;
    *(keypad->port) = 0xF0;
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            keypad->keys[i][j] = default_keys[i][j];
        }
    }
}

uint8_t keypad_get_pressed(Keypad *keypad)
{

    for (int row = 0; row < 4; row++)
    {
        *(keypad->port) = 0xFF;
        *(keypad->port) &= ~(1 << row);

        for (int col = 0; col < 4; col++)
        {
            if (!(*(keypad->pins) & (1 << (col + 4))))
            {
                _delay_ms(20);
                while (!(*(keypad->pins) & (1 << (col + 4))));
                return keypad->keys[row][col];
            }
        }
    }
    return 0;
}
