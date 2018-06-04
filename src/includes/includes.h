/*
*********************************************************************************************************
* File    : includes.H
* By      : Omer Farooq
* Version : V2.77
*
*  Master include file.
* 
*  This file originally came with MSP430 port. I have cleaned it up 
*  and removed stuff that was unused or is not needed anymore.  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <signal.h>
#include  <ctype.h>
#include  <ucos_ii.h>
#include  <io.h>
#include  <iomacros.h>
#include  <string.h>
#include  <stdlib.h>

/*
*********************************************************************************************************
*                                             CONSTANTS AND MACROS
*********************************************************************************************************
*/

/* 
   To disable the shell, following two things must be done. 
   1) Uncomment the following code line (#define ENABLE_SHELL_EN)
   2) In shell.c, on top of the file, #if should be set 0. 

   To Enable the shell, following two things must be done. 
   1) Comment the following code line (#define ENABLE_SHELL_EN)
   2) In shell.c, on top of the file, #if should be set 1.   
*/
//#define ENABLE_SHELL_EN   /* Enable (1) or Disable (0) argument checking  */
#define MAX_SHELL_TASKS      2
#define MAX_SHELL_SEMPS      2
#define MAX_SHELL_MUTEXS     2
#define MAX_SHELL_FLAGGRPS   2
#define MAX_SHELL_MAILBOXS   2
#define MAX_SHELL_QUEUES     2
#define MAX_SHELL_MEMPARTS   1

/* Shell specific files */
#ifndef   ENABLE_SHELL_EN
#include  <app_shell.h>
#include  <app_serialInterface.h>
#include  <app_xmodem.h>
#endif

/* Board specific include files */
#include    "hw_init.h"
#include    "hw_lcd.h"
#include    "hw_uart0.h"

