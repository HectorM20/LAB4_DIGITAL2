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
#include "adc/adc.h"
#include "i2c_slave/i2c_slave.h"


// Prototipos
void setup(void);
uint8_t adc_callback(void);

// Variables globales
volatile uint16_t adc_value = 0;
volatile uint8_t send_high_byte = 0;  // Controla qué byte se envía primero (LSB ? MSB)

//--------------------------------------------------------------
int main(void)
{
	setup();

	while (1)
	{
		// Leer ADC del canal 7 (A7)
		adc_value = adc_read(7);
		_delay_ms(100);  // Leer cada 100ms
	}
}

//--------------------------------------------------------------
void setup(void)
{
	cli();  // Deshabilitar interrupciones globales

	adc_init();                      // Inicializa el ADC (referencia AVCC, prescaler 64)
	i2c_slave_init(0x20);           // Dirección I2C del esclavo #2
	i2c_set_transmit_callback(adc_callback);  // Registrar función para enviar datos

	sei();  // Habilitar interrupciones globales
}

//--------------------------------------------------------------
// Función llamada por la ISR cuando el maestro quiere leer
uint8_t adc_callback(void)
{
	uint8_t dato;

	if (send_high_byte)
	{
		dato = (adc_value >> 8) & 0xFF;  // Byte alto
		send_high_byte = 0;
	}
	else
	{
		dato = adc_value & 0xFF;         // Byte bajo
		send_high_byte = 1;
	}

	return dato;
}