/******************************************************************************
File: 		 sbfm.cpp
Tab stops: every 2 columns
Project:	 Flights of Fantasy
Copyright: 1993-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Erik Lorenzen and Keith Weiner
Purpose:	 Contains low-level sound code to drive the OPL-2 FM synthesizer chip
History:	 08/23/95 EL Started, based on opl.c; reworked for use by sound.cpp
					 09/04/95 KW Revised
					 10/06/95 EL Finalized
******************************************************************************/



#include <stdio.h>
#include <conio.h>


#include "types.h"					//generic types
#include "sbfm.h"					//include our own header



//On-Chip port address

#define WAVESELECT_TEST 	0x01 			// 1 per chip
#define TIMER1				0x02 			// 1 per chip (80  æsec resolution)
#define TIMER2				0x03 			// 1 per chip (320 æsec resolution)
#define IRQ_TIMER_CNTRL 	0x04 			// 1 per chip
#define CSM_SEL 			0x08 			// 1 per chip

#define SND_CHAR			0x20 			//18 per chip (one per operator)
#define LEVEL_OUTPUT		0x40 			//18 per chip (one per operator)
#define ATTACK_DECAY		0x60 			//18 per chip (one per operator)
#define SUSTAIN_RELEASE 	0x80 		//18 per chip (one per operator)
#define FNUML 				0xa0 			// 9 per chip (one per operator pair)
#define KEYON_BLOCK_FNUMH   0xb0 	// 9 per chip (one per operator pair)
#define DRUMCONTROLL		0xbd 			// 1 per chip
#define FEEDBACK_CON		0xc0 			// 9 per chip (one per operator pair)
#define WAVE_SELECT 		0xe0 			//18 per chip (one per operator)


//IO ports

#define STATUS			 0x388			//read-only
#define REGSEL			 0x388			//write-only

#define DATA			 0x389			//write-only


//Delays

#define SHORT 			 7					//aprox 3.3 æsec delay
#define LONG			 36 				//aprox 23 æsec delay
#define TIMEOUT 		 144				//aprox 92 æsec delay


//Bit-Masks/Commands

#define KEYON 			 0x20 			//key on mask
#define STARTTIMER1 	 0x21 			//start timer 1
#define WAVESEL 		 0x20 			//enable wave select
#define RESET_TIMER 	 0x60 			//timer reset
#define IRQ_ENABLE	 0x80 			//enable timer IRQ
#define KEYREL			 0xdf 			//key release mask
#define IRQ_BITMASK   0xe0          // for reading status
#define IRQ_TIMEDOUT  0xc0          // irq timer wound down

//Misc

#define NUMVOICES 			 9					//9 2-operator channels



//CHANNEL NUMBER		 #1 	 #2 	 #3 	 #4 	 #5 	 #6 	 #7 	 #8 	 #9
static ofst_op1[] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};
static ofst_op2[] = {0x03, 0x04, 0x05, 0x0b, 0x0c, 0x0d, 0x13, 0x14, 0x15};

static byte block_fnumh[NUMVOICES];



static void WasteTime(word numreads)
{
	while (numreads--)
	{
		inp(STATUS);			 //reading the status port causes
	} 								 //a timed delay
}


static void Write(unsigned FMreg, unsigned value)
{
	outp(REGSEL, FMreg); 	 //selects a register to write to
	WasteTime(SHORT);

	outp(DATA, value); 		 //outputs a value to the register
	WasteTime(LONG);
}


boolean sbfm_PresenceTest(void)
{
	int res;

	Write(IRQ_TIMER_CNTRL, RESET_TIMER); //reset timer
	Write(IRQ_TIMER_CNTRL, IRQ_ENABLE);  //enable timer IRQ
	res = inp(STATUS); 						 //get res after reset/before timeout

	Write(IRQ_TIMER_CNTRL, RESET_TIMER); //reset timer

			 //bit test :none of bits 5,6 or 7 should be set
	if((res&IRQ_BITMASK) != 0) 	{
		return (false); 						 //if they are, then fail
	}

	Write(TIMER1, 0xff);						 //write low value to timer1
	Write(IRQ_TIMER_CNTRL, STARTTIMER1); //write timer1 start mask
	WasteTime(TIMEOUT); 						 //wait long enough for timer to expire
	res = inp(STATUS);  						 //get res after a timeout

	Write(IRQ_TIMER_CNTRL, RESET_TIMER); //reset timer

				 //make sure IRQ masked bits are set to "timed out"
	if ((res&IRQ_BITMASK) != IRQ_TIMEDOUT) {
		return(false);		 //if not "timed out" fail
	}

	return(true); 											 //it seems that an FM chip is here
}


