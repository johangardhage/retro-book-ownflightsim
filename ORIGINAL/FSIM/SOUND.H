/******************************************************************************
File: 	  sound.h
Tab stops: every 2 columns
Project:   Flights of Fantasy
Copyright: 1995 DiamondWare, Ltd.  All rights reserved.
Written:   Erik Lorenzen and Keith Weiner
Purpose:   Contains the high-level sound interface, callable by FoF Sim
History:   08/24/95 EL Started
			  09/05/95 KW Revised
			  10/06/95 EL Finalized
******************************************************************************/



#ifndef SOUND_H

#define SOUND_H

#ifndef TYPES_H
#include "types.h"
#endif


typedef struct  {
	/*
	 . The sound code relies on this struct being up-to-date!!!
	 . It also relies on it being modeled after the real-word,
	 . you cant crash a plane and leave its motor running.
	*/
	word rpm;											//rpm of engine (on or off)
	word tpos;											//throttle position
	word airspeed;
	boolean engine;									//true if engine running
	boolean airborne; 								//true if the plane has taken off
	boolean stall; 									//true if stall condition
	boolean brake; 									//true if brake on
	boolean crash; 									//true if plane is crashing
	boolean sound; 									//true if sound is on

} sound_CNTRL;



void sound_Init(void);								//Sets up module for use
void sound_Update(sound_CNTRL *cntrl);



#endif
