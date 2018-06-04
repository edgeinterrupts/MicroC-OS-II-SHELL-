#ifndef HW_INIT_H
#define HW_INIT_H

/*$PAGE*/
/*
*********************************************************************************************************
*  File: hw_init.h
*  Author: Omer Farooq
*  Version: 1.0
*  Date:  23-January-06
*
*   Header file for MSP430 board
*
*   NOTE: Most of the comments in this file are from orignal msp430x149.s provided
*       by Dr. Philips.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <io.h>
#include <signal.h>
#include <iomacros.h>

/*
*********************************************************************************************************
*  Port assignments
*  PxDIR:		(0) = Input		(1) = Output	; Direction
*  PxSEL: 		(0) = I/O		(1) = Alt		; Select
*  P(1|2)IE:	(0) = Disabled	(1) = Enabled	; Interrupt Enable
*  P(1|2)IES: 	(0) = Low->High	(1) = High->Low ; Interrupt Edge Select
*  P(1|2)IFG:	(0) = No Int	(1) = Int		; Interrupt Flag Register
*********************************************************************************************************
*/
/*
 * Port 1
 * Keypad
 * P1[0..3] = KP_ROW[0..3] (SW[1..4];SW[5..8];SW[9..12];SW[13..16])
 * weak pulldown (10k) to 0V
 * Cols on P4[4..7]
 */
#define	KP_ROW0		0x01
#define	KP_ROW1		0x02
#define	KP_ROW2		0x04
#define	KP_ROW3		0x08

/*
 * Interrupt Request Switches (general purpose)
 * P1[6..7] = SW[18..19]
 * weak pullup (10k) to 3.3V & filter cap (0.1u) to 0V
 */
#define	IRQ_SW2		0x40
#define	IRQ_SW1		0x80

/*
 * LCD busy flag poll line
 * P1.4 = LCD_DB.7
 * strong pullup (2.2k) (open collector buffer output) to 3.3V
 */
#define	LCD_DB7_IN		0x10
#define	LCD_BUSY		LCD_DB7_IN

/* 
 * P1.5 = unused
 * P1 available via J2
 * P1[0..4,6,7] configured as inputs
 * P1.5 as output per msp430 documentation for unused port pins
 */
#define	HW_P1SEL		0x00
#define	HW_P1DIR		0x20
#define	HW_P1OUT		0x00
#define	HW_P1IE		    0x00
#define	HW_P1IES		0x00

/*
 * P1[0..3] for polling or l->h h->l interrupt key scanning / de-bouncing
 * P1.4		for polling or h->l interrupt LCD not busy
 * P1[6..7]	for polling or h->l interrupt request / hardware de-bouncing
 */


/*
 * Port 2
 * Multiplexed 7-Segment Displays
 * P2[0..7] = DISP[(1&(!P3.0))|(2&(!P3.1))] (active low enables on P3[0..1])
 * Common anode display interfaced with a 74HC240 inverting buffer
 * So the display codes look like common cathode codes
 * P2 has interrupt abilities, If 74HC240's is left un-enabled its output is
 * open and P2 can be reconfigured as inputs.  You could enable the 74HC240's 
 * and the displays will blink (probably faster than you can see) 
 */
#define	DISP_A   0x01
#define	DISP_B   0x02
#define	DISP_C   0x04
#define	DISP_D   0x08
#define	DISP_E   0x10
#define	DISP_F   0x20
#define	DISP_G   0x40
#define	DISP_DP  0x80

/*
 * P2 available via J3
 * P2[0..7] configured as outputs
 */
#define	HW_P2SEL 0x00
#define	HW_P2DIR 0xff
#define	HW_P2OUT 0x00
#define	HW_P2IE  0x00
#define	HW_P2IES 0x00

/*
 * Port 3
 * Multiplexed 7-Segment Displays
 * P3[0..1] = DISP[1..2] enables (active low)
 */
#define  DISP1_EN 0x01
#define  DISP2_EN 0x02

/* P3[2..3] = unused */

/* 
 * UART
 * P3[4..7] = UART[Tx0,Rx0,Tx1,Rx1]
 * UART lines are sent to RS-232 buffer/driver ([0,3.3]<->[+6,-6])
 */

/*
 * P3 available via J5
 * P3[0..3,4,6] configured as outputs
 * P3[5,7]		configured as inputs
 * P3[4..7]		configured as alt (for UART0/1 peripheral)
 */
#define  HW_P3SEL 0xf0
#define  HW_P3DIR 0x5f
#define  HW_P3OUT 0x03

