#ifndef DHT11_H
#define DHT11_H

#include <avr/io.h>
#include <util/delay.h>

typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pinreg;
    uint8_t pin;
} DHT11;

typedef struct
{
    uint8_t humidity;
    uint8_t temperature;
} DHT11_Data;

void dht11_init(DHT11 *dht, char port, uint8_t pin);

uint8_t dht11_read(DHT11 *dht, DHT11_Data *data);

#endif
