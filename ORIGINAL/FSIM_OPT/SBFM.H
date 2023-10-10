/******************************************************************************
File: 		 sbfm.h
Tab stops: every 2 columns
Project:	 Flights of Fantasy
Copyright: 1993-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Erik Lorenzen and Keith Weiner
Purpose:	 Contains low-level sound code to drive the OPL-2 FM synthesizer chip
History:	 08/23/95 EL Started, based on opl.c; reworked for use by sound.cpp
					 09/04/95 KW Revised
					 10/06/95 EL Finalized
******************************************************************************/



#ifndef __sbfm__


	#define __sbfm__



	typedef struct
	{
		byte op1soundchar;
		byte op2soundchar;

		byte op1level_output;
		byte op2level_output;

		byte op1attack_decay;
		byte op2attack_decay;

		byte op1sustain_release;
		byte op2sustain_release;

		byte op1wave_select;
		byte op2wave_select;

		byte op1feedback_con;

		byte reserved[5];

	} sbfm_INST;



	boolean sbfm_PresenceTest(void);

	void sbfm_Reset(void);

	void sbfm_KeyOn(int channel, sbfm_INST *inst, word fnum);

	void sbfm_KeyBend(int channel, word newfnum);

	void sbfm_KeyRelease(int channel);



#endif
