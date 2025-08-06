#include "i2c_slave.h"
#include <avr/interrupt.h>

// Función tipo puntero, que apuntará a la rutina que entrega el valor al maestro cuando lo solicite.
static uint8_t (*transmit_callback)(void) = 0;

// Inicializar como esclavo
void i2c_slave_init(uint8_t adress){
	cli();										// Deshabilita interrupciones
	TWAR = (adress << 1);						// Dirección esclavo (7 bits) alineada a la izquierda
	TWCR = (1 << TWEA) |						// Habilita respuesta ACK
		   (1 << TWEN) |						// Habilita módulo TWIN
		   (1 << TWIE);							// Habilita interrupciones TWI
	sei();										// Habilita interrupciones globales
}		   

// Permite al usuario definir qué función se ejecutará cuando el maestro solicite un byte (en este caso, el valor del contador)
void i2c_set_transmit_callback(uint8_t (*callback)(void)) {
	transmit_callback = callback;
}

// Rutina de interrupción del TWI
// El maestro envió una dirección con escritura (SLA+W) o una dirección general.

ISR(TWI_vect){
	switch(TWSR & 0xF8){
		case 0x60:			// SLA+W recibido, ACK enviado
		case 0x68:			// SLA+W recibido en modo esclavo anteriormente ocupado
		case 0x70:			// Dirección general recibida
		case 0x78:			// Dirección general recibida mientras estaba ocupado
			
			// Esperar datos del maestro
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
			break;
		
		// Maestro envió SLA+R (leer)
		case 0xA8:			// SLA+R recibido, ACK enviado -> el maestro quiere leer
		case 0xB0:			// SLA+R recibido en modo ocupado
			// Colocar el dato a enviar
			if (transmit_callback){
				// El maestro quiere leer un dato. El esclavo coloca el valor en TWDR usando la función callback
				TWDR = transmit_callback();			// Colocar el dato del contador
			} else {
				TWDR = 0xFF;						// Valor por defecto si no hay callback
			}
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;
			
		case 0xB8:				// Maestro recibió byte y espera otro (ACK enviado)
			// El maestro pide más datos, se vuelve a enviar el mismo o nuevo valor
			if (transmit_callback){
				TWDR = transmit_callback();			// Colocar el dato del contador
				} else {
				TWDR = 0xFF;						// Valor por defecto si no hay callback
			}
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
			break;
			
		// El maestro terminó la lectura, no pide más datos. El esclavo se reinicia	
		case 0xC0:				// Maestro recibió último byte, no ACK enviado
		case 0xC8:			
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;
			
		// Cuaquier otro estado, se resetea el estado del bus I2C
		default:
			// Restablecer dato
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;	
						
	}
}