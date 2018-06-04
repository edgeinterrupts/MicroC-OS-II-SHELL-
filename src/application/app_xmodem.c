#if 0
/************************************************************************ 
*  File: app_xmodem.c
*  Author: Omer Farooq
*  Date: 14-Feb-06
*
* This file contains x-modem protocol implementation.
*************************************************************************/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                              File-level constants
*********************************************************************************************************
*/
/* XModem tunables */
#define MAX_NAKS        10    /* Max number of NAKs before giving up */

/* XModem opcodes */
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

/*$PAGE*/
/*
*********************************************************************************************************
*                                        purge_rx
*
* Description: Purge rx buffer by eating characters until a timeout.
*
* Arguments  : timeout - number of millisecond to wait before timing out.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
static void purge_rx(INT16U timeout)
{
   char ch;
   INT8U err; 
  
   while (1)
   {
      err = SerialGetCharTimeout(&ch, timeout); 
      if (err == TIMEDOUT) 
            break;
   }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        XmodemDownload
*
* Description: Perform an Xmodem download to the given buffer. Returns a negative number to 
*              indicate a failure or the length of the downloaded file if successful.
*
* Arguments  : dest - a pointer to the buffer where data should be stored.
*
* Returns    : none
*
* Note(s)    :
*********************************************************************************************************
*/
INT8U XmodemDownload(char *dest)
{
   char a = 0, b = 0;
   char blkno = 1;
   char ch, cksum;
   INT16U len = 0;
   INT16U x, blksz;
   INT16U bad = 1;
   INT16U rc = -1;
   INT16U naks = 0;
   INT8U err;

   while(1)
   {
      if (bad)
      {
         purge_rx(100);
         SerialPutChar(NAK);

         if (naks++ >= MAX_NAKS)
            return XM_TIMEOUT;
      }

      /* Assume failure */
      bad = 1;

      /* SOH */
      err = SerialGetCharTimeout(&ch, 200); 
      if (err == TIMEDOUT) 
         continue;
      switch (ch)
      {
         case SOH:
            blksz = 128;
            break;

         case STX:
            blksz = 1024;
            break;

         case EOT:
            SerialPutChar(ACK);
            return len;

         case CAN:
            return XM_CANCELLED;

         default:
            continue;
      }
 
      /* BLK NUM */
      err = SerialGetCharTimeout(&a, 100); 
      if (err == TIMEDOUT) 
         continue;     

      /* BLK NUM COMPLEMENT */
      err = SerialGetCharTimeout(&b, 100); 
      if (err == TIMEDOUT) 
         continue;

      if (a == blkno && b == (255-blkno))
      {
         /*
          * Block number is exactly what we expected.
          * Nothing special to do.
          */
      }
      else if (a == (blkno-1) && b == (255-(blkno-1)))
      {
         /*
          * Block number is repeat of last block.
          * Skip the block and the checksum.
          */
         for (x=0; x<blksz+1; x++)
         {
            err = SerialGetCharTimeout(&ch, 100); 
            if (err == TIMEDOUT) 
                break;
         }

         if (x != blksz+1)
            continue;

         /* Acknowledge the block and go back for another */
         SerialPutChar(ACK);
         bad = 0;
         naks = 0;
         continue;
      }
      else
      {
         /* Block number is unrecognized: error */
         continue;
      }

      /* DATA */
      cksum = 0;
      for (x=0; x<blksz; x++)
      {
         err = SerialGetCharTimeout(&ch, 100); 
         if (err == TIMEDOUT) 
                break;

         cksum += ch;
         dest[x] = ch;
      }

      if (x != blksz)
         continue;
      
      /* CKSUM */
      err = SerialGetCharTimeout(&ch, 100);  
      if (err == TIMEDOUT) 
          continue;

      if (ch != cksum)
         continue;

      /* ACK the block and commit the data */
      SerialPutChar(ACK);
      blkno++;
      dest += blksz;
      len += blksz;
      bad = 0;
   }
   
   return rc;
}
#endif

