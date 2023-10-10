/******************************************************************************
File: 	  soundstk.cpp
Tab stops: every 3 columns
Project:   FSIM
Copyright: 1995 DiamondWare, Ltd.  All rights reserved.
Written:   Erik Lorenzen and Keith Weiner
Purpose:   Contains the high-level sound interface, callable by FoF Sim
History:   08/24/95 EL Started
			  09/05/95 KW Revised
			  10/06/95 EL Finalized
******************************************************************************/



#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>


#include "types.h"									//generic types
#include "sound.h"									//header for this module


#define  stddef_INCLUDE
#include "dws.h"
#include "dwt.h"
#include "err.h"
#include "dwdsp.h"


static byte *song;
static byte *starter;
static byte *swtch;
static byte *windorg;
static byte *wind;
static byte *stallbuz;
static byte *planeorg;
static byte *plane1;
static byte *plane2;
static byte *backfire;
static byte *landing;
static byte *brake;
static byte *crash;

static dws_DETECTRESULTS dres;
static boolean initted = false;



static void FreeIt(byte **ptr)
{
	if (ptr != NULL)
	{
		free(*ptr);
		*ptr = NULL;
	}
}


static void LoadIt(char *fname, byte **ptr)
{
	FILE *fp;
	struct stat st;

	if (*ptr != NULL)
	{
		printf("\nPtr in use already buddy!\n");
		return;
	}

	if (!stat(fname, &st))							//get info on the file
	{
		*ptr = (byte *)malloc((size_t)st.st_size);

		if (*ptr == NULL)
		{
			printf("\nOuta memory pal.\n");
			goto RETFAIL;
		}

		fp = fopen(fname, "rb");

		if (fp == NULL)
		{
			printf("\nUnable to open %s\n",fname);
			goto RETFAIL;
		}

		if (fread(*ptr, (word)st.st_size, 1, fp) != 1)
		{
			printf("\nError Reading File!!! \n");
			goto RETFAIL;
		}

		if (fp != NULL)
		{
			if (fclose(fp))
			{
				printf("\nError Closeing File!!! \n");
				//don't need to RETFAIL
			}
		}

		return;
	}
	else
	{
		return;
	}

	RETFAIL:

	if (fp != NULL)
	{
		if (fclose(fp))
		{
			printf("\nError Closeing File!!! \n");
		}
	}

	if (*ptr != NULL)
	{
		free(*ptr);
	}
}


static void PlaySwtch(void)
{
	dws_DPLAY dplay;

	dplay.snd		= swtch;
	dplay.count 	= 1;
	dplay.priority = 5000;
	dplay.presnd	= 0;


	if (!dws_DDiscardAO(dplay.snd))				//make sure only one switch noise
	{														//occurs at a time
		err_Display(dws_ErrNo(), err_DWS);
	}

	if (!dws_DPlay(&dplay)) 						//play one from scratch
	{
		err_Display(dws_ErrNo(), err_DWS);
	}
}


static void PlayStarter(void)
{
	dws_DPLAY dplay;

	dplay.snd		= starter;						//set up ptr to snd
	dplay.count 	= 1;								//play it once
	dplay.priority = 5000;							//the startup of the plane is
															//loud and temporary
	dplay.presnd	= 0;								//start it now

	if (!dws_DPlay(&dplay)) 						//play the startup
	{
		err_Display(dws_ErrNo(), err_DWS);
	}
}


static void UpdateCrash(sound_CNTRL *cntrl)
{
	dws_DPLAY dplay;

	dplay.snd		= crash; 						//set up ptr to snd
	dplay.count 	= 1;								//play it 1 time
	dplay.priority = 5000;							//give it it priority
	dplay.presnd	= 0;								//start it now

	if (cntrl->crash == true)
	{
		if (!dws_DClear())							//clear all other digitized sounds
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		if (!dws_DPlay(&dplay)) 					//play a crash
		{
			err_Display(dws_ErrNo(), err_DWS);
		}
	}
}


static void UpdateSwitches(sound_CNTRL *cntrl)
{
	static boolean lastbrake = true; 			//brakes start on
	static boolean engstate  = false;			//engine starts off

	if (lastbrake != cntrl->brake)				//brake switch was hit
	{
		PlaySwtch();

		lastbrake = cntrl->brake;					//update state
	}

	if (cntrl->engine != engstate)				//engine switch was hit
	{
		PlaySwtch();

		engstate = cntrl->engine;					//update state
	}
}


