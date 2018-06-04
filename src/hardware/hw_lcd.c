/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_lcd.c
*  Author: Omer Farooq
*  Date:  20-January-05
*
*  LCD driver for MSP430F149 dev. board.
*
*  NOTE: Some comments in this file are from original code. 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "hw_init.h"
#include "hw_lcd.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                        PollBusyFlags
*
* Description: This function polls the LCD Busy Flag. Private function.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void PollBusyFlag()
{
   /* Release buffer */
   P5OUT = LCD_OUT2IN;

   /* Enter read mode */
   P4OUT |= LCD_RW;

   /* Busy flag/address register */
   P4OUT &= ~LCD_RS;

   /* Wait two instructions */
   nop();
   nop();

   /* Enable read */
   P4OUT |= LCD_EN;

   /* Poll pin (although interrupt operation is possible) */
   while ((LCD_BUSY & P1IN) == 0)
       ;

   /* Disable read */
   P4OUT &= ~LCD_EN;
   nop();
   nop();

   /* Exit read mode */
   P4OUT &= ~LCD_RW;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TransferDataToLCD
*
* Description: Helper function that writes a command on the LCD screen.  Private function.
*
* Arguments  : data -- data that needs to be sent to the LCD controller.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void TransferDataToLCD(unsigned int data)
{
   /* Put content on the LCD pins */
   P5OUT = data;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
    
   /* Check for LCD status = ready */
   PollBusyFlag();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitLCD
*
* Description: Initializes the LCD
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void InitLCD()
{
   /* This is the datasheet software initalization procedure
    * enable is active high.
    */
   P4OUT &= ~(LCD_RS|LCD_RW|LCD_EN);
    
   /* Wait */
   delay(LCD_INIT_WAIT0);

   /* SW init step 1	-	wait >4.1ms */
   P5OUT = LCD_INIT; 
   nop();
   P4OUT |= LCD_EN;
   nop(); /* PW_EH min=450ns @ MCLK 7.3728 MHz ~= 4 Instr Cycles; nop=2 */
   nop(); 
   P4OUT &= ~(LCD_EN);
   delay(LCD_INIT_WAIT1);
    
   /* SW init step 2 - wait >100us */
   P5OUT = LCD_INIT;
   nop(); 
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   delay(LCD_INIT_WAIT0);
   P2OUT |= DISP_C;

   /* SW init step 3 - after command is written, BF (Busy Flag can be checked) */
   P5OUT = LCD_INIT;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);

   /* Check busy flag... accessed through open collector inverter 0 = busy */
   PollBusyFlag();

   /* Do function set (8/4 bit interface, number of display lines, display font) */
   P5OUT = LCD_FUNSET; 
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   PollBusyFlag();

   /* Do display off */
   P5OUT = LCD_OFF;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   PollBusyFlag();

   /* Do clear display */
   P5OUT = LCD_CLR;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   PollBusyFlag();

   /* Do entry mode set */
   P5OUT = LCD_EMS;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   PollBusyFlag();

   /* Do display on */
   P5OUT = LCD_ON;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~(LCD_EN);
   PollBusyFlag();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SetDisplyDDRAMPosition
*
* Description: Set LCD cursor position.
*
* Arguments  : position-- position in the DDRAM
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void SetDisplyDDRAMPosition(unsigned int position)
{
   P4OUT &= ~(LCD_RW|LCD_RS);

   /* Mask the position value */
   position |= 0x80;
 
   TransferDataToLCD(position); 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        ClearLCD
*
* Description: This function clears the LCD. 
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void ClearLCD()
{
   TransferDataToLCD(LCD_CLR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SetLCDCursorPosition
*
* Description: Sets the LCD position to Line number
*
* Arguments  : position-- Where the cursor should be placed.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void SetLCDCursorPosition(unsigned int position)
{
   if (position == LCD_HOME)
   {
      TransferDataToLCD(LCD_HOME);
   }
   else
   {
      position |= 0x80;
      TransferDataToLCD(position);          
   }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        WriteCharToLCD
*
* Description: Write a zero (null) terminated string to the LCD.
*
* Arguments  : character -- a character that will be sent to the LCD
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void WriteCharToLCD(char character)
{
   /* Write data to DRAM */
   P4OUT &= ~LCD_RW;
   P4OUT |= LCD_RS;
   P5OUT = character;
   nop();
   P4OUT |= LCD_EN;
   nop();
   nop();
   P4OUT &= ~LCD_EN;
   PollBusyFlag();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        WriteStringToLCD
*
* Description: Write a zero (null) terminated string to the LCD.
*
* Arguments  : a pointer to a zero-terminated (NULL) string.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void WriteStringToLCD(const char* msg)
{
   /* While we have character we writing it out */
   while (*msg != 0)
      WriteCharToLCD(*msg++);
}
