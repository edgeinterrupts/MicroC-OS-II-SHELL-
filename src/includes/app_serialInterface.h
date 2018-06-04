#ifndef APP_SERIAL_IF_H
#define APP_SERIAL_IF_H

/*$PAGE*/
/*
*********************************************************************************************************
*  File: includes.h
*  Author: Omer Farooq
*  Date:  14-December-05
*
*  Master include file.
* 
*  This file originally came with MSP430 port. I have cleaned it up 
*  and removed stuff that was unused or is not needed anymore. 
*********************************************************************************************************
*/ 

/*
*********************************************************************************************************
*                                             CONSTANTS
*********************************************************************************************************
*/
#define INTIME    0
#define TIMEDOUT  1

/*
*********************************************************************************************************
*                                             Function prototypes 
*********************************************************************************************************
*/
void InitSerialInterface( void );
void OSSerialByteTxSent( void ); 
void OSSerialByteRxRecvd( void );
void SerialPutString( const char* msg ); 
void SerialPutChar( char data );
void SerialPutDecNum( INT32U number );
INT8U SerialGetCharTimeout(char* ch, INT16U timeout);
char SerialGetChar( void );

#endif /* APP_SERIAL_IF_H*/
