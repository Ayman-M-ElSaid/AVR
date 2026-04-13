#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdbool.h>

void adc_init();
void set_prescaler(uint8_t prescaler);
uint16_t adc_read(uint8_t pin);

#endif