void sbfm_Reset(void)
{
	//this loop keys off all channels
	for (int channel=0;channel<NUMVOICES;channel++) {
		Write(LEVEL_OUTPUT + ofst_op1[channel], 0xff);	//1st attenuate
		Write(LEVEL_OUTPUT + ofst_op2[channel], 0xff);	//channels volume

		Write(ATTACK_DECAY + ofst_op1[channel], 0xff);	//2nd accelerate attack,
		Write(ATTACK_DECAY + ofst_op2[channel], 0xff);	//decay, sustain, and
																		//release, this will
		Write(SUSTAIN_RELEASE + ofst_op1[channel], 0x0f); //quite an channel
		Write(SUSTAIN_RELEASE + ofst_op2[channel], 0x0f); //down quickly

		Write(KEYON_BLOCK_FNUMH + channel, 0x00); 				//3rd key release
	} 																									//the channel

	//this will set chip to 9 channel mode
	Write(WAVESELECT_TEST, WAVESEL);		 //enable wave select
	Write(IRQ_TIMER_CNTRL, RESET_TIMER); //reset/mask timer
	Write(CSM_SEL,				 0x00); 			 //clear Composite Speech Mode
	Write(DRUMCONTROLL, 	 0x00); 			 //clear drum mode
}


void sbfm_KeyRelease(int channel)
{
	Write(KEYON_BLOCK_FNUMH + channel, block_fnumh[channel] & KEYREL);
}


void sbfm_KeyBend(int channel, word fnum)
{
	//store block_fnumh; we will need it for key release later
	block_fnumh[channel] = (byte)((fnum >> 8) | KEYON);

	Write(FNUML 						+ channel, (byte)fnum);
	Write(KEYON_BLOCK_FNUMH + channel, (byte)((fnum >> 8) | KEYON));
}


void sbfm_KeyOn(int channel, sbfm_INST *inst, word fnum)
{
	//store block_fnumh; we will need it for key release later
	block_fnumh[channel] = (byte)((fnum >> 8) | KEYON);

	//keyoff an instrument before programming the fm chip,
	//it helps minimize spurious sounds

	Write(LEVEL_OUTPUT + ofst_op1[channel], 0xff);			//attenuate
	Write(LEVEL_OUTPUT + ofst_op2[channel], 0xff);			//channels volume

	Write(KEYON_BLOCK_FNUMH + channel, 0x00); //key release

	// Wait until you're done re-programming a channel before keying
	//it on.	This helps to reduce spurious clicks, pops, and noises.

	//re-program operator 1
	Write(SND_CHAR				+ ofst_op1[channel], inst->op1soundchar);
	Write(LEVEL_OUTPUT		+ ofst_op1[channel], inst->op1level_output);
	Write(ATTACK_DECAY		+ ofst_op1[channel], inst->op1attack_decay);
	Write(SUSTAIN_RELEASE + ofst_op1[channel], inst->op1sustain_release);
	Write(WAVE_SELECT 		+ ofst_op1[channel], inst->op1wave_select);

	//re-program operator 2
	Write(SND_CHAR				+ ofst_op2[channel], inst->op2soundchar);
	Write(LEVEL_OUTPUT		+ ofst_op2[channel], inst->op2level_output);
	Write(ATTACK_DECAY		+ ofst_op2[channel], inst->op2attack_decay);
	Write(SUSTAIN_RELEASE + ofst_op2[channel], inst->op2sustain_release);
	Write(WAVE_SELECT 		+ ofst_op2[channel], inst->op2wave_select);

	//misc. characteristics
	Write(FEEDBACK_CON + channel, inst->op1feedback_con);
	Write(FNUML 			 + channel, (byte)fnum);

	//key it on
	Write(KEYON_BLOCK_FNUMH + channel, (byte)block_fnumh[channel]);

}