static void UpdateBrake(sound_CNTRL *cntrl)
{
	static boolean soundplayed = false; 		//no
	static boolean lastbrake	= true;			//Plane starts with brakes on

	dws_DPLAY dplay;

	dplay.snd		= brake; 						//set up ptr to snd
	dplay.count 	= 1;								//set up for 1 rep
	dplay.priority = 3000;							//give it a priority
	dplay.presnd	= 0;								//start it immediatly

	if ((lastbrake == false)	 &&				//brake was just hit
		 (cntrl->brake == true)  &&				//while plane on ground
		 (cntrl->airborne == false))
	{
		if (!dws_DPlay(&dplay)) 					//play a brake noise
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		soundplayed = true;							//mark sound as played
	}

	if (
			 (soundplayed == true) &&				//sound was played and
			 (
				 (cntrl->airspeed == 0) 	 ||	//plane has stopped or
				 (cntrl->brake 	== false)|| 	//brake has turned off or
				 (cntrl->airborne == true) 		//plane went airborne
			 )
		 )
	{
		if (!dws_DDiscardAO(dplay.snd))			//stop any landing noises
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		soundplayed = false; 						//mark sound as not played
	}

	lastbrake = cntrl->brake;						//update status
}


static void UpdateLanding(sound_CNTRL *cntrl)
{
	static boolean lastairborne = false;		//plane starts on the ground
	static boolean soundplayed	= false;			//no

	dws_DPLAY dplay;

	dplay.snd		 = landing; 					//set up ptr to snd
	dplay.count 	 = 1; 							//set up for 1 rep
	dplay.priority = 3000;							//give it a priority
	dplay.presnd	 = 0; 							//start it immediatly

	if ((cntrl->brake 	 == true)	 &&		//landing w/ brakes on
			(lastairborne 	 == true) 	 &&
			(cntrl->airborne == false))
	{
		if (!dws_DPlay(&dplay)) 					//play a landing noise
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		soundplayed = true;							//mark sound as played
	}

	if (
			 (soundplayed == true) &&				//sound was played and
			 (
				 (cntrl->airspeed == 0) 	 ||	//plane has stopped or
				 (cntrl->brake 	== false) ||	//brake has turned off or
				 (cntrl->airborne == true) 		//plane went airborne
			 )
		 )
	{
		if (!dws_DDiscardAO(dplay.snd))			//stop any landing noises
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		soundplayed = false; 						//mark sound as not played
	}

	lastairborne = cntrl->airborne;				//update state
}


static void UpdateStallB(sound_CNTRL *cntrl)
{
	static boolean laststall = false;
	dws_DPLAY dplay;

	dplay.snd		= stallbuz; 					//set up ptr to snd
	dplay.count 	= 0;								//set up infinite loop
	dplay.priority = 3000;							//give it a priority
	dplay.presnd	= 0;								//start it immediatly

	if ((cntrl->stall == false) && (laststall == false))
	{
		//No action needed, buzzer still off
	}
	else if ((cntrl->stall == true) && (laststall == false))
	{
		if (!dws_DPlay(&dplay)) 					//Turn buzzer on
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		laststall = true;
	}
	else if ((cntrl->stall == true) && (laststall == true))
	{
		//No action needed, buzzer still on
	}
	else //if ((cntrl->stall == false) && (laststall == true))
	{
		if (!dws_DDiscardAO(dplay.snd))			//Turn buzzer off
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		laststall = false;
	}
}


