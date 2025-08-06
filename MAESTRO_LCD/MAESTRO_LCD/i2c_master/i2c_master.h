#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#include <avr/io.h>
// Inicializa el m�dulo I2C como maestro configurando la velocidad del reloj I2C.
void i2c_init(void);

// Inicia una comunicaci�n I2C enviando una se�al START.
void i2c_start(void);

// Finaliza la comunicaci�n I2C.
void i2c_stop(void);

//Lee un byte y env�a ACK al esclavo indicando que quieres seguir leyendo.
uint8_t i2c_read_ack(void);

// Lee el �ltimo byte y no env�a ACK, indicando fin de lectura.
uint8_t i2c_read_nack(void);

// Env�a un byte al esclavo (por ejemplo, direcci�n o datos)
void i2c_write(uint8_t data);

// Funci�n que encapsula un ciclo completo: START -> SLA+R -> READ -> STOP
uint8_t i2c_read_from(uint8_t address);



#endif /* I2C_MASTER_H_ */