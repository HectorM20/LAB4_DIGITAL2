//***************************************************************
//Universidad del Valle de Guatemala
//Electrónica Digital 2
//Autor: Héctor Alejandro Martínez Guerra
//Hardware: ATMEGA328P
//LAB4
//***************************************************************
//***************************************************************


//Encabezado
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd/lcd.h"
#include "i2c_master/i2c_master.h"

// Prototipos
void setup(void);
uint8_t read_byte_from(uint8_t address);
uint16_t read_word_from(uint8_t address);

int main(void)
{
	uint8_t contador;
	uint16_t adc_valor;

	setup();

	while (1)
	{
		// Leer contador (1 byte) del esclavo #1 (0x10)
		contador = read_byte_from(0x10);

		// Leer valor de 2 bytes del esclavo #2 (0x20)
		adc_valor = read_word_from(0x20);

		// Mostrar resultados en la LCD
		lcd_gotoxy(0, 0);
		lcd_print("Contador:       ");
		lcd_gotoxy(10, 0);
		lcd_print_num(contador);

		lcd_gotoxy(0, 1);
		lcd_print("ADC:            ");
		lcd_gotoxy(5, 1);
		lcd_print_num(adc_valor);

		_delay_ms(200);
	}
}

//--------------------------------------------------------------
void setup(void)
{
	cli();          // Deshabilitar interrupciones globales
	UCSR0A = 0x00;
	UCSR0B = 0x00;
	UCSR0C = 0x00;
	lcd_init();     // Inicializa pantalla
	lcd_clear();    // Limpia contenido
	i2c_init();     // Inicializa bus I2C como maestro
	sei();          // Habilita interrupciones globales
}

//--------------------------------------------------------------
// Leer 1 byte desde esclavo (como contador)
uint8_t read_byte_from(uint8_t address)
{
	uint8_t data;
	i2c_start();
	i2c_write((address << 1) | 1);  // SLA+R
	data = i2c_read_nack();         // Leer un solo byte sin ACK
	i2c_stop();
	return data;
}

//--------------------------------------------------------------
// Leer 2 bytes desde esclavo (como ADC 10 bits)
uint16_t read_word_from(uint8_t address)
{
	uint8_t lsb, msb;
	i2c_start();
	i2c_write((address << 1) | 1);  // SLA+R
	lsb = i2c_read_ack();          // Primer byte ? LSB
	msb = i2c_read_nack();         // Segundo byte ? MSB
	i2c_stop();
	return ((uint16_t)msb << 8) | lsb;
}