static void UpdateEngine(sound_CNTRL *cntrl)
{
	/*
	 . Chosing the correct engine sound is critical.
	 .	 1) a short sound helps us save space & CPU (DSP takes some CPU)
	 .	 2) more pitch adjustments sound better but cause a slow down.
	 .
	 . We must balance the size of sound, number of repeats, and number
	 . of pitch adjustments to obtain the optimal results.
	*/
	static word presndnum		= 0;				//start it immediatly
	static word lastrpm			= 0;
	static boolean soundplayed = false; 		//no

	/*
	 . Set up count for 1 rep; only fast machines will be able to keep up,
	 . but we will adjust count on the fly until it works for the
	 . slowest machines.	This will give us the best sound quality possible
	 . on machines that can handle it.
	*/
	static word count = 1;

	dword srclen = 1424;
	dword newlen;
	word result;
	word volume;

	dws_DPLAY dplay;

	//Set up dplay struct for a stall buzzer
	dplay.snd		 = plane1;		//set up ptr to snd
	dplay.priority  = 2000; 		//give it it priority
	dplay.presnd	 = presndnum;	//start it after presndnum
											// if presndnum == 0, start it imediatly
											// if presndnum != 0, and sound is not done
											// 	playingsound, the new sound gets sequenced
											// 	to the end of old sound and starts playing
											// 	after the old sound is done playing
											// if presndnum != 0, sound is done sound
											// 	starts right away

	if (!cntrl->rpm && !lastrpm)					//engine still off
	{
		//No action needed
	}
	else if (cntrl->rpm && !lastrpm) 			//engine has turned on
	{
		PlayStarter(); 								//turn engine on
	}
	else if (cntrl->rpm && lastrpm)				//engine still running
	{
		//Keep engine running, adjust engine rpm, & adjust loudness

		if (!dws_DSoundStatus(presndnum, &result))
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		if ((soundplayed == true) &&				//engine sound has
			 (!(result & dws_DSOUNDSTATUSPLAYING))) //run out and died
		{
			count++; 									//play it longer next time
		}

		if (!(result & dws_DSOUNDSTATUSSEQUENCED)) //engine sound is
		{													//waiting for a sequence
			/*
			 . 1) To get here the sound is either not playing -or-
			 .		playing and not sequenced.
			 . 2) Once we get here we will need to sequence a sound
			 . 3) Because of the relatively few times we get here, we will
			 .		not spend any extra time scaling volume/pitch on a sound
			 .		and then not use it
			*/
			if (lastrpm != cntrl->rpm) 			//check to see if pitch
			{												//needs updateing
				/*
				 . Sequence another of the previous sound, if we spend
				 . too much time doing DSP this will play, if we finish in time
				 . the new sound will replace it.
				*/
				dplay.count = count;

				if (!dws_DPlay(&dplay))
				{
					err_Display(dws_ErrNo(), err_DWS);
				}

				dplay.snd = plane2;					//use as place holder
				plane2 = plane1;						//swap pointers around
				plane1 = dplay.snd;


				/*
				 . NB: plane.dwd is the biggest possible size we will need it
				 . to be (lowest frequency).	If you attempt to do anything but
				 . but shorten the sound you will need to allocate more memory.
				*/
				newlen  = srclen; 					//size of sound
				newlen -= cntrl->rpm / 3;			//the more rpm's the shorter
															//the sound will be
				if (!dwdsp_ChngLen(plane1, planeorg, newlen))
				{
					err_Display(dwdsp_ErrNo(), err_DWDSP);
				}

				volume  = cntrl->tpos;				//vary engine loudness based
															//on throtle position
				volume *= 8;							//scale by some factor
				volume += 128; 						//add a minimum volume

				if (!dwdsp_ChngVol(plane1, plane1, volume))
				{
					err_Display(dwdsp_ErrNo(), err_DWDSP);
				}
			}

			dplay.count = count; 					//see above comment for count

			if (!dws_DPlay(&dplay)) 				//Play the sound
			{
				err_Display(dws_ErrNo(), err_DWS);
			}
			else
			{
				/*
				 . Call succeded, dws_DPlay returns a snd number from 10-65535
				 . if the previous sound is still playing, dplay.soundnum will
				 . remain the same, if its done playing we will be assigned a new
				 . number.
				*/
				presndnum = dplay.soundnum;

				soundplayed = true;					//mark sound as played
			}
		}
	}
	else //if (!cntrl->rpm && lastrpm)			//engine just turned off
	{
		if (!dws_DDiscardAO(plane1))				//make sure engine stops
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		if (!dws_DDiscardAO(plane2))				//make sure engine stops
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		soundplayed = false; 						//mark sound as not playing
	}

	lastrpm = cntrl->rpm;
}


static void UpdateWind(sound_CNTRL *cntrl)
{
	static word lastairspeed = 0; 				//plane starts at 0 airspeed
	dws_DPLAY dplay;

	dplay.snd		= wind;							//set up ptr to snd
	dplay.count 	= 0;								//set up infinite loop
	dplay.priority = 500;							//give it it priority
	dplay.presnd	= 0;								//start it now

	/*
	 . Please note that in order to conserve memory, UpdateWind uses
	 . a 2 buffer system.  We update the copy sound in place while it's playing.
	 . This works fine as long as the volume change is not too great and
	 . the real-mode STK is used.
	 .
	 . For an example of how to do it in p-mode, see UpdateEngine, it uses
	 . a multi-buffer system & sequences sounds.
	*/

	if ((cntrl->airspeed > 0) && (lastairspeed == 0)) //plane started moving
	{
		if (!dwdsp_ChngVol(wind, windorg, cntrl->airspeed)) //change wind volume
		{
			err_Display(dwdsp_ErrNo(), err_DWDSP);
		}

		if (!dws_DPlay(&dplay)) 					//start wind sound
		{
			err_Display(dws_ErrNo(), err_DWS);
		}
	}
	else if (cntrl->airspeed != lastairspeed) //MPH has changed
	{
		if (!dwdsp_ChngVol(wind, windorg, cntrl->airspeed)) //change wind volume
		{
			err_Display(dwdsp_ErrNo(), err_DWDSP);
		}
	}
	else if ((cntrl->airspeed == 0) && (lastairspeed == 0)) //plane stopped
	{
		if (!dws_DDiscardAO(dplay.snd))			//turn wind off
		{
			err_Display(dws_ErrNo(), err_DWS);
		}
	}

	lastairspeed = cntrl->airspeed;				//update state
}


