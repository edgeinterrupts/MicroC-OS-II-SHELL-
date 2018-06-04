#ifndef ENABLE_SHELL_EN

/*$PAGE*/
/*
********************************************************************************************************* 
*  File: app_shellservices.c
*  Author: Omer Farooq
*  Date: 20-February-05
*
*  This file provides contains function that service shell commands. 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <includes.h>
#include <app_shellServices.h>

/*
*********************************************************************************************************
*                                             FILE LEVEL GLOBALS FILES
*********************************************************************************************************
*/

/* Shell Task stuff */
#define SHELL_TSK_SIZE    16
static INT8U TaskCnt = 0;
static INT8U TaskPrio[MAX_SHELL_TASKS] = {0xFF};
static OS_STK SHELL_TSK_STK[MAX_SHELL_TASKS][SHELL_TSK_SIZE];	/* Stack for the Task */

/* SHELL Sempahore stuff */
static INT8U SempCnt = 0;
static OS_EVENT *ShellSempList[MAX_SHELL_SEMPS] = {0};

/* SHELL Mutex stuff */
static INT8U MutexCnt = 0;
static OS_EVENT *ShellMutexList[MAX_SHELL_MUTEXS] = {0};

/* SHELL Flaggrp stuff */
static INT8U FlagGrpCnt = 0;
static OS_FLAG_GRP *ShellFlagGroupList[MAX_SHELL_FLAGGRPS] = {0};

/* SHELL Mailbox stuff */
static INT8U MailboxCnt = 0;
static OS_EVENT *ShellMailBoxList[MAX_SHELL_MAILBOXS] = {0};

/* SHELL Flag group stuff */
#define QUEUE_SIZE 4
static INT8U QueueCnt = 0;
static void *ShellQueueBuffers[MAX_SHELL_QUEUES][QUEUE_SIZE];
static OS_EVENT *ShellQueueList[MAX_SHELL_QUEUES] = {0};

/* SHELL Memory parts */
#define BLK_NUMBER 2
#define BLK_SIZE   4
static INT8U MemPartCount = 0;
static OS_MEM *ShellMemParts[MAX_SHELL_MEMPARTS];
INT8U MemPart[BLK_NUMBER][BLK_SIZE];

/*$PAGE*/
/*
*********************************************************************************************************
*                                        ShellTasksFunction
*
* Description: This is a dummy function that gets called when the shell creates a task.
*              
* Arguments  : pdata - value is ignored.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
void ShellTasksFunction(void *pdata){
    while(1)
        ;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetTaskONEParm
*
* Description: This is a helper function used to extract arguments from a string. 
*
* Arguments  : cmd - a null-terminated string.
*              val - a pointer to where the extracted the value is placed.
*
* Returns    : a 8-bit error code.
*
* Note(s)    :
*********************************************************************************************************
*/
static INT8U GetTaskONEParm(char* cmd, INT8U *Val)
{
   char *token;
   INT8U err = OK;
   INT8U tokenCount  = 0;         
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			*Val = atoi ( token ); 
            break;

         default:
            err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 1)                  /* If there were enough arguments then return */
        err  = ILLEGAL_NUMBER_ARGS;

   return err;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        mystrtok
