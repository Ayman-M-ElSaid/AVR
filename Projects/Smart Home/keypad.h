#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <avr/io.h>
#include <util/delay.h>

typedef struct
{
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pins;
    uint8_t keys[4][4];
} Keypad;

void keypad_init(Keypad *keypad, char port_name);
uint8_t keypad_get_pressed();

#endif