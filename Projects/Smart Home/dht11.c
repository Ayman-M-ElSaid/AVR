#include "dht11.h"

#define TIMEOUT_US 200u


static uint8_t wait_for_level(DHT11 *dht, uint8_t level)
{
    uint16_t count = 0;
    while (((*dht->pinreg >> dht->pin) & 1) != level)
    {
        _delay_us(1);
        if (++count > TIMEOUT_US)
            return 1;
    }
    return 0;
}

void dht11_init(DHT11 *dht, char port, uint8_t pin)
{
    if (pin > 7)
        return;
    switch (port)
    {
    case 'A':
        dht->port = &PORTA;
        dht->ddr = &DDRA;
        dht->pinreg = &PINA;
        break;
    case 'B':
        dht->port = &PORTB;
        dht->ddr = &DDRB;
        dht->pinreg = &PINB;
        break;
    case 'C':
        dht->port = &PORTC;
        dht->ddr = &DDRC;
        dht->pinreg = &PINC;
        break;
    case 'D':
        dht->port = &PORTD;
        dht->ddr = &DDRD;
        dht->pinreg = &PIND;
        break;
    default:
        return;
    }
    dht->pin = pin;

    *dht->ddr &= ~(1 << (dht->pin));
    *dht->port |= (1 << (dht->pin));
    _delay_ms(1000);
}

uint8_t dht11_read(DHT11 *dht, DHT11_Data *data)
{
    /* ---- 1. Host start signal ------------------------------------ */
    *dht->ddr |= (1 << (dht->pin));
    *dht->port &= ~(1 << (dht->pin));
    _delay_ms(18);

    *dht->ddr &= ~(1 << (dht->pin));
    *dht->port |= (1 << (dht->pin));
    _delay_us(30);

    /* ---- 2. Sensor response -------------------------------------- */
    if (wait_for_level(dht, 0))
        return 1;
    if (wait_for_level(dht, 1))
        return 1;
    if (wait_for_level(dht, 0))
        return 1;

    /* ---- 3. Read 40 bits ----------------------------------------- */
    uint8_t raw[5] = {0, 0, 0, 0, 0};

    for (uint8_t byte_idx = 0; byte_idx < 5; byte_idx++)
    {
        for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            if (wait_for_level(dht, 1))
                return 1;

            _delay_us(40);
            raw[byte_idx] <<= 1;
            if ((*dht->pinreg >> dht->pin) & 1)
            {
                raw[byte_idx] |= 1;
                if (wait_for_level(dht, 0))
                    return 1;
            }
        }
    }

    /* ---- 4. Verify checksum ------------------------------------- */
    uint8_t checksum = (uint8_t)(raw[0] + raw[1] + raw[2] + raw[3]);
    if (checksum != raw[4])
        return 1;

    /* ---- 5. Populate output ------------------------------------- */
    data->humidity = raw[0];    /* integer part, % RH */
    data->temperature = raw[2]; /* integer part, °C   */

    *dht->ddr &= ~(1 << (dht->pin));
    *dht->port |= (1 << (dht->pin));
    return 0;
}
