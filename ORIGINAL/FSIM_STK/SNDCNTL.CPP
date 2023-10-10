// SNDCNTL.CPP
//
//
#include <dos.h>
#include "types.h"						// generic types
#include "aircraft.h"					// need AirCraft definition
#include "sound.h"
#include "sndcntl.h" 					// header for this module


static boolean sound_on = true;		// de-toggles svct.sound_chng
												// it will now be either on or off

// transfer the data form the aircraft to the sound control structure
// then call sound update.  Note that the crash field is set
// previous to calling ExtractSoundData
static void ExtractSoundData(sound_CNTRL& cntrl, AirCraft& ac)
{
	if(ac.sound_chng == true)	{
		sound_on = sound_on ? false:true;
		ac.sound_chng = false;
	}

	cntrl.rpm		= (word)ac.rpm;
	cntrl.tpos		= (word)ac.throttle_pos;
	cntrl.airspeed = (word)ac.h_speed;
	cntrl.engine	= ac.engine_on;
	cntrl.airborne = ac.airborne;
	cntrl.stall 	= ac.stall;
	cntrl.brake 	= ac.brake;
	cntrl.sound 	= sound_on;

	sound_Update(&cntrl);
}

void SoundCheck(AirCraft &svct)
{
	sound_CNTRL cntrl;
	cntrl.crash = false;
	ExtractSoundData(cntrl,svct);
}


void SoundCrash(AirCraft& svct)
{
	sound_CNTRL cntrl;
	cntrl.crash	= true;
	ExtractSoundData(cntrl,svct);
}
