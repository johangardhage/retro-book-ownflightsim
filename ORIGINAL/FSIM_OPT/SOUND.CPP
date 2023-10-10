/******************************************************************************
File: 		 sound.cpp
Tab stops: every 2 columns
Project:	 FSIM
Copyright: 1995 DiamondWare, Ltd.  All rights reserved.
Written:	 Erik Lorenzen and Keith Weiner
Purpose:	 Contains the high-level sound interface, callable by FoF Sim
History:	 08/24/95 EL 		Started
			 09/05/95 KW 		Revised
			 10/06/95 EL 		Finalized
			 10/10/95 mracky    tweak and update
			 10/26/95 mracky    fix sound on/off switch effect
******************************************************************************/

#include "types.h"							 // generic types
#include "sbfm.h" 							 // FM chip control
#include "sound.h"							 // header for this module


//CHANNEL allocation list

#define E1			 0	 //reserved for frequently used sounds
#define E2			 1	 //
#define E3			 2	 //
#define STL 		 3	 //

#define U1			 4	 //unreserved, switch between temporary
#define U2			 5	 //or occasionally used sounds
#define U3			 6	 //
#define U4			 7	 //
#define U5			 8	 //


//SFX bank list

#define ENG1		 0
#define ENG2		 1
#define ENG3		 2
#define STALL 	 3
#define CRASH 	 4
#define CRASH2	 5
#define SWITCH	 6

//FM stuff
#define NUMOCTAVES 8



typedef struct
{
	/* The top three members of the struct are used by varying sounds */
	byte			block;	//stores block number, needed to play variable sfx
	word			minf; 	//min freq needed for the instrument to sound good
	word			scale;	//constant to divide by, to determine freq
	sbfm_INST inst; 	//for static instruments only

} sound_SFX;