static void _cdecl Kill(void)
{
	if(initted == true)
	{
		if(!(dws_Kill())) 							//kill sound modual
		{
			err_Display(dws_ErrNo(), err_DWS);
		}

		FreeIt(&song); 								//free all allocated mem
		FreeIt(&planeorg);
		FreeIt(&plane1);
		FreeIt(&plane2);
		FreeIt(&starter);
		FreeIt(&backfire);
		FreeIt(&landing);
		FreeIt(&brake);
		FreeIt(&crash);
		FreeIt(&windorg);
		FreeIt(&wind);
		FreeIt(&swtch);
		FreeIt(&stallbuz);
	}
}


/*------------------------------------------------------------------------
-  Public functions
------------------------------------------------------------------------*/
void sound_Update(sound_CNTRL *cntrl)
{
	if (initted == true)
	{
		if (dres.capability & dws_capability_DIG)
		{
			//Only do Updates if digitized sound is available

			if (cntrl->sound == false)
			{
				dws_XMaster(0);
			}
			else
			{
				dws_XMaster(242);
			}

			UpdateEngine(cntrl);
			UpdateBrake(cntrl);
			UpdateLanding(cntrl);
			UpdateStallB(cntrl);
			UpdateCrash(cntrl);
			UpdateWind(cntrl);
			UpdateSwitches(cntrl);
		}
	}
}


void sound_Init(void)
{
	dws_DETECTOVERRIDES dov;
	dws_IDEAL			  ideal;
	dws_MPLAY			  mplay;

	if (initted == false)
	{
		song		= NULL;
		planeorg = NULL;
		plane1	= NULL;
		plane2	= NULL;
		starter	= NULL;
		backfire = NULL;
		landing	= NULL;
		brake 	= NULL;
		crash 	= NULL;
		windorg	= NULL;
		wind		= NULL;
		swtch 	= NULL;
		stallbuz = NULL;

		dov.baseport = (word)-1;
		dov.digdma	 = (word)-1;
		dov.digirq	 = (word)-1;

		if (dws_DetectHardWare(&dov, &dres))
		{
			ideal.musictyp   = 1;						//0=No Music, 1=OPL2
			ideal.digtyp	  = 8;						//0=No Dig, 8=8bit
			ideal.digrate	  = 11000;					//sampling rate, in Hz
			ideal.dignvoices = 16;						//1 - 16 channels
			ideal.dignchan   = 1;						//1=mono

			if (dws_Init(&dres, &ideal))
			{
				initted = true;

				atexit(Kill);

																		//Where did the mem go?
																		//How to use less?
																		//
				LoadIt("music\\moon2.dwm", 	&song);		// 36,912 x
				LoadIt("sound\\starter.dwd",	&starter);	//  9,792 *
				LoadIt("sound\\swtch.dwd", 	&swtch); 	//  1,762 *
				LoadIt("sound\\wind.dwd",		&windorg);	// 10,240 +
				LoadIt("sound\\wind.dwd",		&wind);		// 10,240 +
				LoadIt("sound\\stallbuz.dwd", &stallbuz); // 	675
				LoadIt("sound\\plane.dwd", 	&planeorg); //  1,424
				LoadIt("sound\\plane.dwd", 	&plane1);	//  1,424
				LoadIt("sound\\plane.dwd", 	&plane2);	//  1,424
				LoadIt("sound\\brake.dwd", 	&brake); 	// 10,816 *+
				LoadIt("sound\\landing.dwd",	&landing);	//  8,704 *
				LoadIt("sound\\crash.dwd", 	&crash); 	//  6,188 *+
																		// 10,000 Other snd code?
																		//+47,212 @ SW STK
																		//--------
																		// 156,813 Total memory
																		//
																		// * could play from disk
																		// + could try to shorten
																		// x get a shorter dwm
																		// @ the registerd ver
																		//   is smaller

				//starter is too loud, make it quiter
				if (!dwdsp_ChngVol(starter, starter, 160))
				{
					err_Display(dwdsp_ErrNo(), err_DWDSP);
				}

				mplay.track = song;
				mplay.count = 0;

				if (!dws_MPlay(&mplay))
				{
					err_Display(dws_ErrNo(), err_DWS);
				}

				if (!dws_XMusic(190))
				{
					err_Display(dws_ErrNo(), err_DWS);
				}
			}
			else
			{
				err_Display(dws_ErrNo(), err_DWS);
			}
		}
		else
		{
			err_Display(dws_ErrNo(), err_DWS);
		}
	}
}
