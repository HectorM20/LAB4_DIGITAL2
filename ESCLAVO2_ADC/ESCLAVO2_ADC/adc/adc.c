#include <avr/io.h>
#include "adc.h"
//Configuración del ADC
void adc_init(void) {
	ADMUX = (1 << REFS0);					//Referencia AVCC (5V)
	ADCSRA = (1 << ADEN)					//Habilita ADC
	| (1 << ADPS2) | (1 << ADPS1);			//Prescaler 64
}

//
uint16_t adc_read(uint8_t canal)
{
	if (canal > 7) canal = 0;

	ADMUX = (ADMUX & 0xF0) | (canal & 0x07); //borra los bits bajos del registro ADMUX (donde se elige el canal), y luego pone el nuevo valor.

	ADCSRA |= (1 << ADSC);					//Inicia conversión
	while (ADCSRA & (1 << ADSC));			//Espera a que termine

	return ADC;								//Devuelve el valor entre 0-1023
}