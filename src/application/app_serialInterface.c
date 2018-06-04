/*$PAGE*/

/*
********************************************************************************************************* 
*  File: app_taskwatch.c
*  Author: Omer Farooq
*  Date: 20-February-05
*
*  This file provides the hardware-independent function to read/write
*  characters on the UART0. It is assumed that the hardware initialization 
*  function setups the UART. Also, it is assumed the lower hardware
*  layer will call application level-hook functions described in this file
*  to indicate external events. All function in this file are TASK safe.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                            File-level Constants
*********************************************************************************************************
*/
#define DEC_BUFFER_LENGTH 12

/*
*********************************************************************************************************
*                                            File-level VARIABLES
*********************************************************************************************************
*/
static OS_EVENT *TxReadySemp;
static OS_EVENT *RxReadySemp;
static OS_EVENT *SerialResourceMutex;
static char NumberBuffer[DEC_BUFFER_LENGTH];

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitSerialInterface
*
* Description: This function is called by the OS to setup a application layer
*              of the UART HAL. In this function two semaphores and a mutex are
*              initialized. The mutex is used to protect the resource, in this case
*              access to the uart. The semaphores are used to keep track of external
*              events that are reported by the lower hardware layer.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void InitSerialInterface()
{
    INT8U err;

    OSMutexCreate(0, &err);
    RxReadySemp= OSSemCreate(0);       /* We have to wait for first byte to come in before reading the buffer */
    TxReadySemp= OSSemCreate(0);	    /* We can start sending a byte out rightaway */	
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitSerialInterface
*
* Description: This function is called to grab the UART device. This function is blocking.
*              This function is task-safe. After this funtion return it is safe to write/read
*             data to the hardware.               
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void GetSerialDevice ( void )
{
	INT8U err;
	OSMutexPend(SerialResourceMutex, 0, &err);     /* Get the device */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        ReleaseSerialDevice
*
* Description: This function is called to release the UART device.                
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void ReleaseSerialDevice ( void )
{
	OSMutexPost(SerialResourceMutex); /* Release the device */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSerialByteTxSent
*
* Description: This is a hook-function that will be called only by lower hardware layer 
*              to notify this OS-layer of TX hardware events. This function should only be
*              called during interrupt context. This function is kept simple to avoid
*              keeping interrupts turned OFF for an extended time.                
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void OSSerialByteTxSent( void )
{
	OSSemPost(TxReadySemp);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSerialByteRxRecvd
*
* Description: This is a hook-function that will be called only by lower hardware layer 
*              to notify this OS-layer of RX hardware events. This function should only be
*              called during interrupt context. This function is kept simple to avoid
*              keeping interrupts turned OFF for an extended time.                
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void OSSerialByteRxRecvd()
{
    OSSemPost(RxReadySemp);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialPutChar
*
* Description: This functions sends a single byte to the UART. The process of actually writing 
*              a byte out to the hardware is abstracted away to the hardware layer. This function is 
*              task-safe. The function writes a byte out in following steps, 1) Get the serial device, 
*              2) make sure the hardware is not busy. 3) Write the byte to the hardware. 4) release the
*              resource.               
*
* Arguments  : data - A byte of data to be written out.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void SerialPutChar( char data)
{
    INT8U err; 
    GetSerialDevice();			          /* Get the device */
    OSSemPend(TxReadySemp, 0, &err);    /* Wait for the last byte to be transmitted */
    UART0SendByte(data);		          /* Write this byte out now */
    ReleaseSerialDevice();		          /* Release the resource */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialPutString
*
* Description: This functions writes a string to the UART. Each byte is transmitted individually. 
*              The process of actually writing a byte out to the hardware is abstracted away to
*              the hardware layer. The function writes a byte out in following steps, 1) Get the serial  
*              device,  2) make sure the hardware is not busy. 3) Write the byte to the hardware. 4) release 
*              the resource.               
*
* Arguments  : msg - a pointe to a null-terminated string.
*
* Returns    : none
*
* Note(s)    : This function is task-safe.
*********************************************************************************************************
*/
void SerialPutString( const char *msg )
{
   INT8U err; 
   GetSerialDevice();			  /* Get the device */ 

   /* While we have character we write them out */
   while (*msg != 0)
   {
      OSSemPend(TxReadySemp, 0, &err);  /* Wait for the last byte to be transmitted */
      UART0SendByte(*msg++);		       /* Write this byte out now */
   }

   ReleaseSerialDevice();		          /* Release the resource */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialPutDecNum
*
* Description: This functions writes a decimal number to the UART. First the number is translated into
*              a ascii string and stored into a local buffer. Then the buffer is transmitted in reverse 
*              order to make sure the number is printed properly.              
*
* Arguments  : number - a 32 bit number is passed. 
*
* Returns    : none
*
* Note(s)    : This function is task-safe.
*********************************************************************************************************
*/
void SerialPutDecNum( INT32U number ){   
   INT16U len = 0, index = 0;
   INT32U quotVal = number;

   while(1)
   {
	  index  = quotVal % 10;
	  quotVal = quotVal / 10;
	  NumberBuffer[len] = ('0' + index);

      if ( (quotVal > 0) && (len < DEC_BUFFER_LENGTH) )
        len++;
      else 
	    break;
   }
 
   while(1)                                 /* We put the string on the serial backwards */
   {                            
        SerialPutChar(NumberBuffer[len]);   /* Null terminate the return string */
        
        if (len == 0)                
           break;
        
        len--;
   }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialGetChar
*
* Description: This functions get a byte from the UART. This function is blocking. Receiving a byte 
*              consists of follwing steps, 1) Get the uart device. 2) wait for Rx uart event. 3) get
*              the received byte. 4) release the device. 5)return the just received character.
*
* Arguments  : none 
*
* Returns    : char -- received character.
*
* Note(s)    : This function is task-safe.
*********************************************************************************************************
*/
char SerialGetChar( void )
{
    INT8U err; 
    char returnByte = 0; 

    GetSerialDevice();			  /* Get the device */
    OSSemPend(RxReadySemp, 0, &err);  /* Wait for some data to show up in the buffer */
    returnByte = UART0GetByte();  /* Get the character */
    ReleaseSerialDevice();		  /* Release the resource */
    return returnByte; 	
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialGetCharTimeout
*
* Description: This functions get a byte from the UART but only wait a certain time before giving up.
*              This function is blocking. Receiving a byte with timeout consists of follwing steps, 
*              1) Get the uart device. 2) wait for Rx uart event. If there is no event UART and timeout 
*              expires, a error message is returned. 4) if a uart event happens before timeout the byte
*              is read from uart rx 4) release the device. 5)return the just received character.
*
* Arguments  : none 
*
* Returns    : char -- received character.
*
* Note(s)    : This function is task-safe.
*********************************************************************************************************
*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SerialPutDecNum
*
* Description: This functions writes a decimal number to the UART. First the number is translated into
*              a ascii string and stored into a local buffer. Then the buffer is transmitted in reverse 
*              order to make sure the number is printed properly.              
*
* Arguments  : ch - a pointer wher  
*
* Returns    : none
*
* Note(s)    : This function is task-safe.
*********************************************************************************************************
*/
INT8U SerialGetCharTimeout(char *ch, INT16U timeout){
    INT8U err = INTIME; 
    OSTimeDlyHMSM(0, 0, 1, 0);
    
    GetSerialDevice();		                /* Get the device */
    if (err == OS_TIMEOUT)
    {
	    err = TIMEDOUT; 
    }
    else  
    {
	    err = INTIME; 
	    *ch = UART0GetByte();       /* Get the character */       
    }  
    
    ReleaseSerialDevice();	       /* Release the resource */
    return err; 	
}
