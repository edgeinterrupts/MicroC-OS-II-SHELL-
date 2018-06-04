/*$PAGE*/
/*
*********************************************************************************************************
*  File: app_main.c
*  Author: Omer Farooq
*  Date:   20-January-05
*  
*  Startup application level code.
*
*  NOTE: Some comments in this file are from the original code (app.c) 
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
*                                             MACROS 
*********************************************************************************************************
*/
#define  TOTAL_USER_TASKS        2   /* Define each task stack size as 64 bytes */
#define  TASK_STK_SIZE          64  /* Each task's stack size */
#define  TASK_SHELL_STK_SIZE   128  /* Each task's stack size */

/*
*********************************************************************************************************
*                                             LOCAL VARIABLES 
*********************************************************************************************************
*/
OS_STK   TaskStartStk [TASK_STK_SIZE];		/* Stack for the Task */ 

#ifndef ENABLE_SHELL_EN 
OS_STK   TaskOSShellStk [TASK_SHELL_STK_SIZE];		/* Stack for the Task */
#endif

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES 
*********************************************************************************************************
*/
void TaskStart(void *data);             /* Function prototypes of Startup task  */

/*$PAGE*/
/*
*********************************************************************************************************
*                                        main
*
* Description: This function is entry point to user code after reset code.
*
* Arguments  : none (2400, 8N1)
*
* Returns    : int
*
* Note(s)    : 
*********************************************************************************************************
*/
int main(void){
   WDTCTL = WDTPW + WDTHOLD;          /* Disable the watchdog timer   */           

   InitHW();					            /* Initialize the hardware */
   TACTL    = TASSEL1 + TACLR;         /* Configure TIMERA for the system Tick source. */
   TACTL   |= 0x00C0;                  /* Clear the Timer and set SMCLK (7.38Mhz) as the source. */ 
   TACCTL0  = CCIE;                    /* Input divider is /8.         */                           
   TACCR0   = 9216;                    /* Enable the TACCR0 interrupt. */                           
                                       /* Load the TACCR0 register.    */   
   OSInit();                           /* Initialize uC/OS-II  */
 
   OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 1);  /* Create a startup task */

   InitSerialInterface();       /* Initialize the Serial Interface */

#ifndef ENABLE_SHELL_EN
   OSTaskCreate(OSShellTaskStart, (void *)0,	/* Create a OS Shell Task task */
      &TaskOSShellStk[TASK_SHELL_STK_SIZE - 1], 2);  
#endif

   OSStart();				  /* Start multitasking   */ 
   return(0);				  /* we should never get here */ 
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        TaskStart
*
* Description: This is startup task function.
*
* Arguments  : pdata
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
void TaskStart(void *pdata){
   char count = 0; 

   pdata  = pdata;         /* Prevent compiler warning */
   TACTL |= MC1;           /* Start the Timer in Continuous mode. */

#if (OS_TASK_STAT_EN > 0)
   OSStatInit();           /* Initialize uC/OS-II's statistics */ 
#endif
  
   /* Initial value on the 7 Segment LCD */
   P2OUT = DISP_DP;

   /* Light up the display every second forever */
   while (1) 
   {
      if (count == 0)
      {   
         P3OUT &= ~DISP1_EN;
         count++;
      }
      else
      {   
         P3OUT |= DISP1_EN; 
         count--;
      }

	   OSCtxSwCtr = 0;       

      /* Wait one second */
      OSTimeDlyHMSM(0, 0, 1, 0);
   }
}
