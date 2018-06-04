#if 1

/*$PAGE*/
/*
*********************************************************************************************************
*  File: app_shell.h
*  Author: Omer Farooq
*  Date:  20-February-05
*
*  ucOS-II aplication level shell task.
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
*                                             Function prototypes 
*********************************************************************************************************
*/ 
static INT8U Man( char *cmdArgs);
static INT8U Help( char *cmdArgs);
static INT8U AutoMode( char *cmdArgs);
static void ProcessUserInput(const char *processCmd);

/*
*********************************************************************************************************
*                                             Global Types
*********************************************************************************************************
*/
/* Function pointers table definition*/
struct CmdFunctions
{
   char* CmdName;
   INT8U (*Process)(char *cmdArgs);
   char* HelpText;
};

/*
*********************************************************************************************************
*                                            File-level Constants
*********************************************************************************************************
*/
#define TOTAL_CMDS          4          /* Should be power-2 number (2,4,8,16..1024) */
#define MAX_INPUT           32
#define BACKSPACE           0x8
#define ESC                 0x1b

#define AUTO_MODE_LOCKED    0
#define AUTO_MODE_UNLOCKED  1
#define MAX_FILE_SIZE       1

static const char *logo =  
   "                   OOOOOOO       OO    OOOOOOOOOO  OOOOOOO   OO  OO\r\n"
   "                  OO            OO    OO       OO  OO        OO  OO\r\n"
   "    OO      OO  OO            OO    OO       OO    OO        OO  OO\r\n"
   "   OO      OO  OO             OO    OO       OO      OOOOO   OO  OO\r\n"
   "  OO      OO    OO           OO    OO       OO          OO   OO  OO\r\n"
   " OO      OO      OO         OO    OO       OO         OOO    OO  OO\r\n"
   "OOOOOOOOOOO       OOOOOOO  OO    OOOOOOOOOOOO    OOOOOOO     OO  OO\r\n"
   "\r\n\t\tMicroC/OS-II Shell\r\n"
   ;                               