/*
 * Port 4
 * LCD
 * Port 4[0..3] and port 5[0..7] are interfaced to 74LVC07 non-inverting open
 * collector buffers with strong pull ups (2.2k) to 5V
 * The P4[1..2] are available for their alt function as timer_B
 * outputs as long as the LCD enable (P4.0) is left unactive
 * P4.3 is also available
 * P4.0 	= LCD_EN
 * P4.1 	= LCD_RW
 * P4.2		= LCD_RS
 */
#define  LCD_EN   0x01
#define  LCD_RW   0x02
#define  LCD_RS   0x04

/* 
 * LED
 * P4.3		= LED1
 */
#define LED1   0x08

/* 
 * Keypad
 * P4[4..7] = KP_COL[0..3]	(SW[1,5,9,13];SW[2,6,10,14];
 *            SW[3,7,11,15];SW[4,8,12,16])
 */
#define  KP_COL0  0x10
#define  KP_COL1  0x20
#define  KP_COL2  0x40
#define  KP_COL3  0x80

/* 
 * P4 available via J4
 * P4[0..7] 	configured as outputs
 */
#define  HW_P4SEL 0x00
#define  HW_P4DIR 0xff
#define  HW_P4OUT 0x00

/*
 * Port 5
 * LCD
 * MSP430 Clock signals (buffered) are available on P5[4..6] as alt functions
 * P5.4 = MCLK;		P5.5 = SMCLK;	P5.6 = ACLK
 * MCLK should be the HFO (7.3728 MHz)
 * SMCLK 	= MCLK
 * ACLK should be the LFO (32.768 KHz)
 * ACLK should be left on and used for LPMs
 * upon wake up start the DCO = MCLK = SMCLK (DCO has fast startup time)
 * start the HFO if need be, do tasks while waiting for HFO to stabilize
 * then switch to HFO... I have no idea why I have commented this here.
 * P5[0..7]	= LCD_DB[0..7]
 */
#define  LCD_DB0_OUT 0x01
#define  LCD_DB1_OUT 0x02
#define  LCD_DB2_OUT 0x04
#define  LCD_DB3_OUT 0x08
#define  LCD_DB4_OUT 0x10
#define  LCD_DB5_OUT 0x20
#define  LCD_DB6_OUT 0x40
#define  LCD_DB7_OUT 0x80

/* 
 * P5 available via J6
 * P5[0..7] 	configured as outputs
 */
#define  HW_P5SEL 0x00
#define  HW_P5DIR 0xff
#define  HW_P5OUT 0x00

/*
 * Port 6
 * unused
 * P6 available via J7
 * P6[0..7] 	configured as outputs
 */
#define  HW_P6SEL 0x00
#define  HW_P6DIR 0xff
#define  HW_P6OUT 0x00

/*************************************************************************
*  Memory Map
**************************************************************************/
#define  F_CODE_TOP  0xFFFF
#define  F_IVT_BOT   0xFFE0
#define  F_CODE_BOT  0x1100

#define  F_INFO_TOP  0x10FF
#define  F_INFO_BOT  0x1000

#define  BOOT_ROM_TOP   0x0FFF
#define  BOOT_ROM_BOT    0x0C00

#define  RAM_TOP     0x09ff
#define  RAM_BOT     0x0200
#define  PER16_TOP   0x01FF
#define  PER16_BOT   0x0100
#define  PER8_TOP    0x00ff
#define  PER8_BOT    0x0010
#define  SFR_TOP     0x000f
#define  SFR_BOT     0x0000

/*************************************************************************
*  WDT modes
**************************************************************************/
#define WDT_OFF     (0x5A00|WDTHOLD)

/*************************************************************************
*  Stack pointer
**************************************************************************/
#define  INIT_SP  (RAM_TOP+1)

/*************************************************************************
*  HFO clock sinks
**************************************************************************/
#define INIT_BCSCTL2 (SELM1|SELS)

/*************************************************************************
*  HFO stabilize timeout
**************************************************************************/
#define HFO_TIMEOUT  0x0FFF
  
/*************************************************************************
*  Errno codes
**************************************************************************/
#define  ERR_HFO  0x0001
#define  ERR_LCD  0x0002
#define  ERR_UART0   0x0004
#define  ERR_UART1   0x0008

/*************************************************************************
*  Errno codes
**************************************************************************/
/*
 * 5V stabilize <25ms the 3.3V doesn't matter because the MSP430 has
 * a supervisor ic on its reset line.  The 5V does because LCD init can't start
 * before the LCD has 4.5V to it, else it doesn't work.
 */
#define LCD_PS_WAIT 	0xEFFE



/*************************************************************************
*  Timer
**************************************************************************/ 
#define TACTL_INIT      TASSEL0|TACLR|ID_3
#define ONESECOND       512             

/*************************************************************************
*  Function prototypes
**************************************************************************/
void delay(unsigned int delayCount);
void InitHW(void);

#endif /* HW_INIT_H */