static sound_SFX sfx[]	=
{
	{ //ENG1 - varying sound
		0, 300, 3,
		{0xb1,0xa4,0xc0,0x80,0x6e,0xdf,0x17,0x0e,
		 0x00,0x00,0x08,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //ENG2 - varying sound
		1, 300, 4,
		{0x31,0xa1,0x43,0x00,0x6e,0xab,0x17,0x0c,
		 0x01,0x01,0x02,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //ENG3 - varying sound
		1, 400, 6,
		{0x00,0x11,0x02,0x80,0xf0,0xf0,0xff,0xff,
		 0x00,0x00,0x06,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //STALL - static sound
		0, 0, 0,
		{0x31,0xa1,0x43,0x00,0x6e,0xab,0x17,0x0c,
		 0x01,0x01,0x02,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //CRASH - static sound
		0, 0, 0,
		{0x11,0x01,0x85,0x00,0xb3,0xa1,0xe1,0xe4,
		 0x00,0x00,0x0a,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //CRASH2 - static sound
		0, 0, 0,
		{0x09,0x00,0x00,0x00,0xf1,0xf6,0xb3,0xb4,
		 0x00,0x00,0x0e,{0x00,0x00,0x00,0x00,0x00}}
	},
	{ //SWITCH - static sound
		0, 0, 0,
		{0x0e,0x0f,0x00,0x00,0xf8,0xf8,0xe6,0xf6,
		 0x00,0x00,0x00,{0x00,0x00,0x00,0x00,0x00}}
	}
};


static byte block[NUMOCTAVES] =
{
	0x0000, 0x0400, 0x0800, 0x0c00, 0x1000, 0x1400, 0x1800, 0x1c00
};


static boolean initted=false;
// REV 2.1 created a new bool to signify user toggles sound off 
//          (even though the sound card is present).  this corrects
//          the bug when along with moving the engstate variable from
//          outside the function, for triggering the sbfm_KeyOn calls.
static boolean user_ear = true;
static boolean engstate=false;		//engine starts off
static word lastrpm=0;


/*------------------------------------------------------------------------
| 												Sound functions 															 |
------------------------------------------------------------------------*/
static void PlaySwitch(void)
{
	sbfm_KeyOn(U1, &sfx[SWITCH].inst, 0x1d81);

	sbfm_KeyRelease(U1);
}


static boolean UpdateCrash(sound_CNTRL *cntrl)
{
	if (cntrl->crash == true)	{
		sbfm_KeyOn(U1, &sfx[CRASH].inst,	0x016b);
		sbfm_KeyOn(U2, &sfx[CRASH].inst,	0x01e5);
		sbfm_KeyOn(U3, &sfx[CRASH].inst,	0x0202);
		sbfm_KeyOn(U4, &sfx[CRASH].inst,	0x0120);
		sbfm_KeyOn(U5, &sfx[CRASH2].inst, 0x0141);

		if( engstate && lastrpm) {
			sbfm_KeyRelease(E1);
			sbfm_KeyRelease(E2);
			sbfm_KeyRelease(E3);
			sbfm_KeyRelease(STL);
		}
		sbfm_KeyRelease(U1);
		sbfm_KeyRelease(U2);
		sbfm_KeyRelease(U3);
		sbfm_KeyRelease(U4);
		sbfm_KeyRelease(U5);
		return true;
	}
	return false;
}


static void UpdateEngine(sound_CNTRL *cntrl)
{
	dword fnum;

	if (cntrl->engine != engstate) {		//if the engine state has changed
		PlaySwitch();						//figure out if it has turned on
											//or off and turn it sound on or
											//off accordinly
		if (engstate == false) {
			//Turn part of the engine noise on
			fnum = (dword)sfx[ENG3].minf;
			fnum |= block[sfx[ENG3].block];
			sbfm_KeyOn(E3, &sfx[ENG3].inst, (word)fnum);
		}
		else {
			//Turn part of the engine noise off
			sbfm_KeyRelease(E3);
		}
		engstate = cntrl->engine;
	}

	if (!cntrl->rpm && !lastrpm) {
		//No sound is produced for this condition
	}
	else if (cntrl->rpm && !lastrpm) {
		lastrpm = cntrl->rpm;

		//Turn other part of the engine noise on
		fnum = (dword)sfx[ENG1].minf;
		fnum |= block[sfx[ENG1].block];
		sbfm_KeyOn(E1, &sfx[ENG1].inst, (word)fnum);

		fnum = (dword)sfx[ENG2].minf;
		fnum |= block[sfx[ENG2].block];
		sbfm_KeyOn(E2, &sfx[ENG2].inst, (word)fnum);
	}
	else if (cntrl->rpm && lastrpm) {
		//Update the engine noise for a new rpm
		fnum = (dword)(sfx[ENG1].minf + (cntrl->rpm/sfx[ENG1].scale));
		fnum |= block[sfx[ENG1].block];
		sbfm_KeyBend(E1, (word)fnum);

		fnum = (dword)(sfx[ENG2].minf + (cntrl->rpm/sfx[ENG2].scale));
		fnum |= block[sfx[ENG2].block];
		sbfm_KeyBend(E2, (word)fnum);

		if (engstate == true) {
			//update only if engine is running & has > 0 rpm
			fnum = (dword)(sfx[ENG3].minf + (cntrl->rpm/sfx[ENG3].scale));
			fnum |= block[sfx[ENG3].block];
			sbfm_KeyBend(E3, (word)fnum);
		}
	}
	else  { //if (!cntrl->rpm && lastrpm)
		lastrpm = cntrl->rpm;

		//Turn part of the engine noise off
		sbfm_KeyRelease(E1);
		sbfm_KeyRelease(E2);
	}
}


static void UpdateStallB(sound_CNTRL *cntrl)
{
	static boolean laststall = false;

	if ((cntrl->stall == true) && (laststall == false)) {
		//Turn buzzer on
		laststall = true;
		sbfm_KeyOn(STALL, &sfx[STALL].inst, 0x8fae);
	}
	else if ((cntrl->stall == false) && (laststall == true)) {
		//Turn buzzer off
		laststall = false;
		sbfm_KeyRelease(STALL);
	}
	//else if ((cntrl->stall == false) && (laststall == false)) {
		//No action needed
	//}
	//else if ((cntrl->stall == true) && (laststall == true)) {
		//No action needed
	//}
}


static void UpdateBrake(sound_CNTRL *cntrl)
{
	static boolean brake = true;				//Plane starts with brakes on

	if (brake != cntrl->brake)	{
		brake = cntrl->brake;
		PlaySwitch();
	}

	if (brake != cntrl->brake)	{
		brake = cntrl->brake;
		PlaySwitch();
	}
}


/*------------------------------------------------------------------------
| 												Public functions															 |
------------------------------------------------------------------------*/
void sound_Update(sound_CNTRL *cntrl)
{
	if ((initted == true)	&&		//Make sure only to update sounds
	 	(user_ear == true))			//if the sound system has been inited
	{
		if( UpdateCrash(cntrl) ) // if crash , no need to do anything else
			return;
		UpdateEngine(cntrl);
		UpdateStallB(cntrl);
		UpdateBrake(cntrl);
	}
}


void sound_Kill()
{
	if( initted==true)
		sbfm_Reset();
	initted = false;
}

void sound_Off()
{
	user_ear = false;
	if( initted==true)
		sbfm_Reset();
}
void sound_On()
{
	user_ear = true;
	// if we are turning the sound back on 
	//   and the engine has already been on...
	if( initted == true) {
		engstate = false; // turn engine off
		lastrpm = 0;						
	}
}
void sound_Init(void)
{
	if (sbfm_PresenceTest() == true)
	{
		sbfm_Reset();
		initted = true;
		user_ear = true;
	}
}
