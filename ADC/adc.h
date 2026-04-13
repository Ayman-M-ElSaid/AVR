#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void adc_init();
uint16_t adc_read(uint8_t pin);

#endif
