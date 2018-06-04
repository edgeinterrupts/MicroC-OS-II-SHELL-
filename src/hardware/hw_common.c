/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_common.c
*  Author: Omer Farooq
*  Date:   14-December-05
*
*  C file with some common functions.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "hw_init.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                        delay
*
* Description: This function is a simple delay execution function.
*
* Arguments  : delayCount - number of seconds to delay execution
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
__attribute__ ((naked)) void delay(unsigned int delayCount) 
{
    /* First arg comes in register R15
     * the loop uses 3 cycles per round
     * the eight cycles come from the call overhead and ret
     */
    asm("_wait_: dec %0\n  jnz _wait_\n ret" :: "r" (delayCount));
}
