#include "adc.h"

void adc_init()
{
    ADMUX  = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

uint16_t adc_read(uint8_t pin)
{
    DDRA  &= ~(1 << pin);

    ADMUX  = (ADMUX & 0b11100000) | pin;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}
