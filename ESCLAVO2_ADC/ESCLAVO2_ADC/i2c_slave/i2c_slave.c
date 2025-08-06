#include "i2c_slave.h"
#include <avr/interrupt.h>

// Funci�n tipo puntero, que apuntar� a la rutina que entrega el valor al maestro cuando lo solicite.
static uint8_t (*transmit_callback)(void) = 0;

// Inicializar como esclavo
void i2c_slave_init(uint8_t adress){
	cli();										// Deshabilita interrupciones
	TWAR = (adress << 1);						// Direcci�n esclavo (7 bits) alineada a la izquierda
	TWCR = (1 << TWEA) |						// Habilita respuesta ACK
		   (1 << TWEN) |						// Habilita m�dulo TWIN
		   (1 << TWIE);							// Habilita interrupciones TWI
	sei();										// Habilita interrupciones globales
}		   

// Permite al usuario definir qu� funci�n se ejecutar� cuando el maestro solicite un byte (en este caso, el valor del contador)
void i2c_set_transmit_callback(uint8_t (*callback)(void)) {
	transmit_callback = callback;
}

// Rutina de interrupci�n del TWI
// El maestro envi� una direcci�n con escritura (SLA+W) o una direcci�n general.

ISR(TWI_vect){
	switch(TWSR & 0xF8){
		case 0x60:			// SLA+W recibido, ACK enviado
		case 0x68:			// SLA+W recibido en modo esclavo anteriormente ocupado
		case 0x70:			// Direcci�n general recibida
		case 0x78:			// Direcci�n general recibida mientras estaba ocupado
			
			// Esperar datos del maestro
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
			break;
		
		// Maestro envi� SLA+R (leer)
		case 0xA8:			// SLA+R recibido, ACK enviado -> el maestro quiere leer
		case 0xB0:			// SLA+R recibido en modo ocupado
			// Colocar el dato a enviar
			if (transmit_callback){
				// El maestro quiere leer un dato. El esclavo coloca el valor en TWDR usando la funci�n callback
				TWDR = transmit_callback();			// Colocar el dato del contador
			} else {
				TWDR = 0xFF;						// Valor por defecto si no hay callback
			}
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;
			
		case 0xB8:				// Maestro recibi� byte y espera otro (ACK enviado)
			// El maestro pide m�s datos, se vuelve a enviar el mismo o nuevo valor
			if (transmit_callback){
				TWDR = transmit_callback();			// Colocar el dato del contador
				} else {
				TWDR = 0xFF;						// Valor por defecto si no hay callback
			}
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
			break;
			
		// El maestro termin� la lectura, no pide m�s datos. El esclavo se reinicia	
		case 0xC0:				// Maestro recibi� �ltimo byte, no ACK enviado
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