*
* Description: Lite version of strtok.
*
* Arguments  : str - a pointer null terminated string.
*              delim  - a ASCII character that is delimter
*
* Returns    : a pointer to a token.
*
* Note(s)    :
*********************************************************************************************************
*/
char *mystrtok(char* str, char delim)
{
   static char *ptr;
   char *tmp;
   char *result;
   
   /* If string specified is not null, 
    *  then set this string as our default
    *  working string. 
    */
   if (str)
      ptr = str;

   /* Eat up all delimiter at the start of a string */
   while (*ptr == delim)
      ptr++;

   /* Is this an empty string? */
   if (*ptr == '\0')
      return NULL;

   /* Now we know we have a valid character so lets mark this point as start */
   tmp = ptr;
   while (*tmp && *tmp != delim)
      tmp++;

   /* either we have found the delimeter or null. 
    * If its a delimiter then set the pointer after the 
    * delimiter for next time. 
    */
   result = ptr;
   if (*tmp)
      ptr = tmp+1;
   else
      ptr = tmp;

   *tmp = '\0';
   return result;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetTCB
*
* Description: This function prints out the complete task list.
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetTCB( char *cmdArgs )
{

   char *token;
   INT8U    i;
   OS_TCB  *ptcbPtr;
   ptcbPtr = &OSTCBTbl[0];
   INT8U err = ILLEGAL_NUMBER_ARGS;
   
   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token == NULL)          /* There should no more tokens */
   {
      err  = OK;
      SerialPutString("Task Control Blocks\r\n");

       SerialPutString("\r\n There are ");
       SerialPutDecNum((OS_MAX_TASKS + OS_N_SYS_TASKS - 1)); 
       SerialPutString(" task allocated\r\n"); 
         
            for (i = 0; i < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1); i++) {  /* Init. list of free TCBs            */
        
                    SerialPutDecNum((i+1));
                    SerialPutString("\r\n OSTCBStkSize: ");
                    SerialPutDecNum(ptcbPtr->OSTCBStkSize);
                    SerialPutString("\r\n OSTCBOpt: ");
                    SerialPutDecNum(ptcbPtr->OSTCBOpt);
                    SerialPutString("\r\n OSTCBNext: ");
                    SerialPutDecNum((INT16U)ptcbPtr->OSTCBNext);
                    SerialPutString("\r\n OSTCBPrev: ");            
                    SerialPutDecNum((INT16U)ptcbPtr->OSTCBPrev);
                    SerialPutString("\r\n OSTCBEventPtr: ");
                    SerialPutDecNum((INT16U) ptcbPtr->OSTCBEventPtr);
                    SerialPutString("\r\n OSTCBMsg: ");
                    SerialPutDecNum((INT16U) ptcbPtr->OSTCBMsg);
                    SerialPutString("\r\n OSTCBId: ");
                    SerialPutDecNum(ptcbPtr->OSTCBId);
                    SerialPutString("\r\n OSTCBDly: ");
                    SerialPutDecNum(ptcbPtr->OSTCBDly);
                    SerialPutString("\r\n OSTCBStat: ");
                    SerialPutDecNum(ptcbPtr->OSTCBStat);
                    SerialPutString("\r\n OSTCBPrio: ");
                    SerialPutDecNum(ptcbPtr->OSTCBPrio);
                    SerialPutString("\r\n OSTCBX: ");
                    SerialPutDecNum(ptcbPtr->OSTCBX);
                    SerialPutString("\r\n OSTCBY: ");
                    SerialPutDecNum(ptcbPtr->OSTCBY);
                    SerialPutString("\r\n OSTCBBitx: ");
                    SerialPutDecNum(ptcbPtr->OSTCBBitX);
                    SerialPutString("\r\n OSTCBBitY: ");
                    SerialPutDecNum(ptcbPtr->OSTCBBitY);
                    SerialPutString("\r\n OSTCBDelReq: ");
                    SerialPutDecNum(ptcbPtr->OSTCBDelReq);
                    SerialPutString("\r\n");
                    ptcbPtr++;
            }
   }

   return err;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetOFB
*
* Description: This function prints out the complete flag list.
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetOFB( char *cmdArgs)
{
#if (OS_FLAG_EN > 0)
    OS_FLAG_GRP *pgrp1;
#endif 
   char *token;
   INT8U     i;
   
    
   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token != NULL)          /* There should no more tokens */
       return ILLEGAL_NUMBER_ARGS;

    #if (OS_MAX_FLAGS > 0) && (OS_FLAG_EN > 0) 
       pgrp1 = (OS_FLAG_GRP *)&OSFlagTbl[0];
       SerialPutString("\r\n There are ");
       SerialPutDecNum(OS_MAX_FLAGS); 
       SerialPutString(" flag groups allocated\r\n"); 
       for (i = 0; i < OS_MAX_FLAGS; i++){
                SerialPutDecNum((i+1));
                SerialPutString("\r\n OSFlagType: ");
                SerialPutDecNum(pgrp1->OSFlagType);

                SerialPutString("\r\n OSFlagWaitList: ");
                SerialPutDecNum((INT16U)pgrp1->OSFlagWaitList);

                SerialPutString("\r\n OSFlagFlags: ");
                SerialPutDecNum((INT16U)pgrp1->OSFlagFlags);

                SerialPutString("\r\n");
                pgrp1++;
        }

    
    #else
        SerialPutString("\r\n There are no OS FLAG GRP in the system");
    #endif


	return OK;    
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetMCB
*
* Description: This function prints out the complete memory control blocks.
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetMCB( char *cmdArgs)
{
#if (OS_MEM_EN > 0)
    OS_MEM  *pmem;
#endif 
   
   char *token;
   INT8U     i;  
    
   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token != NULL)          /* There should no more tokens */
       return ILLEGAL_NUMBER_ARGS;

    #if (OS_MAX_MEM_PART > 0) && (OS_MEM_EN > 0) 
       SerialPutString("\r\n There are ");
       SerialPutDecNum(OS_MAX_MEM_PART); 
       SerialPutString(" memory part allocated\r\n"); 
       pmem = (OS_MEM *)&OSMemTbl[0];                        /* Point to memory control block (MCB)       */
       for (i = 0; i < OS_MAX_MEM_PART; i++){
                SerialPutDecNum((i+1));
                SerialPutString("\r\n OSMemFreeList: ");
                SerialPutDecNum((INT16U) pmem->OSMemFreeList);
                SerialPutString("\r\n");
                pmem++;
        }    
    #else
        SerialPutString("\r\n There are no OS Memory GRP in the system");
    #endif
	return OK;  
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetOMQ
*
* Description: This function prints out the complete queue control blocks.
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetOMQ( char *cmdArgs)
{
   char *token;
   INT8U     i;
   OS_FLAG_GRP *pgrp1 = (OS_FLAG_GRP *)&OSFlagTbl[0];
#if (OS_Q_EN > 0)
    OS_Q   *pq1;
#endif
    
   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token != NULL)          /* There should no more tokens */
       return ILLEGAL_NUMBER_ARGS;

    #if (OS_MAX_FLAGS > 0) && (OS_Q_EN > 0) 
       pq1 = &OSQTbl[0];
       SerialPutString("\r\n There are ");
       SerialPutDecNum(OS_MAX_QS); 
       SerialPutString(" QUEUE part allocated\r\n"); 
       for (i = 0; i < OS_MAX_FLAGS; i++){
                SerialPutDecNum((i+1));
                SerialPutString("\r\n OSQPtr: ");
                SerialPutDecNum((INT16U)pq1->OSQPtr);
                SerialPutString("\r\n");
                pgrp1++;
        }    
    #else
        SerialPutString("\r\n There are no OS Memory GRP in the system");
    #endif
	return OK;  
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetOEB
*
* Description: This function prints out the complete event control block.
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetOEB( char *cmdArgs)
{
   char *token;
   INT16U     i;

   OS_EVENT  *pevent1;
    
   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token != NULL)          /* There should no more tokens */
       return ILLEGAL_NUMBER_ARGS;

