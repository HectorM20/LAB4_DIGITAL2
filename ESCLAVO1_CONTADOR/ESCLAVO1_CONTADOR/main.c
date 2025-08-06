//***************************************************************
//Universidad del Valle de Guatemala
//Electrónica Digital 2
//Autor: Héctor Alejandro Martínez Guerra
//Hardware: ATMEGA328P
//LAB4
//***************************************************************
//***************************************************************

//ESCLAVO
//ESCLAVO
//ESCLAVO

// Encabezado
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c_slave/i2c_slave.h"

// Prototipos
void setup(void);
uint8_t get_counter_value(void);

// Variables globales
volatile uint8_t contador = 0;

//--------------------------------------------------------------
int main(void)
{
	setup();
	while (1)
	{
		// Leer botón de incremento (PC0)
		if (!(PINC & (1 << PC0))) {
			_delay_ms(50); // Antirrebote
			if (!(PINC & (1 << PC0))) {
				if (contador < 15) contador++;
				PORTD = (PORTD & 0xF0) | (contador & 0x0F); // Mostrar en PD0-PD3
				while (!(PINC & (1 << PC0))); // Esperar a soltar
			}
		}

		// Leer botón de decremento (PC1)
		if (!(PINC & (1 << PC1))) {
			_delay_ms(50); // Antirrebote
			if (!(PINC & (1 << PC1))) {
				if (contador > 0) contador--;
				PORTD = (PORTD & 0xF0) | (contador & 0x0F); // Mostrar en PD0-PD3
				while (!(PINC & (1 << PC1))); // Esperar a soltar
			}
		}
	}
}

//--------------------------------------------------------------
void setup(void)
{
	cli();  // Deshabilitar interrupciones globales
	
	// Deshablitar comunicación UART
	UCSR0A = 0x00;
	UCSR0B = 0x00;
	UCSR0C = 0x00;

	// Configurar PD0-PD3 como salidas (LEDs)
	DDRD |= 0x0F;

	// Configurar PC0 y PC1 como entradas (botones)
	DDRC &= ~((1 << PC0) | (1 << PC1));
	PORTC |= (1 << PC0) | (1 << PC1); // Activar pull-up internos

	// Inicializar valor del contador
	PORTD = (PORTD & 0xF0) | (contador & 0x0F);

	// Inicializar esclavo I2C con dirección 0x10
	i2c_slave_init(0x10);
	i2c_set_transmit_callback(get_counter_value);

	sei();  // Habilitar interrupciones globales
}

//--------------------------------------------------------------
// Función que devuelve el valor actual del contador
uint8_t get_counter_value(void)
{
	return contador;
}