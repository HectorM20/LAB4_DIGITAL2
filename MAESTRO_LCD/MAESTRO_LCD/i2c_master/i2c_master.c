#include "i2c_master.h"

void i2c_init(void) {
	// Pines I2C como entrada. (No hace falta configurar los pines si se activa TWEN)
	// DDRC &= ~((1<<DDC4)|(1<<DDC5));
	// Frecuencia = F_CPU / (16 + 2 * TWBR * prescaler)
	// Para frecuencia SCL de 100kHz, con F_CPU = 16MHz, prescaler de 1. Buscar TWBR
	TWSR = 0x00;		// Prescaler = 1. bits TWSP1:TWSP0 = 00
	TWBR = 72;			// Valor para que la velocidad SCL sea 100kHz       
}

// Inicia una comunicaci�n I2C enviando una se�al START.
void i2c_start(void) {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);	//Activar TWI y enviar condici�n START
	while (!(TWCR & (1 << TWINT)));  // Esperar a que se complete el START
}

// Finaliza la comunicaci�n I2C.
void i2c_stop(void) {
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);	// Enviar STOP
}

// Env�a un byte al esclavo (por ejemplo, direcci�n o datos).
void i2c_write(uint8_t data) {
	TWDR = data;						// Carga dato en el registro de datos
	TWCR = (1 << TWEN) | (1 << TWINT);	// Inicia envio
	while (!(TWCR & (1 << TWINT)));  // Esperar a que se complete el env�o
}

// Lee un byte y env�a ACK al esclavo indicando que quieres seguir leyendo.
uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);  // Leer con ACK
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

// Lee el �ltimo byte y no env�a ACK, indicando fin de lectura.
uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWEN) | (1 << TWINT);  // Leer sin ACK
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

// Funci�n que encapsula un ciclo completo: START -> SLA+R -> READ -> STOP
uint8_t i2c_read_from(uint8_t address) {
	uint8_t data;

	i2c_start();					// START
	i2c_write((address << 1) | 1);  // SLA+R (bit 0 = 1)
	data = i2c_read_nack();			// leer un byte, sin ACK
	i2c_stop();						// STOP

	return data;					// Retornar byte le�do
}
// El maestro lee un byte de un esclavo (contandor de 4 bits o ADC)