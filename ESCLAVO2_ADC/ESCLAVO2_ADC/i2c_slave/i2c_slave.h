
#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void i2c_slave_init(uint8_t address);

// Función tipo puntero, que apuntará a la rutina que entrega el valor al maestro cuando lo solicite.
void i2c_set_transmit_callback(uint8_t (*callback)(void));

#endif /* I2C_SLAVE_H_ */