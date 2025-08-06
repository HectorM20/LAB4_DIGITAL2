#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#include <avr/io.h>
// Inicializa el módulo I2C como maestro configurando la velocidad del reloj I2C.
void i2c_init(void);

// Inicia una comunicación I2C enviando una señal START.
void i2c_start(void);

// Finaliza la comunicación I2C.
void i2c_stop(void);

//Lee un byte y envía ACK al esclavo indicando que quieres seguir leyendo.
uint8_t i2c_read_ack(void);

// Lee el último byte y no envía ACK, indicando fin de lectura.
uint8_t i2c_read_nack(void);

// Envía un byte al esclavo (por ejemplo, dirección o datos)
void i2c_write(uint8_t data);

// Función que encapsula un ciclo completo: START -> SLA+R -> READ -> STOP
uint8_t i2c_read_from(uint8_t address);



#endif /* I2C_MASTER_H_ */