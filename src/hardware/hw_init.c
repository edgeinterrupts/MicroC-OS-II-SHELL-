/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_init.h
*  Author: Omer Farooq
*  Version: 2.0
*  Date:   20-January-2005
*
*  Board initialization code.
*
*  NOTE: Some comments in this file are from original code. 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <hw_init.h>
#include <hw_lcd.h>
#include <hw_uart0.h>

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitHFO
*
* Description: This function turns ON the high Frequency Oscillator XT2(7.3728MHz crystal)
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void InitHFO()
{
   int count = 0, timeout = 0;

   /* Turn on HFO XT2  */
   BCSCTL1 &= ~(XT2OFF);  

   while(timeout < HFO_TIMEOUT)
   {
      IFG1 &= ~(OFIFG);

      /* Basic wait until countdown loop      
       * apparently, directly polling the 
       * OSCFault flag is not recommended
       */
      for(count = 0; count < 0xff; count++)
         ;

      /* Incrment the timeout */
      timeout++;

      /* OSCFault flag still set? If yes (1) then clear flag
       * and wait some more otherwise no (0) XT2 is up so return.
       */
      if (!(OFIFG & IFG1))
         return; 
   }

   /* If made it here then there was an error 
   * XT2 failed, shut it down and Clear OSCFault flag.
   */  
   BCSCTL1 |= XT2OFF;
   IFG1    &= ~(OFIFG);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitPorts
*
* Description: Initialize the board ports
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static void InitPorts()
{
   /* Setup the port 1 */
   P1SEL = HW_P1SEL; 
   P1DIR = HW_P1DIR; 
   P1OUT = HW_P1OUT; 
   P1IE  = HW_P1IE; 
   P1IES = HW_P1IES; 

   /* Setup the port 2 */                   
   P2SEL = HW_P2SEL; 
   P2DIR = HW_P2DIR; 
   P2OUT = HW_P2OUT; 
   P2IE  = HW_P2IE; 
   P2IES = HW_P2IES; 

   /* Setup the port 3 */ 
   P3SEL = HW_P3SEL;
   P3DIR = HW_P3DIR; 
   P3OUT = HW_P3OUT; 

   /* Setup the port 4 */                   
   P4SEL = HW_P4SEL; 
   P4DIR = HW_P4DIR; 
   P4OUT = HW_P4OUT;

   /* Setup the port 5 */                    
   P5SEL = HW_P5SEL; 
   P5DIR = HW_P5DIR; 
   P5OUT = HW_P5OUT; 

  /* Setup the port 6 */                   
   P6SEL = HW_P6SEL;
   P6DIR = HW_P6DIR; 
   P6OUT = HW_P6OUT; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        InitHW
*
* Description: This function initializes the processor and ports.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void InitHW()
{ 
   /* Initialize ports */
   InitPorts();

   /* Turn on the LED to indicate the init sequence */
   P4OUT |= LED1;

   /* Each stage lights a display segment */
   P2OUT |= DISP_A; 
   P3OUT &= ~(DISP1_EN|DISP2_EN);

   /* Initilaize the HFO */
   InitHFO();

   /* There was an error if HFO is not on and stable */
   if (BCSCTL1 & XT2OFF)
   {
       /* HFO stage error */
       P2OUT |= DISP_DP;
   }
   else
   {
       /* Source MCLK & SMCLK from it XT2 end HFO stage */
       BCSCTL2 |= INIT_BCSCTL2;
       P2OUT |= DISP_B;
   }

   /* Wait till the 5v are stabilized before initializing the LCD. */
   delay(LCD_PS_WAIT);

   /* Initilize the LCD */
   InitLCD();

   /* Add a stage light on segment display */
   P2OUT |= DISP_D; 

   /* Post Initial message on the LCD */
   WriteStringToLCD("OFAROOQ Final PRJ");
   
   /* Initialize the UART0  */
   InitMSP430UART0();
   
   /* Turn OFF the LED to indicate the init sequence */
   P4OUT &= ~(LED1); 

   /* Write 'init complete' message on LCD */
   SetLCDCursorPosition(LCD_LINE1);
   WriteStringToLCD("   OS SHELL   ");

   return;
}
