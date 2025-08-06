#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

// LCD Pines (modo 8 bits)
// RS: PB4
// E:  PB3
// D0-D5: PD2–PD7
// D6-D7: PB0–PB1

//Pines de control
#define RS PB4			//Enviar un comando (RS=0), enviar datos (RS=1)
#define E  PB3			//Activar la escritura de datos o comandos cuando hay un flanco de bajada (1 a 0).

//Configuración bit a bit
//Función encargada de enviar bit por bit
void lcd_set_data(uint8_t data) {
	//Bits 0–5: PORTD (PD2–PD7)
	if (data & (1 << 0)) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
	if (data & (1 << 1)) PORTD |= (1 << PD3); else PORTD &= ~(1 << PD3);
	if (data & (1 << 2)) PORTD |= (1 << PD4); else PORTD &= ~(1 << PD4);
	if (data & (1 << 3)) PORTD |= (1 << PD5); else PORTD &= ~(1 << PD5);
	if (data & (1 << 4)) PORTD |= (1 << PD6); else PORTD &= ~(1 << PD6);
	if (data & (1 << 5)) PORTD |= (1 << PD7); else PORTD &= ~(1 << PD7);

	//Bits 6–7: PORTB (PB0, PB1)
	if (data & (1 << 6)) PORTB |= (1 << PB0); else PORTB &= ~(1 << PB0);
	if (data & (1 << 7)) PORTB |= (1 << PB1); else PORTB &= ~(1 << PB1);
}

//Función para mandar comando a LCD
void lcd_cmd(uint8_t cmd) {
	PORTB &= ~(1 << RS);		//RS = 0, indica que se va a enviar un comando
	lcd_set_data(cmd);			//Se envía el byte para ejecutar un comando
	PORTB |= (1 << E);			//Enable en High, la LCD se prepara para capturar los datos del bus de datos
	_delay_us(1);				//Delay para la lectura de valores
	PORTB &= ~(1 << E);			//Enable en Low, capturar los datos del bus
	_delay_ms(2);
}

void lcd_data(uint8_t data) {
	PORTB |= (1 << RS);			//RS = 1, indica que se va a enviar un dato
	lcd_set_data(data);			//Coloca los bits del carácter a mostras, sobre pines de datos
	PORTB |= (1 << E);			//prepararse para leer los datos del bus
	_delay_us(1);
	PORTB &= ~(1 << E);			//Capturar el dato presente en los pines
	_delay_ms(2);
}

void lcd_init(void) {
	//Control (RS, E, PB0, PB1)
	DDRB |= (1 << RS) | (1 << E) | (1 << PB0) | (1 << PB1);
	//Datos: PD2–PD7
	DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4)
	| (1 << PD5) | (1 << PD6) | (1 << PD7);

	_delay_ms(20);				//Delay  mínimo de 15 ms
	lcd_cmd(0x38);				//Modo 8 bits, 2 líneas
	lcd_cmd(0x0C);				//Display ON, cursor OFF
	lcd_cmd(0x01);				//Limpiar display 
	_delay_ms(2);
	lcd_cmd(0x06);				//Entry mode
}

//Función para limpiar contenido en pantalla
void lcd_clear(void) {
	lcd_cmd(0x01);				//Comando: limpiar el lcd y lleva el cursor a la posición inicial, siempre se empieza en este estado
	_delay_ms(2);
}

//Mueve el cursor a la posición (x, y)
//La primera fila comienza en 0x00
//La segunda fila comienza en 0x40
void lcd_gotoxy(uint8_t x, uint8_t y) {
	uint8_t addr = (y == 0) ? x : 0x40 + x;
	lcd_cmd(0x80 | addr);		//Comando: Mover cursor 
}

//Muestra una cadena (char*) en la pantalla.
void lcd_print(char* str) {
	while (*str) {				//Mientas el carácter actual no sea el último de una cadena, continúa
		lcd_data(*str++);
	}
}

//Convierte un número entero a texto con itoa() y lo imprime.
void lcd_print_num(uint16_t num) {
	char buffer[6];				//Areglo de 6 caracteres
	itoa(num, buffer, 10);
	lcd_print(buffer);
}