/* Network Interface callback structures for each CPU */
static const struct CmdFunctions Commands[] =
{
   /* name,              function,               help text */
   { "Automate",          AutoMode,             "Runs a script" },
   { "TCB",               GetTCB,               "Lists all Tassask Control Block nodes" },
   { "GETTIME",           GetTime,              "Gets current system tick count" },
   { "SETTIME",           SetTime,              "Sets current system time. SetTime TickCount" },
   { "TIMEDELAY",         TimeDelay,            "Places this task in a delay. TimeDelay hours minutes seconds milli" },
   { "AUTOMATE",          AutoMode,             "Places this task in script mode. Send the file over X-modem" },   
   { "MCB",               GetMCB,               "Lists all Memory Control Block nodes" },
   { "ECB",               GetOEB,               "Lists all Event Control Block nodes" },
   { "EFG",               GetOFB,               "Lists all Event Flag group nodes" },
   { "OMQ",               GetOMQ,               "Lists all Message queue nodes" },
   { "HELP",              Help,                 "Help"},
   { "OSINFO",            GetOSInfo,            "Provides OS Info" },
   { "OSSTATS",           GetOSStats,           "Provides OS Statistics" },
   { "OSTASKCREATE",      OSTaskCreateS,        "Creates a task. \r\n\tOSTASKCREATE Tsk_Prio" },
   { "OSTASKDEL",         OSTaskDeleteS,        "Deletes a task. \r\n\tOSTASKDELETE Tsk_Prio" },
   { "OSTASKSUSPEND",     OSTaskSuspendS,       "Suspends a task. \r\n\tOSTASKSUSPEND Tsk_Prio"},
   { "OSTASKRESUME",      OSTaskResumeS,        "Resumes a task.  \r\n\tOSTASKSuspend Tsk_Prio"},
   { "OSSEMAPHORECREATE", OSSemphoreCreateS,    "Creates a semaphore. \r\n\tOSSEMAPHORECREATE Count" },
   { "OSSEMAPHOREDEL",    OSSemphoreDeleteS,    "Deletes a semaphore. \r\n\tOSSEMAPHOREDEL ECBNumber opt" },
   { "OSSEMAPHOREPOST",   OSSemphorePostS,      "Posts on a semaphore. \r\n\tOSSEMAPHOREPOST ECBNumber"},
   { "OSSEMAPHOREPEND",   OSSemphorePendS,      "Pends the semaphore. \r\n\tOSSEMAPHOREPEND ECBNumber timeout "},
   { "OSMUTEXCREATE",     OSMutexCreateS,       "Creates a mutex. \r\n\tOSMUTEXCREATE Prio" },
   { "OSMUTEXDEL",        OSMutexDeleteS,       "Deletes a mutex. \r\n\tOSMUTEXDELETE ECBNumber opt" },
   { "OSMUTEXDPOST",      OSMutexPostS,         "Pends the a mutex. \r\n\tOSMUTEXDPOST ECBNumber"},
   { "OSMUTEXPEND",       OSMutexPendS,         "Posts on a mutex. \r\n\tOSMUTEXPEND ECBNumber timeout"},
   { "OSFLAGCREATE",      OSFlagCreateS,        "Creates a Flag Group. \r\n\tOSFLAGCREATE flags" },
   { "OSFLAGDEL",         OSFLagDeleteS,        "Deletes a Flag Group. \r\n\tOSFLAGDELETE flagGrpNumber opt" },
   { "OSFLAGPOST",        OSFlagPostS,          "Posts on a Flag Group. \r\n\tOSFLAGPOST flagGrpNumber flags opt"},
   { "OSFLAGPEND",        OSFlagPendS,          "Pends a Flag Group. \r\n\tOSFLAGPEND flagGrpNumber flags wait_type timeout"},
   { "OSMAILBOXCREATE",   OSMailBoxCreateS,     "Creates a OSMailBox. \r\n\tOSMAILBOXCREATE msg" },
   { "OSMAILBOXDEL",      OSMailBoxDeleteS,     "Deletes a OSMailBox. \r\n\tOSMAILBOXDELETE ECBNumber opt" },
   { "OSMAILBOXPOST",     OSMailBoxPostS,       "Pends a OSMailBox. \r\n\tOSMAILBOXPOST ECBNumber msg"},
   { "OSMAILBOXPEND",     OSMailBoxPendS,       "Posts on a OSMailBox. \r\n\tOSMAILBOXPEND ECBNumber timeout"},
   { "OSMAILBOXOPT",      OSMailBoxPostOpts,    "Options of a OSMailBox. \r\n\tOSMAILBOXOPT [ARGS]"},
   { "OSQUEUECREATE",     OSQueueCreateS,       "Creates a Queue. \r\n\tOSQUEUECREATE" },
   { "OSQUEUEDEL",        OSQueueDeleteS,       "Deletes a Queue. \r\n\tOSQUEUEDELETE QueuNumber opt" },
   { "OSQUEUEPOST",       OSQueuePostS,         "Posts on a Queue. \r\n\tOSQUEUEPOST QueuNumber msg"},
   { "OSQUEUEPEND",       OSMemoryPendS,        "Pends the Queue. \r\n\tOSQUEUEPEND QueuNumber timeout"},
   { "OSMMEMORYCREATE",   OSMemoryCreateS,      "Creates a mem part. \r\n\tOSMMEMORYCREATE [ARGS]" },
   { "OSMEMORYGET",       OSMemoryGetS,         "Gets a mem part. \r\n\tOSMEMORYGET [ARGS]" },
   { "OSMEMORYPUT",       OSMemoryPutS,         "Puts the mem part back. \r\n\tOSMEMORYPUT [ARGS]"},
   { "MAN",               Man,                   "Simpler version of UNIX man." },
};
#define NUM_CMDS ( sizeof(Commands) / sizeof(Commands[0]) )

#define SCRIPT_SIZE 3
static const char SciptCmdList[SCRIPT_SIZE][8] =  { "ECB", "HELP", "TCB",};

/*
*********************************************************************************************************
*                                            File-level VARIBALES
*********************************************************************************************************
*/
static char input_line[TOTAL_CMDS][MAX_INPUT];
static INT8U AutoModeStatus = AUTO_MODE_UNLOCKED;

