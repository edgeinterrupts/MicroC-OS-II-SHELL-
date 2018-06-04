#ifndef ENABLE_SHELL_EN 
#ifndef APP_SHELL_SERICES_H
#define APP_SHELL_SERICES_H
 
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
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                             CONSTANTS
*********************************************************************************************************
*/

#define  OK                    0
#define  ILLEGAL_CMD           1
#define  ILLEGAL_NUMBER_ARGS   2
#define  CMD_TOO_LONG          3 
#define  MAX_RETURN_VALUE      4
#define  MAX_ERR_MSG_LENGTH    16

static const char ErrorMsgs[MAX_RETURN_VALUE][MAX_ERR_MSG_LENGTH] =
  { "OK", "Invalid CMD", "Invalid Args", "Input too Long"};

/*
*********************************************************************************************************
*                                             FUCTION PROTOTYPES
*********************************************************************************************************
*/
char   *mystrtok(char* str, char delim);
INT8U  GetTCB( char *cmdArgs ); 
INT8U  GetMCB( char *cmdArgs ); 
INT8U  GetOFB( char *cmdArgs ); 
INT8U  GetOEB( char *cmdArgs );
INT8U  GetOSInfo( char *cmdArgs );
INT8U  GetOSStats( char *cmdArgs );
INT8U  GetTime( char *cmdArgs ); 
INT8U  SetTime( char *cmdArgs ); 
INT8U  TimeDelay( char *cmdArgs );
INT8U  OSTaskCreateS( char *cmdArgs );
INT8U  OSTaskDeleteS( char *cmdArgs );
INT8U  OSTaskSuspendS( char *cmdArgs );
INT8U  OSTaskResumeS( char *cmdArgs );
INT8U  OSSemphoreCreateS( char *cmdArgs );
INT8U  OSSemphoreDeleteS( char *cmdArgs );
INT8U  OSSemphorePostS( char *cmdArgs );
INT8U  OSSemphorePendS( char *cmdArgs );
INT8U  OSMutexCreateS( char *cmdArgs );
INT8U  OSMutexDeleteS( char *cmdArgs );
INT8U  OSMutexPostS( char *cmdArgs );
INT8U  OSMutexPendS( char *cmdArgs );
INT8U  OSFlagCreateS( char *cmdArgs );
INT8U  OSFLagDeleteS( char *cmdArgs );
INT8U  OSFlagPostS( char *cmdArgs );
INT8U  OSFlagPendS( char *cmdArgs );
INT8U  OSMailBoxCreateS( char *cmdArgs );
INT8U  OSMailBoxDeleteS( char *cmdArgs ); 
INT8U  OSMailBoxPendS( char *cmdArgs ); 
INT8U  OSMailBoxPostS( char *cmdArgs ); 
INT8U  OSMailBoxPostOpts( char *cmdArgs ); 
INT8U  OSQueueCreateS( char *cmdArgs ); 
INT8U  OSQueueDeleteS( char *cmdArgs ); 
INT8U  OSQueueAcceptS( char *cmdArgs ); 
INT8U  OSQueuePostS( char *cmdArgs ); 
INT8U  OSMemoryPendS( char *cmdArgs ); 
INT8U  OSMemoryCreateS( char *cmdArgs ); 
INT8U  OSMemoryGetS( char *cmdArgs ); 
INT8U  OSMemoryPutS( char *cmdArgs ); 
INT8U  WaitForEvent( char *cmdArgs );
INT8U  GetOMQ( char *cmdArgs);

#endif /* APP_SHELL_SERICES_H */
#endif /* ENABLE_SHELL_EN  */
