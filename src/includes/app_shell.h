#ifndef ENABLE_SHELL_EN 
#ifndef APP_SHELL_H
#define APP_SHELL_H

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
*                                             FUNCTION PROTOTYPES 
*********************************************************************************************************
*/
void OSShellTaskStart(void *pdata);

#endif /* APP_SHELL_H */
#endif /* ENABLE_SHELL */
