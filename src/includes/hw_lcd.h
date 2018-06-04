#ifndef HW_LCD_H
#define HW_LCD_H

/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_lcd.h
*  Author: Omer Farooq
*  Date:   14-December-05
*
* Header file for LCD driver.
* 
* NOTE: Most of the comments in this file are from orignal lcd.s provided
*       by Dr. Philips.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             CONSTANTS
*********************************************************************************************************
*/  
#define	LCD_FUNSET    0x3C
#define LCD_OFF 	  0x08
#define	LCD_CLR 	  0x01
#define	LCD_EMS 	  0x06
#define	LCD_ON 	      0x0f

#define	LCD_LINE0 	  0x00
#define	LCD_LINE1 	  0x40
#define	LCD_LINE_SZ   0x14
#define	LCD_HOME 	  0x02

/* 
 * This must be written to P5OUT before changing from write to read mode
 * the LCD is interfaced through 5-3.3V open collector non-inverting buffers
 * writing a one (1) releases the buffer's hard pull-down preventing
 * contestion on the line when the LCD tries to drive it
 * there is still a strong pull-up to 5V through a 2.2K Ohm resistor. 
 */
#define	LCD_OUT2IN        0xFF

/* SW init equates */
#define	LCD_INIT          0x30

/*
 * the delay loop takes n*3(for the dec&jnz) + 5(for the call) + 3(for the ret)
 * cycles @ MCLK = 7.3728 MHz
 * this needs to be > 15ms
 */
#define	LCD_INIT_WAIT0 0x8FFE

/*  > 4.1ms */
#define	LCD_INIT_WAIT1 0x275A

/*  > 100us */
#define	LCD_INIT_WAIT2 0x00F4

/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void InitLCD();
void ClearLCD();
void WriteCharToLCD(char character);
void WriteStringToLCD(const char* msg);
void SetLCDCursorPosition(unsigned int position);
void SetDisplyDDRAMPosition(unsigned int position);

#endif /* HW_LCD_H */
