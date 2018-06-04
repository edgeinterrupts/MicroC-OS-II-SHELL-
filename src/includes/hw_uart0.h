#ifndef HW_UART0_H
#define HW_UART0_H

/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_lcd.h
*  Author: Omer Farooq
*  Date:   04-Febuary-06
*
*  Header file for UART0 driver. 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             CONSTANTS
*********************************************************************************************************
*/
/* UART constants. UART settings for 9600 8N1 */
#define UART_LFO_CRTL   	CHAR
#define UART_LFO_TX_CRTL 	SSEL0
#define UART_LFO_B0_CRTL 	0x03
#define UART_LFO_B1_CRTL 	0x00
#define UART_LFO_M_CRTL 	0x4A

/*
*********************************************************************************************************
*                                             FUCTION PROTOTYPES
*********************************************************************************************************
*/                                                              
void InitMSP430UART0(void);
char UART0GetByte();
void UART0SendString(const char* msg);
void UART0SendByte(const char byte);

#endif /* HW_UART0_H */
