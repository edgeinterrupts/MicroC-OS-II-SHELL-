/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_lcd.c
*  Author: Omer Farooq
*  Date: 20-January-05
*
*  UART0 Hardware Driver. This driver is any application and OS specific.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "hw_init.h"
#include "hw_uart0.h"
#include "includes.h"

/*
*********************************************************************************************************
*                                             CONSTANTS
*********************************************************************************************************
*/
#define RxBufferSize   8			 /* Alway should be a power of 2, so we shift in HW. */

/*
*********************************************************************************************************
*                                             FILE LEVEL GLOBALS
*********************************************************************************************************
*/
static INT8U RxHeadPtr = 0; 
static INT8U RxTailPtr = 0; 
static char RxBuffer[RxBufferSize];  /* circular buffer for incoming pointers */ 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitMSP430UART0
*
* Description: This function initializes the uart0 on the MSP430 board.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void InitMSP430UART0()
{
    RxHeadPtr = RxTailPtr = 0;
    U0CTL  |= UART_LFO_CRTL;     
    U0TCTL |= UART_LFO_TX_CRTL;  
    U0BR0 = UART_LFO_B0_CRTL;    
    U0BR1 = UART_LFO_B1_CRTL;    
    U0MCTL = UART_LFO_M_CRTL;    
    ME1 |= (UTXE0|URXE0);        
    UCTL0 &= ~SWRST;               // Do  a software reset of the serial port.
    IE1 |= (URXIE0 | UTXIE0);      // Enable UART0 interrupts 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        UART0RXIntHandler
*
* Description: This function is called by the uart0 Rx handler. This function job is to save the incoming 
*              byte locally and send a notifiction upto the kernel that there is byte received. 
*  
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void UART0RXIntHandler( void )
{
   char recvdByte = U0RXBUF;
   if ( ((RxHeadPtr + 1) % RxBufferSize) != RxTailPtr) /* If the queue is not full */
   {
       	RxBuffer[RxHeadPtr] = recvdByte;
       	RxHeadPtr = (RxHeadPtr + 1) % RxBufferSize;
       	OSSerialByteRxRecvd();                             /* Notify the Upper layer about a new byte */
   }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        UART0TXIntHandler
*
* Description: This function is called by the uart0 Tx handler. This function only job is to 
*              report character transmission to the kernel. 
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void UART0TXIntHandler( void )
{
	OSSerialByteTxSent();   /* Acknowlege the kernel about last byte sent */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        UART0SendByte
*
* Description: This function is part of the HAL (hardware abstration layer). This function needs to be
*              defined as specified by the HAL design. This function abstract actual hardware byte 
*              transmission from the application and BSP level software.
*
* Arguments  : Data -- byte that is supposed to be written out.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void UART0SendByte(const char Data)
{
	U0TXBUF = Data;	/* Write the byte to the hardware, it is safe. */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        UART0GetByte
*
* Description: This function is called by the upper layer to reterive an received byte. 
*              This function is part of the HAL (hardware abstration layer). This function needs to be
*              defined as specified by the HAL design. 
*
* Arguments  : none
*
* Returns    : char
*
* Note(s)    : 
*********************************************************************************************************
*/
char UART0GetByte()
{
	char byteReturn = 0; 

   if (RxTailPtr != RxHeadPtr) /* This should never happen since the kernel wait for receive before calling this */
	{
		byteReturn = RxBuffer[RxTailPtr];
		RxTailPtr = (RxTailPtr + 1) % RxBufferSize;
	}

	return byteReturn;
}