/*$PAGE*/
/*
*********************************************************************************************************
*                                        Man
*
* Description: This function supports man command of the shell. The man command is used to print 
*              the help text of the command.
*
* Arguments  : cmdArgs - a pointer to the rest of the command that was entered by the user.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
static INT8U Man( char *cmdArgs)
{
   INT8U found = 0, index = 0; 
   INT8U err = ILLEGAL_NUMBER_ARGS; 
   
   cmdArgs = mystrtok(NULL, ' '); /* Process the string */  
   if (cmdArgs != NULL)           /* If there no more tokens then get out */
        err = OK;

   if (err == OK)                   /* If there were enough arguments then return */
   {
       while ((index < NUM_CMDS) && (!found))
       {
           if (strcmp(cmdArgs, Commands[index].CmdName) == 0)
               found = 1;
           else
	           index++;              /* increment the counter */
       } 

       if (found == 1)    /* If not found then discard this command */
       {
            SerialPutString(&Commands[index].HelpText[0]); 
            SerialPutString("\r\n");
       }
       else
       {
            SerialPutString("NO Such command\r\n");            
       }    
   }
   
   return err; 
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        help
*
* Description: This function supports man command of the shell. The help command is used to all the commands
*              and their brief descriptions.
*
* Arguments  : cmdArgs - a pointer to the rest of the command that was entered by the user.
*
* Returns    : none
*
* Note(s)    : 
*********************************************************************************************************
*/
static INT8U Help( char *cmdArgs){
   INT8U index= 0;  
   INT8S err = ILLEGAL_NUMBER_ARGS;
   
   cmdArgs = mystrtok(NULL, ' '); /* Process the string */
   if ( cmdArgs == NULL)          /* There should no more tokens */
   {
       err = OK;   
       while (index <NUM_CMDS)
       {
          SerialPutChar('(');
          SerialPutString(Commands[index].CmdName);
          SerialPutString(") -");
          SerialPutString(Commands[index].HelpText);
          SerialPutString("\r\n");
	      index++;
       }
   } 

   return err;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        ScriptingMode
*
* Description: This function is simple static helper function that calls downloads a file over 
*              xmodem, then parse all data into strings and calls command processor for each 
*              line to be processed.
*
* Arguments  : none.
*
* Note(s)    : 
*********************************************************************************************************
*/
static void ScriptingMode(){
#if 0
   INT8S rc;
   
   SerialPutString("NOW SEND THE FILE THROUGH XMODEM (CTRL-X to cancel)\r\n");
   rc = XmodemDownload(&ScriptFileBuffer[0]);
   if (rc > 0)        
   {
      SerialPutString("\r\nSuccess.\r\n");
   }
   else if (rc == XM_TIMEOUT)
   {
      SerialPutString("\r\nTimeout.\r\n");
   }
   else if (rc == XM_CANCELLED)
   {
      SerialPutString("\r\nAborted.\r\n");
   }
#endif
#if 1
   INT8U index = 0;
   SerialPutString("\r\nSkipping XMODEM download.\r\n");
   while (index < SCRIPT_SIZE)
   {      
      ProcessUserInput((char*) &SciptCmdList[index][0]);
      index++;
   }
#endif  

   SerialPutString("Automate Done.\r\n");
   AutoModeStatus = AUTO_MODE_UNLOCKED; /* So we don't start scripting when in script mode */
}
   
/*$PAGE*/
/*
*********************************************************************************************************
*                                        AutoMode
*
* Description: This function supports automate command of the shell. The automate command place the shell
*              scripting mode.
*
* Arguments  : cmdArgs - a pointer to the rest of the command that was entered by the user.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/ 
static INT8U AutoMode( char *cmdArgs){  
   INT8U err = ILLEGAL_NUMBER_ARGS; 

   cmdArgs = mystrtok(NULL, ' '); /* Process the string */
   if ( cmdArgs == NULL)          /* There should no more tokens */
   {
       err = OK;   

       if (AutoModeStatus == AUTO_MODE_LOCKED)
         err = ILLEGAL_CMD;
       else
          AutoModeStatus = AUTO_MODE_LOCKED;
    }

    return err;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        ProcessUserInput
*
* Description: This function parses user's input and calls the correct function to handle the command.
*
* Arguments  : processCmd -- a pointer to a null-terminated string.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
static void ProcessUserInput(const char *processCmd){
   char *cmdName;
   INT8U found = 0, index = 0, err = 0;
   char command[MAX_INPUT] = {0};  
   memcpy(command, processCmd, MAX_INPUT); /* Copy the string into a temp to avoid modification */

   cmdName = mystrtok(command, ' '); /* Process the string */
   while(index < NUM_CMDS && !found)
   {
       if (strcmp(cmdName, Commands[index].CmdName) == 0)
           found = 1;
       else
	       index++;              /* increment the counter */
   } 

   if (!found)    /* If not found then discard this command */
      err = ILLEGAL_CMD;

   /* If it read/write make sure the variables */
   if ( (Commands[index].Process != NULL) && found)
      err = Commands[index].Process(NULL);

   if (err != OK)
   {
      SerialPutString((char*) &ErrorMsgs[err][0]);
	  SerialPutString("\r\n");
   }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OSShellTaskStart
*
* Description: This OSShellTaskStart function is called by OS when the Shell task registered and started up.
*              This function continously waits for user input and calls command processor function to
*              process the command.
*
* Arguments  : pdata -- ignored.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
void OSShellTaskStart(void *pdata){
   char inchar = 0;
   INT16U currentCmd = 0, escactive = 0;
   INT16S index, found = 0, nextCmd = 0, prevCmd = 0;
   
   /* Prevent compiler warning */
   pdata = pdata;  

   SerialPutString(logo);           /* Print the startup logo */

   /* We never exit this */
   while(1)  
   { 
      if (AutoModeStatus == AUTO_MODE_LOCKED)
        ScriptingMode();
 
      SerialPutString("/admin/$ ");

      index = 0;      
      /* Get user input while avoiding buffer overflow */
      while(index < MAX_INPUT)
      {
         /* Get the next byte */
         inchar = SerialGetChar();

         /* Keep processing until there is enter key */
         if (inchar == '\r') 
               break;

		 /* Service non-ascii keys */
         if (escactive == 2)
         {
            /* Set the command number */
            found = -1;

            /* 
             * Modify this command to look exactly like last one. 
             * If the is first command ever then discard this character. 
             */
            if (inchar == 'A')
            { 
               /* Determine the next command */
               while ((currentCmd != prevCmd) && (found == -1))
               {
                  if ((input_line[prevCmd][0] != '\0') && (nextCmd != currentCmd))
                     found = prevCmd; 
                  
                  /* Unlock the nextCmd */
                  nextCmd = prevCmd;

                  /* Decrement the count */
                  prevCmd--;
                  
                  /* Prevent overflow */ 
                  if (prevCmd == -1)
                     prevCmd = (TOTAL_CMDS-1);
               }
            }

            /* 
             * Modify this command to look exactly like last one
             * user scrolled up to see. If the is first command 
             * ever or the user hasn't scrolled up to see previous
             * then discard this character. 
             */
            if (inchar == 'B')
            { 
               /* Determine the next command */
               while ((currentCmd != nextCmd) && (found == -1))
               {
                  /* Compute the next command */ 
                  nextCmd = (nextCmd + 1) % TOTAL_CMDS;

                  /* Make sure we don't step over current command */
                  if (nextCmd == currentCmd)
                     break;

                  /* Unlock the prevCmd */
                  prevCmd = nextCmd;

                  /* Handle paritial full array of commands */
                  if (input_line[nextCmd][0] != '\0')
                     found = nextCmd; 
               }
            }

			/* If found an entry then let process it */
            if (found != -1)
            {

               while (index)
               {
                  SerialPutChar(BACKSPACE);
                  SerialPutChar(' ');
                  SerialPutChar(BACKSPACE);
                  index--;
               }

               while( input_line[found][index] != '\0')
               {
                  input_line[currentCmd][index] = input_line[found][index];
                  SerialPutChar(input_line[currentCmd][index]);
                  index++;
               }
            }
            
            escactive = 0;
            continue;
         }

         /* Echo character, digits and whitespace */   
         if (isspace(inchar) || isdigit(inchar) || isalpha(inchar))
         {
            SerialPutChar(inchar);       
            if (isalpha(inchar) && islower(inchar))   /* Convert all lower case to upper */
                inchar = inchar - 32; 
            input_line[currentCmd][index] = inchar;                       
            index++;
         }
         else if (inchar == BACKSPACE && index)
         {
            /* if the character equals back space */
            SerialPutChar(BACKSPACE);
            SerialPutChar(' ');
            SerialPutChar(BACKSPACE);
            index--;
         }
         else if (inchar == ESC)
         {
            escactive = 1;
         }
         else if (inchar == '[' && escactive == 1)
         {
            escactive = 2;
         }
         else
         {
            /* It's a char we don't recognize at all: ignore it */
         }
      }
    
      /* Move the cursor to next line */
      SerialPutString("\r\n");

      /* Command is too long */
	  if (index == MAX_INPUT) 
	  {
          SerialPutString((char*) &ErrorMsgs[CMD_TOO_LONG][0]);
	      SerialPutString("\r\n");
   		  continue;
	  }

      /* process users request */
      if (inchar == '\r' && index)
      {
         /* NULL terminate the string. */
         input_line[currentCmd][index++] = '\0';

		   /* Process the command. Pass by value */
         ProcessUserInput((char*) &input_line[currentCmd]);
         
		   /* Increment the command count number */
         nextCmd = prevCmd = currentCmd; 
         currentCmd = (currentCmd + 1) % TOTAL_CMDS;
      }
   } /* while */
}
#endif /* ENABLE_SHELL */
