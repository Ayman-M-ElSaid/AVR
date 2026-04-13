#include "adc.h"

void adc_init()
{
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;
}

void set_prescaler(uint8_t prescaler)
{
    bool PS0, PS1, PS2;
    switch (prescaler)
    {
    case 2:
        PS0 = 1, PS1 = 0, PS2 = 0;
        break;
    case 4:
        PS0 = 0, PS1 = 1, PS2 = 0;
        break;
    case 8:
        PS0 = 1, PS1 = 1, PS2 = 0;
        break;
    case 16:
        PS0 = 0, PS1 = 0, PS2 = 1;
        break;
    case 32:
        PS0 = 1, PS1 = 0, PS2 = 1;
        break;
    case 64:
        PS0 = 0, PS1 = 1, PS2 = 1;
        break;
    case 128:
        PS0 = 1, PS1 = 1, PS2 = 1;
        break;
    default:
        return;
    }
    ADCSRA &= ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
    ADCSRA |= (PS2 << ADPS2) | (PS1 << ADPS1) | (PS0 << ADPS0);
}

uint16_t adc_read(uint8_t pin)
{
    DDRA &= ~(1 << pin);

    ADMUX = (ADMUX & 0b11100000) | pin;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;

    return ADC;
}
