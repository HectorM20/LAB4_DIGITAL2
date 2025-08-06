#ifndef LCD_H
#define LCD_H

//Prototipos de funciones
void lcd_init(void);
void lcd_clear(void);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_print(char* str);
void lcd_print_num(uint16_t num);

#endif
