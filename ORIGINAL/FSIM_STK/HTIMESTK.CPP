/******************************************************************************
File: 	  htimestk.cpp
Tab stops: every 3 columns
Project:   FSIM
Copyright: 1995 DiamondWare, Ltd.  All rights reserved. *
Written:   Erik Lorenzen
Purpose:   Contains a C++ wrapper for DWT, providing an HTimer-like interface
History:   10/07/95 EL Started (based on htime.cpp)
			  10/30/95 EL Finalized

* A section of this code is copied wholesale from htime.cpp.
******************************************************************************/



#include <dos.h>
#include <stdio.h>

#ifndef __TYPES__
    #include "types.h"
#endif
#ifndef __HTIMER__
    #include "htimer.h"
#endif

#define stddef_INCLUDE
#include "dws.h"
#include "dwt.h"



//initialization of static member data
word HTimer::numTimers = 0;						//instance counter

//pointer to low word of bios time of day counter at 0040:006C
static volatile word far* bios_ticks = (word far*)MK_FP(0x40, 0x6c);


/******************************************************************************/
/*
 . NB: This is here for compatibility reasons, but never gets used!
 .
*/
dword HTimer::ticks = 0;
word HTimer::countVal = 0;

void interrupt HTimer::int8Handler(...)
{
	//Stub this method, because it's in the header file
}
/******************************************************************************/


//Constructor for HTimer class
HTimer::HTimer()
{
	if (numTimers == 0)								//is this the first instance?
	{
		dwt_Init(dwt_72_8HZ);
	}

	numTimers++;										//increment instance counter
}


//Destructor for HTimer class.
HTimer::~HTimer()
{
	if (numTimers == 1)
	{
		dwt_Kill();
	}

	numTimers--;
}


//this function is used to calculate the elapsed time in microseconds
dword HTimer::calcElapsed()
{
	dword elapsedTime;

	elapsedTime  = dwt_MasterTick();
	elapsedTime -= startTick;
	elapsedTime *= (dword)(1000000.0 / 72.8); //1sec(in microseconds)/DWT rate

	return(elapsedTime);
}


//timerOn() starts the timing process
void HTimer::timerOn()
{
	startTick = dwt_MasterTick();
}


dword HTimer::timerOff()
{
	/*
	 . If it looks like the timer isn't being turned off your right!
	 . It will get turned off when destructed.	Make sure not to call
	 . getElapsed() after timerOff() as its results are technically
	 . undefined.
	*/

	return(calcElapsed());
}


//this function returns the elapsed time for the current timing process in
//microseconds. The timer continues to run after the function returns
dword HTimer::getElapsed()
{
	return(calcElapsed());							//calc and return elapsed time
}


//The HTimer::setCount() function is used to reprogram the timer rate
void HTimer::setCount(word regCount)
{
	/*
	 . This function is not supported, although DWT could be reinnited to
	 . use a new rate.	DWT does not support timer rates which would cause
	 . the DOS timer to become erratic
	 .
	 . This is how to set DWT for a new rate, beware it will cause any music
	 . that is playing to change rate.
	 .	 dwt_Kill();
	 .	 dwt_Init(NEWRATE);
	*/

	regCount++;
}


//**************************************************************************


/*
 . NB: This section is basically copied wholesale from htime.cpp
*/


//-------+---------+---------+---------+---------+---------+---------+---------+
// 1991-1992 Betz Associates. Released to the Public Domain. This source code
// is offered without warranty or guarantee of any kind, and the author(s)
// expressly disclaim any responsibility for consequences of it's use in
// products for commercial, shareware, or public domain release of any kind.
//
// File Name: HTIMER.H
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  nothing
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          Initial release
//      9-20-1992       1.1     MB          Update release v 1.1
//
// Description
// -----------
//      Header file for the hi-res multi-channel timer
//
//-------+---------+---------+---------+---------+---------+---------+---------+


//a test frame for the timer object; these three functions time the number
//of microseconds in one tick of the bios time counter. Since this counter
//"ticks" at a frequency of approx. 18.2 hz, we should expect 54,945.05
//microseconds per tick. The first times the interval by turning the timer
//on, then off. The second performs the same test using the polling function.
//The timer continues to run. The third sets the timer to interrupt at a
//custom rate, and then times a bios tick to make sure that the higher
//hardware interrupt rate does not effect the timing.

//tests elapsed time by turning timer on, then off
dword testTimerOnOff()
{
	HTimer timer1; 									//timer object for test
	dword etime;
	word lastTick;

	lastTick = *bios_ticks; 						//grab current time tick
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	lastTick = *bios_ticks; 						//it changed, grab the new value
	timer1.timerOn(); 								//turn on the timer
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	etime = timer1.timerOff(); 					//turn off the timer
															//value should be about 54,945
	return(etime);
}


//tests elapsed time by turning timer off, then calling HTimer::getElapsed.
dword testGetElapsed()
{
	HTimer timer1; 									//timer object for test
	dword etime;
	word lastTick;

	lastTick = *bios_ticks; 						//grab current time tick
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	lastTick = *bios_ticks; 						//it changed, grab the new value
	timer1.timerOn(); 								//turn on the timer
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	etime = timer1.getElapsed();					//turn off the timer
															//value should be about 54,945
	timer1.timerOff();
	return(etime);
}


//this function first sets the channel 1 counter value to intRate, which
//changes the frequency at which interrupts are generated by the time on
//IRQ 0. It then times one bios time tick exactly as the previous two
//functions do. If caller of this function has set up an isr for interrupt
//8, and is calling the original system isr for this interrupt at the
//proper frequency, we will expect to get the same value as with the other
//two test functions. Otherwise, this function will return the actual time
//between calls to the original interrupt handler.

dword testFastCount( word intRate )
{
	HTimer timer1; 									//timer object for test
	dword etime;
	word lastTick;

	timer1.setCount( intRate );					//alter interrupt rate
	lastTick = *bios_ticks; 						//grab current time tick
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	lastTick = *bios_ticks; 						//it changed, grab the new value
	timer1.timerOn(); 								//turn on the timer
	while( lastTick == *bios_ticks ){;} 		//wait for it to change
	etime = timer1.getElapsed();					//turn off the timer
															//value should be about 54,945
	timer1.setCount( 0 );							//reset int rate to default
	return(etime);
}


//**************************************************************************
