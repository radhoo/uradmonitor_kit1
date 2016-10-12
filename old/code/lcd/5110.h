/*
 * Nokia 3110 / 5110 unbuffered driver
 * (C)2013 Radu Motisan
 * www.pocketmagic.net
 * Based on a work by Tony Myatt - 2007
 */

#pragma once

#include "../timeout.h"

// define  Lcd screen size 
#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define LCD_SETXADDR			0x80
#define LCD_SETYADDR			0x40
#define LCD_EXTENDED_COMMANDS	0x21	// LCD Extended Commands
#define LCD_VOP					0xC8	// Set LCD Vop(Contrast)
#define LCD_TEMP				0x06	// Set Temp coefficent
#define LCD_BIAS_MODE			0x13	// LCD bias mode 1:48
#define LCD_HOR_ADDRESSING		0x20	// Standard Commands, Horizontal addressing
#define LCD_NORMAL_MODE			0x0C	// LCD in normal mode


class LCD_5110 {
	/* Command type sent to the lcd */
	typedef enum { LCD_CMD  = 0, LCD_DATA = 1 } LcdCmdData;
	
	uint8_t  m_dq_SCE, m_dq_RST, m_dq_DC, m_dq_DATA, m_dq_CLK;
	volatile uint8_t  *m_port_SCE, *m_port_RST, *m_port_DC, *m_port_DATA, *m_port_CLK;
	
	int lcdCacheIdx;

	volatile uint8_t* Port2DDR(volatile uint8_t *port) {
		return port - 1;
	}
	


public:
	void init(
		volatile uint8_t  *port_RST, uint8_t  dq_RST,
		volatile uint8_t  *port_SCE, uint8_t  dq_SCE,
		volatile uint8_t  *port_DC, uint8_t  dq_DC,
		volatile uint8_t  *port_DATA, uint8_t  dq_DATA,
		volatile uint8_t  *port_CLK, uint8_t  dq_CLK
	);
	void contrast(unsigned char contrast);
	void clear(void);
	void clear_area(unsigned char line, unsigned char startX, unsigned char endX);
	void clear_line(unsigned char line);
	void goto_xy(unsigned char x, unsigned char y);
	void goto_xy_exact(unsigned char x, unsigned char y);
	void send_chr(char chr);
	void send_string(char* str);
	void send_format_string(const char *szFormat, ...);
	void send(unsigned char data, LcdCmdData cd);
	void base_addr(unsigned int addr) ;
	void col(char chr);
	void pixelBack(void);
	void printPictureOnLCD ( const unsigned char *data);
	void drawPixel(unsigned char  x, unsigned char  y, int color);
};