#if (OS_MAX_EVENTS > 0)
       pevent1 = &OSEventTbl[0];
       SerialPutString("\r\n There are ");
       SerialPutDecNum(OS_MAX_EVENTS); 
       SerialPutString(" event flags groups allocated\r\n"); 

       for (i = 0; i < OS_MAX_EVENTS; i++) {             /* Init. list of free EVENT control blocks */

                SerialPutDecNum((i+1));
                SerialPutString("\r\n OSEventType: ");
                SerialPutDecNum(pevent1->OSEventType);

                SerialPutString("\r\n OSEventPtr: ");
                SerialPutDecNum((INT16U)pevent1->OSEventPtr);

                SerialPutString("\r\n OSEventCnt: ");
                SerialPutDecNum((INT16U)pevent1->OSEventCnt);

                SerialPutString("\r\n OSEventGrp: ");
                SerialPutDecNum((INT16U)pevent1->OSEventGrp);

                SerialPutString("\r\n");
                pevent1++;
        }
#else
   SerialPutString("\r\n OS_MAX_EVENTS Disabled");
#endif
    
    return OK;
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        TimeDelay
*
* Description: This function supports the timedelay command. This function needs a time parameters. 
*
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U TimeDelay( char *cmdArgs)
{ 
   INT8U hours = 0;
   INT8U minutes = 0;
   INT8U seconds = 0;
   INT16U milli = 0;
   INT8U err = OK;
   INT8U tokenCount  = 0;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			hours = atoi ( token ); 
            break;

         case 1: 
			minutes = atoi ( token ); 
            break;

         case 2: 
			seconds = atoi ( token ); 
            break;

         case 3:
			milli = atoi ( token ); 
            break;

	     default:
            err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if ( (err == OK) && (tokenCount == 4) )                  /* If there were enough arguments then return */
   {        
        err = OSTimeDlyHMSM(hours, minutes, seconds, milli);
        if (err != OS_NO_ERR)
            SerialPutString("OSTimeDlyHMSM failed\r\n");  

        err = OK;   
   }
   
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetTime
*
* Description: This function gets system tick count. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetTime( char *cmdArgs){ 
   char *token;
   INT8U err = OK;
   
   token = mystrtok(NULL, ' '); /* Process the string */
  
   if ( token != NULL)          /* If there no more tokens then get out */
        err  = ILLEGAL_NUMBER_ARGS;
   else
   {
        SerialPutString("Tick Count: ");
        SerialPutDecNum(OSTimeGet());
        SerialPutString("\r\n");        
   }
   
   return err;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        SetTime
*
* Description: This function sets system tick count. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U SetTime( char *cmdArgs){ 
   char *token;
   INT8U err = OK;
   INT32U tickCount = 0;
   INT8U tokenCount  = 0;  

   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			tickCount = atoi ( token ); 
            break;

         default:
            err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 1)                  /* If there were enough arguments then return */
        err  = ILLEGAL_NUMBER_ARGS;
   else
   {
        OSTimeSet(tickCount);
        GetTime( 0 );
   }
   
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetOSInfo
*
* Description: This function sets system info 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetOSInfo( char *cmdArgs){
   char *token;
   INT8U err = OK;
   INT16U os_version = 0;
   
   token = mystrtok(NULL, ' '); /* Process the string */
  
   if ( token != NULL)          /* If there no more tokens then get out */
        err  = ILLEGAL_NUMBER_ARGS;
   else
   {
       os_version = OSVersion(); 
       SerialPutString("Micro OS-II Version: ");    
       SerialPutDecNum(os_version);
       SerialPutString("\r\n");
   }

   return err;
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        GetOSStats
*
* Description: This function sets system statistics 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U GetOSStats( char *cmdArgs){ 
#if (OS_TASK_STAT_EN > 0)
   char *token;
   INT8U err = OK;

   token = mystrtok(NULL, ' '); /* Process the string */
   if ( token != NULL)          /* If there no more tokens then get out */
   {
        err  = ILLEGAL_NUMBER_ARGS;
   }
   else
   {
       SerialPutString("# of Tasks: ");
       SerialPutDecNum(OSTaskCtr);
       SerialPutString("\r\nCPU USEAGE: ");
       SerialPutDecNum(OSCPUUsage);
       SerialPutString("\r\n");
   }
   return err;
#else
    SerialPutString("OS Stat task was never enabled\r\n");
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSTaskCreateS
*
* Description: This function creates a task. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSTaskCreateS( char *cmdArgs ) 
{
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U priority = 0;
   INT8U place = 0;   
   
   err = GetTaskONEParm(cmdArgs, &priority);
   if (err == OK)
   {
        if(TaskCnt == MAX_SHELL_TASKS)
        {
            SerialPutString("Total Task limit reached\r\n");
        }
        else
        {
			while ( (TaskPrio[place] != 0xff) && (place != MAX_SHELL_TASKS) ) 
                     place++;
			
            errOS = OSTaskCreate(ShellTasksFunction, (void *)0,
                       &SHELL_TSK_STK[place][SHELL_TSK_SIZE - 1], priority);
            if (errOS == OS_NO_ERR)
            {
                SerialPutString("Task Created\r\n");              
                TaskPrio[place] = priority;
                TaskCnt++;
            }
            else
            {
			   TaskPrio[place] = 0xff;
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
            }
        }            
   }
   
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSTaskDeleteS
*
* Description: This function deletes a task. If the task deleted is one of the task created with the shell
*              a resources are freed. a task. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSTaskDeleteS( char *cmdArgs )
{
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U priority = 0;
   INT8U index = 0;
   
   err = GetTaskONEParm(cmdArgs, &priority);
   if (err == OK)
   {
        errOS = OSTaskDel(priority);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Task Deleted\r\n");

                for (index = 0; index < TaskCnt; index++)
                {
                    if( priority == TaskPrio[index])  /* If the task we just deleted was a shell task, then decrease the count */
                    {
                        TaskPrio[index] = 0xFF; 
                        TaskCnt--;                 
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSTaskSuspendS
*
* Description: This function suspends a task. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSTaskSuspendS( char *cmdArgs )
{

   INT8U err = OK;
   INT8U errOS = OK;
   INT8U priority = 0;
   
   err = GetTaskONEParm(cmdArgs, &priority);
   if (err == OK)
   {
        errOS = OSTaskSuspend(priority);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Task Suspended\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSTaskResumeS
*
* Description: This function suspends a task. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSTaskResumeS( char *cmdArgs )
{
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U priority = 0;

   err = GetTaskONEParm(cmdArgs, &priority);
   if (err == OK)
   {
        errOS = OSTaskResume(priority);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Task Resumed\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSemphoreCreateS
*
* Description: This function creates a task. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSSemphoreCreateS( char *cmdArgs )
{
#if (OS_SEM_EN > 0)
   INT8U err = OK;
   INT16U count = 0;
   INT8U place = 0;

   err = GetTaskONEParm(cmdArgs, (INT8U*) &count);
   if (err == OK)
   {
        if(SempCnt == MAX_SHELL_SEMPS)
        {
            SerialPutString("Total Semaphore limit reached\r\n");
        }
        else
        {
            while ( (ShellSempList[place] != 0) && (place != MAX_SHELL_SEMPS) ) 
               place++;       
        
            ShellSempList[place] = OSSemCreate(count);
            if (ShellSempList[SempCnt] != OSEventFreeList)
            {
                SerialPutString("Semaphore Created\r\n");
                SempCnt++;
            }
            else
            {
               ShellSempList[place] = 0;
               SerialPutString("ECB is full. \r\n");
            }
        }            
   }
   return err; 
#else
    SerialPutString("OS_SEM_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSemphoreDeleteS
*
* Description: This function deletes a semaphore. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSSemphoreDeleteS( char *cmdArgs )
{
#if (OS_SEM_EN > 0)
#if (OS_SEM_DEL_EN > 0)
#if (OS_MAX_EVENTS > 0)
   INT8U index = 0, tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, opt = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			opt = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }


   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSSemDel(pevent, opt, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Semaphore Deleted\r\n");
                for (index = 0; (index < MAX_SHELL_SEMPS && SempCnt); index++)
                {
                    if( pevent == ShellSempList[index])  /* If the semaphore we just deleted that was  */
                    {                                   /* created by the shell, we will restore the resources. */
                        ShellSempList[index] = (OS_EVENT*)0; 
                        SempCnt--;              
                        break;                        
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }   
   return err; 
#else
    SerialPutString("OS_MAX_EVENTS is 0\r\n");
    return  OK; 
#endif
#else
    SerialPutString("OS_SEM_DEL_EN is not enabled\r\n");
    return  OK; 
#endif
#else
    SerialPutString("OS_SEM_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSemphorePendS
*
* Description: This function pends a semaphore. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSSemphorePendS( char *cmdArgs )
{
#if (OS_SEM_EN > 0)
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, timeout = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			timeout = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSSemPend(pevent, timeout, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Semaphore Pended\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else
    SerialPutString("OS_SEM_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSSemphorePostS
*
* Description: This function posts on a semaphore. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSSemphorePostS( char *cmdArgs )
{
#if (OS_SEM_EN > 0)
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U ECBNumber = 0;
   OS_EVENT  *pevent;

   err = GetTaskONEParm(cmdArgs, &ECBNumber);
   if (err == OK)
   {
        pevent = &OSEventTbl[ECBNumber];
        errOS = OSSemPost(pevent);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Semaphore posted.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else
    SerialPutString("OS_SEM_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMutexCreateS
*
* Description: This function creates a mutex. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMutexCreateS( char *cmdArgs )
{
#if (OS_MUTEX_EN > 0)
   INT8U err = OK;
   INT8U errOS; 
   INT16U prio = 0;
   INT8U place = 0;

   err = GetTaskONEParm(cmdArgs, (INT8U*) &prio);
   if (err == OK)
   {
        if(MutexCnt == MAX_SHELL_MUTEXS)
        {
            SerialPutString("Total Mutex limit Exceed\r\n");
        }
        else
        {

			while ( (ShellMutexList[place] != 0) && (place != MAX_SHELL_MUTEXS) ) 
               place++; 
        
            ShellMutexList[place] = OSMutexCreate(prio, &errOS);
            if (errOS == OS_NO_ERR)
            {
                SerialPutString("Mutex Created\r\n");
                MutexCnt++;
            }
            else
            {
                   ShellMutexList[place] = 0;
                   SerialPutString("Operation failed. Errror # ");
                   SerialPutDecNum(errOS);
                   SerialPutString("\r\n");
            }
        }            
   }
   return err; 
#else
    SerialPutString("OS_MUTEX_EN is 0\r\n");
    return OK;
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMutexDeleteS
*
* Description: This function deletes a mutex. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSMutexDeleteS( char *cmdArgs )
{
#if (OS_MUTEX_EN > 0)
   INT8U index = 0;
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, opt = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			opt = atoi ( token ); 
            break;

          default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSMutexDel(pevent, opt, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Mutex Deleted\r\n");
                for (index = 0; (index < MAX_SHELL_MUTEXS && MutexCnt); index++)
                {
                    if( pevent == ShellMutexList[index])
                    {
                        ShellMutexList[index] = (OS_EVENT*)0; 
                        MutexCnt--;
                        break;                 
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else
    SerialPutString("OS_MUTEX_EN is 0\r\n");
    return OK;
#endif  
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMutexPostS
*
* Description: This function posts on a mutex. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSMutexPostS( char *cmdArgs )
{
#if (OS_MUTEX_EN > 0)
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U ECBNumber = 0;
   OS_EVENT  *pevent;

   err = GetTaskONEParm(cmdArgs, &ECBNumber);
   if (err == OK)
   {
        pevent = &OSEventTbl[ECBNumber];
        errOS = OSMutexPost(pevent);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Mutex posted.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else
    SerialPutString("OS_MUTEX_EN is 0\r\n");
    return OK;
#endif  
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMutexPendS
*
* Description: This function pends on a mutex. 
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSMutexPendS( char *cmdArgs )
{
#if (OS_MUTEX_EN > 0)
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, timeout = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			timeout = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSMutexPend(pevent, timeout, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Semaphore Pended\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else
    SerialPutString("OS_MUTEX_EN is 0\r\n");
    return OK;
#endif     
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSFlagCreateS
*
* Description: This function creates a flag group.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSFlagCreateS( char *cmdArgs )
{
#if (OS_FLAG_EN > 0)
   INT8U err = OK;
   INT8U errOS; 
   INT16U flags = 0;
   INT8U place = 0;

   err = GetTaskONEParm(cmdArgs, (INT8U*) &flags);
   if (err == OK)
   {
        if(FlagGrpCnt == MAX_SHELL_FLAGGRPS)
        {
            SerialPutString("Total flag group limit reached\r\n");
        }
        else
        {

		   while ( (ShellFlagGroupList[place] != 0) && (place != MAX_SHELL_FLAGGRPS) ) 
               place++;

            ShellFlagGroupList[place] = OSFlagCreate(flags, &errOS);
            if (errOS == OS_NO_ERR)
            {
                SerialPutString("Flaggroup Created\r\n");
                FlagGrpCnt++;
            }
            else
            {   
                ShellFlagGroupList[place] = (OS_FLAG_GRP*)0;
                SerialPutString("Operation failed. Errror # ");
                SerialPutDecNum(errOS);
                SerialPutString("\r\n");
            }
        }            
   }
   return err;

#else
    SerialPutString("OS_FLAG_EN is 0\r\n");
    return OK;
#endif  
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSFLagDeleteS
*
* Description: This function deletes a flag group.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSFLagDeleteS( char *cmdArgs )
{
#if (OS_FLAG_EN > 0)
#if (OS_FLAG_DEL_EN > 0)

   INT8U index = 0;
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U flagGrpNumber = 0, opt = 0;
   OS_FLAG_GRP  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			flagGrpNumber = atoi ( token ); 
            break;

         case 1: 
			opt = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSFlagTbl[0];
        OSFlagDel(pevent, opt, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Flag group Deleted\r\n");
                for (index = 0; (index < MAX_SHELL_FLAGGRPS && FlagGrpCnt); index++)
                {
                    if( pevent == ShellFlagGroupList[index])                     
                    {
                        ShellFlagGroupList[index] = (OS_FLAG_GRP*)0; 
                        FlagGrpCnt--;
                        break;                 
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }   
   return err;
#else
    SerialPutString("OS_FLAG_DEL_EN is disabled. \r\n");
    return OK;
#endif
#else
    SerialPutString("OS_MUTEX_EN is 0. \r\n");
    return OK;
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSFlagPostS
*
* Description: This function posts on a flag group.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSFlagPostS( char *cmdArgs )
{
#if (OS_FLAG_EN > 0)
   INT8U flagGrpNumber = 0;
   INT16U flags = 0;
   INT8U opt = 0;
   INT8U err = OK;
   INT8U tokenCount  = 0;
   INT8U errOS = OK;
   OS_FLAG_GRP  *pevent;
   
   while (1)
   {    
      cmdArgs = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (cmdArgs == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			flagGrpNumber = atoi ( cmdArgs ); 
            break;

         case 1: 
			flags = atoi ( cmdArgs ); 
            break;

         case 2: 
			opt = atoi ( cmdArgs ); 
            break;

         default:
            err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
   }

   if (tokenCount != 3)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)
   {
        pevent = &OSFlagTbl[flagGrpNumber];
        OSFlagPost(pevent, flags, opt, &err);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Flaggroup posted.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else
    SerialPutString("OS_FLAG_EN is 0\r\n");
    return OK;
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSFlagPendS
*
* Description: This function pends on a flag group.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSFlagPendS( char *cmdArgs )
{
#if (OS_FLAG_EN > 0)
   INT8U flagGrpNumber = 0;
   INT16U flags = 0;
   INT8U waitType = 0;
   INT16U timeout = 0;
   INT8U err = OK;
   INT8U tokenCount  = 0;
   INT8U errOS = OK;
   OS_FLAG_GRP  *pevent;
   
   while (1)
   {    
      cmdArgs = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (cmdArgs == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			flagGrpNumber = atoi ( cmdArgs ); 
            break;

         case 1: 
			flags = atoi ( cmdArgs ); 
            break;

         case 2: 
			waitType = atoi ( cmdArgs ); 
            break;

         case 3: 
			timeout = atoi ( cmdArgs ); 
            break;

         default:
            err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
   }

   if (tokenCount != 4)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)
   {
        pevent = &OSFlagTbl[flagGrpNumber];
        OSFlagPend(pevent, flags, waitType, timeout, &err);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Flaggroup pended.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else
    SerialPutString("OS_FLAG_EN is 0\r\n");
    return OK;
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMailBoxCreateS
*
* Description: This function creates a mailbox.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMailBoxCreateS( char *cmdArgs )
{
#if (OS_MBOX_EN > 0)
   INT8U tokenCount= 0;
   INT8U place = 0;
   INT8U err = OK;
   char *msg = 0; 
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			msg = token; 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }


   if (tokenCount != 1)
        err  = ILLEGAL_NUMBER_ARGS;


   if (err == OK)
   {
        if(MailboxCnt == MAX_SHELL_MAILBOXS)
        {
            SerialPutString("Total mailbox limit reached\r\n");
        }
        else
        {

			while ( (ShellMailBoxList[place] != 0) && (place != MAX_SHELL_MAILBOXS) ) 
               place++;
            
            ShellMailBoxList[place] = OSMboxCreate(msg);
            if (ShellMailBoxList[place] != OSEventFreeList)
            {
                SerialPutString("MailboxCnt Created\r\n");
                MailboxCnt++;
            }
            else
            {
               ShellMailBoxList[place] = 0;
               SerialPutString("ECB is full. \r\n");
            }
        }            
   }
   return err; 
#else
    SerialPutString("OS_MBOX_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMailBoxDeleteS
*
* Description: This function deletes a mailbox.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMailBoxDeleteS( char *cmdArgs )
{
#if (OS_MBOX_EN > 0)
#if (OS_MBOX_DEL_EN > 0)
   INT8U index = 0;
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, opt = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			opt = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }


   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSMboxDel(pevent, opt, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Mailbox Deleted\r\n");
                for (index = 0; (index < MAX_SHELL_MAILBOXS && MailboxCnt); index++)
                {
                    if (pevent == ShellMailBoxList[index])  /* If the task we just deleted was a shell task, then decrease the count */
                    {
                        ShellMailBoxList[index] = (OS_EVENT*)0; 
                        MailboxCnt--;
                        break;                 
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else 
    SerialPutString("OS_MBOX_DEL_EN is 0\r\n");
    return OK:
#endif
#else
    SerialPutString("OS_MBOX_EN is 0\r\n");
    return OK:
#endif
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMailBoxPendS
*
* Description: This function pends on a mailbox.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMailBoxPendS( char *cmdArgs )
{
#if (OS_MBOX_EN > 0)
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, timeout = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			timeout = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = &OSEventTbl[ECBNumber];
        OSMboxPend(pevent, timeout, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Mailbox Pended\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }   
   return err;
#else
    SerialPutString("OS_MBOX_EN is 0\r\n");
    return OK:
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMailBoxPostS
*
* Description: This function posts on a mailbox.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMailBoxPostS( char *cmdArgs )
{
#if (OS_MBOX_EN > 0)
#if (OS_MBOX_POST_EN > 0)
   INT8U err = OK;
   INT8U errOS = 0;
   INT8U tokenCount = 0; 
   INT8U ECBNumber = 0;
   OS_EVENT  *pevent;
   char *msg = 0;

   while (1)
   {    
      cmdArgs = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (cmdArgs == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
            ECBNumber = atoi ( cmdArgs );
            break;

         case 1: 
			msg = cmdArgs;
            err = OK; 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
      }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;    

   if (err == OK)
   {
        pevent = &OSEventTbl[ECBNumber];
        errOS = OSMboxPost(pevent, (void*) msg);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Mailbox posted.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else 
    SerialPutString("OS_MBOX_POST_EN is 0\r\n");
    return OK:
#endif
#else
    SerialPutString("OS_MBOX_EN is 0\r\n");
    return OK:
#endif
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMailBoxPostOpts
*
* Description: This function changes option on what a mailbox posts on.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMailBoxPostOpts( char *cmdArgs )
{
#if (OS_MBOX_EN > 0)
#if (OS_MBOX_POST_OPT_EN > 0)
   INT8U err = OK;
   INT8U errOS = 0;
   INT8U opt = 0;
   INT8U tokenCount = 0; 
   INT8U ECBNumber = 0;
   OS_EVENT  *pevent;
   char *msg = 0;

   while (1)
   {    
      cmdArgs = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (cmdArgs == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
            ECBNumber = atoi ( cmdArgs );
            break;

         case 1: 
			msg = cmdArgs;
            break;

         case 2: 
			opt = atoi ( cmdArgs );;
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
      }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 3)
        err  = ILLEGAL_NUMBER_ARGS;    

   if (err == OK)
   {
        pevent = &OSEventTbl[ECBNumber];
        errOS = OSMboxPostOpt(pevent, (void*) msg, opt);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Mailbox posted with options.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else 
    SerialPutString("OS_MBOX_POST_OPT_EN is 0\r\n");
    return OK:
#endif
#else
    SerialPutString("OS_MBOX_EN is 0\r\n");
    return OK:
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSQueueCreateS
*
* Description: This function creates a queue.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSQueueCreateS( char *cmdArgs )
{
#if (OS_Q_EN > 0)
   INT8U err = OK;
   INT8U errOS = 0;
   INT8U place = 0;

   cmdArgs = mystrtok(NULL, ' '); /* Process the string */
   if ( cmdArgs != NULL)
        err  = ILLEGAL_NUMBER_ARGS;  

   if (err == OK)
   {
        if(QueueCnt == MAX_SHELL_QUEUES)
        {
            SerialPutString("Total Queue limit reached\r\n");
        }
        else
        {
			while ( (ShellQueueList[place] != 0) && (place != MAX_SHELL_QUEUES) ) 
               place++;              
         
            ShellQueueList[place] = (OS_EVENT*) OSQCreate(&ShellQueueBuffers[MAX_SHELL_QUEUES][0], QUEUE_SIZE);
            if (errOS == OS_NO_ERR)
            {
                SerialPutString("Queue Created\r\n");
                QueueCnt++;
            }
            else
            {   ShellQueueList[place] = 0;
                SerialPutString("Operation failed. Errror # ");
                SerialPutDecNum(errOS);
                SerialPutString("\r\n");
            }
        }            
   }
   return err;
#else 
    SerialPutString("OS_Q_EN is 0\r\n");
    return OK:
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSQueueDeleteSs
*
* Description: This function deletes a queue.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSQueueDeleteS( char *cmdArgs )
{ 
#if (OS_Q_EN > 0)
#if (OS_Q_DEL_EN > 0)

   INT8U index = 0;
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, opt = 0;
   OS_EVENT  *pevent;

   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			opt = atoi ( token ); 
            break;

          default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = (OS_EVENT*) &OSQTbl[ECBNumber];
        OSQDel(pevent, opt, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Queue Deleted\r\n");
                for (index = 0; (index < MAX_SHELL_QUEUES && QueueCnt); index++)
                {
                    if( pevent == ShellQueueList[index])  /* If the task we just deleted was a shell task, then decrease the count */
                    {
                        ShellQueueList[QueueCount] = (OS_EVENT*)0; 
                        QueueCnt--;
                        break;                 
                    }
                }
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else 
    SerialPutString("OS_Q_DEL_EN is 0\r\n");
    return OK;
#endif 
#else
    SerialPutString("OS_Q_EN is 0\r\n");
    return OK;
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSQueuePostS
*
* Description: This function posts on a queue.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSQueuePostS( char *cmdArgs )
{ 
#if (OS_Q_EN > 0)
#if (OS_Q_POST_EN > 0) 
   INT8U err = OK;
   INT8U errOS = OK;
   INT8U tokenCount = 0;
   INT8U ECBNumber = 0;
   OS_EVENT  *pevent;
   char* msg = 0;

   while (1)
   {    
      cmdArgs = mystrtok(NULL, ' '); /* Process the string */  
      if ( (cmdArgs == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( cmdArgs ); 
            break;

         case 1: 
			msg = cmdArgs; 
            break;

          default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   /* make sure token count is valid */
   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)
   {
        pevent = (OS_EVENT*) &OSQTbl[ECBNumber];
        errOS = OSQPost(pevent, msg);
        if (errOS == OS_NO_ERR)
        {
              SerialPutString("Queue posted.\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   return err;
#else
    SerialPutString("OS_Q_POST_EN is 0\r\n");
    return OK;
#endif 
#else
    SerialPutString("OS_Q_EN is 0\r\n");
    return OK;
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMemoryPendS
*
* Description: This function pends on a queue.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U  OSMemoryPendS( char *cmdArgs )
{
#if (OS_Q_EN > 0)
   INT8U tokenCount= 0;
   INT8U errOS, err = OK;
   INT8U ECBNumber = 0, timeout = 0;
   OS_EVENT  *pevent;
   char *token;
   
   while (1)
   {    
      token = mystrtok(NULL, ' '); /* Process the string */
  
      if ( (token == NULL) || (err != OK) )   /* If there no more tokens then get out */
        break;

      switch (tokenCount)
      {         
         case 0:                          /* Command name */
			ECBNumber = atoi ( token ); 
            break;

         case 1: 
			timeout = atoi ( token ); 
            break;

         default: 
			err  = ILLEGAL_NUMBER_ARGS;
            continue;
	  }
      
      tokenCount++;						 /* Increment the token count */
      /* Get the next token */
   }

   if (tokenCount != 2)
        err  = ILLEGAL_NUMBER_ARGS;

   if (err == OK)                  /* If there were enough arguments then return */
   {   
        pevent = (OS_EVENT*) &OSQTbl[ECBNumber];
        OSQPend(pevent, timeout, &errOS);
        if (errOS == OS_NO_ERR)
        {
                SerialPutString("Queue Pended\r\n");
        }
        else
        {
               SerialPutString("Operation failed. Errror # ");
               SerialPutDecNum(errOS);
               SerialPutString("\r\n");
        }
   }
   
   return err;
#else
    SerialPutString("OS_Q_EN is 0\r\n");
    return OK
#endif 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMemoryCreateS
*
* Description: This function creates a memory partition.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U OSMemoryCreateS(char *cmdArgs)
{
#if (OS_MEM_EN > 0)
   INT8U errOS = OK;
   if(MemPartCount == MAX_SHELL_MEMPARTS)
   {
       SerialPutString("Total Memory parts limit reached\r\n");
   }
   else
   {
       ShellMemParts[MemPartCount] = OSMemCreate(MemPart, BLK_NUMBER, BLK_SIZE, &errOS);
       if (errOS == OS_NO_ERR)
       {
           SerialPutString("Memory part created\r\n");
           MemPartCount++;
       }
       else
       {
          SerialPutString("Operation failed. Errror # ");
          SerialPutDecNum(errOS);
          SerialPutString("\r\n");
       }
   }
   return OK; 
#else
    SerialPutString("OS_MEM_EN not enabled");
    return OK;    
#endif 
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMemoryGetS
*
* Description: This function obtains a block from a particular memory partition.
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    : This function is not implemented.
*********************************************************************************************************
*/
INT8U OSMemoryGetS( char *cmdArgs )
{
#if (OS_MEM_EN > 0)
    return OK;
#else
    SerialPutString("Operation failed. Errror # ");
    return OK;    
#endif 
} 

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSMemoryPutS
*
* Description: This function return a block to a memory partition
* 
* Arguments  : cmdArgs - a pointer to null terminated string. 
*
* Returns    : an error code
*
* Note(s)    : This function is not implemented.
*********************************************************************************************************
*/
INT8U  OSMemoryPutS( char *cmdArgs )
{
#if (OS_MEM_EN > 0)
    return OK;
#else
    SerialPutString("OS_MEM_EN not enabled");
    return OK;    
#endif 
} 

#endif   /* ENABLE_SHELL  */

