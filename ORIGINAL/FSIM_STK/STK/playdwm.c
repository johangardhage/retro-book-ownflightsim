/******************************************************************************
File: 		 playdwm.c
Version:	 2.20
Tab stops: every 2 columns
Project:	 DWM Player
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Keith Weiner & Erik Lorenzen
Purpose:	 Contains simple example code to show how to load/play a .DWM file
History:	 94/10/21 KW Started
					 95/02/21 EL Finalized
					 95/03/22 EL Finalized for 1.01
					 95/04/11 EL Finalized for 1.02
					 95/06/16 EL Finalized for 1.03
					 95/06/16 EL Finalized for 2.00, #ifdef'd _far for protected-mode
					 95/10/05 EL Finalized for 2.10, added modual support for err_
					 95/10/18 EL Finalized for 2.20, changed vol's to 95%

Notes
-----
This code isn't really robust when it comes to standard error checking
and particularly recovery, software engineering technique, etc.  A buffer
is statically allocated.	A better technique would be to use fstat() or stat()
to determine the file's size then malloc(size).  The STK will handle songs
larger than 64K (but not digitized sounds).  Obviously, you'd need to fread()
such a file in chunks, or write some sort of hfread() (huge fread).  Also,
exitting and cleanup is not handled robustly in this code.	The code below can
only be validated by extremely careful scrutiny to make sure each case is
handled properly.  A better method would the use of C's atexit function.

But all such code would make this example file less clear; its purpose was
to illustrate how to call the STK, not how to write QA-proof software.
******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "dws.h"
#include "dwt.h"
#include "err.h"



#define BUFFSIZE 65535		/* If the linker outputs the error */
													/* "stack plus data exceed 64K" */
													/* try reducing BUFFSIZE to about 32K, */
													/* or compile for large model. */
#ifdef __FLAT__
	byte song[BUFFSIZE];		/* _far isn't need for p-mode */
#else
	byte _far song[BUFFSIZE]; /* in r-mode the STK expects a _far ptr */
#endif




void main(int argc, char **argv)
{
	FILE								*fp;
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;
	dws_IDEAL 					ideal;
	dws_MPLAY 					mplay;
	int 								input=0;
	word								musvol=255; 	/* Default mxr volume at startup is max */
	word								songstatus;

	printf("\nPLAYDWM 2.20 is Copyright 1994-95 DiamondWare, Ltd.\n");
	printf("All rights reserved.\n\n\n");

	if (argc < 2)
	{
		printf("Usage PLAYDWM <dwm-file>\n");
		exit(-1);
	}

	fp = fopen(argv[1], "rb");

	if (fp == NULL)
	{
		printf("Unable to open %s\n", argv[1]);
		exit(-1);
	}

	fread(song, (size_t)BUFFSIZE, 1, fp);  /* if filelen<BUFFSIZE, this works */

	fclose(fp);

	/*
	 . We need to set every field to -1 in dws_DETECTOVERRIDES struct;
	 . this tells the STK to autodetect everything.  Any other value
	 . overrides the autodetect routine, and will be accepted on
	 . faith, though the STK will verify it if possible.
	*/
	dov.baseport = (word)-1;
	dov.digdma	 = (word)-1;
	dov.digirq	 = (word)-1;

	if (!dws_DetectHardWare(&dov, &dres))
	{
		err_Display(dws_ErrNo(), err_DWS);
		exit(-1);
	}

	if (!(dres.capability & dws_capability_FM))
	{
		printf("FM support not found\n");
		exit(-1);
	}

	/*
	 . The "ideal" struct tells the STK how you'd like it to initialize the
	 . sound hardware.	In all cases, if the hardware won't support your
	 . request, the STK will go as close as possible.  For example, not all
	 . sound boards will support all sampling rates (some only support five or
	 . six discrete rates).
	*/
	ideal.musictyp	 = 1; 		/* for now, it's OPL2 music */
	ideal.digtyp		 = 0; 		/* 0=No Dig, 8=8bit, 16=16bit */
	ideal.digrate 	 = 0; 		/* sampling rate, in Hz */
	ideal.dignvoices = 0; 		/* number of voices (up to 16) */
	ideal.dignchan	 = 0; 		/* 1=mono, 2=stereo */

	if (!dws_Init(&dres, &ideal))
	{
		err_Display(dws_ErrNo(), err_DWS);
		exit(-1);
	}

	/*
	 . 72.8Hz is a decent compromise.  It will work in a Windows DOS box
	 . without any problems, and yet it allows music to sound pretty good.
	 . In my opinion, there's no reason to go lower than 72.8 (unless you
	 . don't want the hardware timer reprogrammed)--music sounds kinda chunky
	 . at lower rates.	You can go to 145.6 Hz, and get smoother (very
	 . subtly) sounding music, at the cost that it will NOT run at the correct
	 . (or constant) speed in a Windows DOS box.
	*/
	dwt_Init(dwt_72_8HZ);

	/* Set Music Volume to about 95% max */
	musvol = 242;

	if (!dws_XMusic(musvol))
	{
		err_Display(dws_ErrNo(), err_DWS);
	}

	mplay.track = song;
	mplay.count = 1;					/* 0=infinite loop, 1-N num times to play sound */

	if (!dws_MPlay(&mplay))
	{
		err_Display(dws_ErrNo(), err_DWS);
		goto KillIt;
	}

	/*
	 . We're playing.  Until the song is over, let's allow the user
	 . to fiddle with the volume level (mixer) in the meantime
	*/

	if (!dws_MSongStatus(&songstatus))
	{
		err_Display(dws_ErrNo(), err_DWS);
		goto KillIt;
	}

	printf("Press + or - to change playback volume \n");

	while (songstatus)
	{
		if (kbhit())
		{
			input = getch();
		}
		else
		{
			input = 0;
		}

		switch (input)
		{
			case 'q':
			case 'Q':
			case 27:								/* ESC */
			{
				if (!dws_MClear())
				{
					err_Display(dws_ErrNo(), err_DWS);
				}

				break;
			}
			case '+':
			{
				musvol++;

				printf("Music Volume is %d\n", musvol);

				if (!dws_XMusic(musvol))
				{
					err_Display(dws_ErrNo(), err_DWS);
				}

				break;
			}
			case '-':
			{
				musvol--;

				printf("Music Volume is %d\n", musvol);

				if (!dws_XMusic(musvol))
				{
					err_Display(dws_ErrNo(), err_DWS);
				}

				break;
			}
		}

		if (!dws_MSongStatus(&songstatus))
		{
			err_Display(dws_ErrNo(), err_DWS);
			goto KillIt;
		}
	}

	KillIt:

	/* If dwt is not inited calling dwt_Kill will have no effect */
	dwt_Kill();

	if (!dws_Kill())
	{
		/*
		 . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		 . or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		 . must remove his tsr, and dws_Kill must be called again.	If it's
		 . dws_NOTINITTED, there's nothing to worry about at this point.
		*/
		err_Display(dws_ErrNo(), err_DWS);

		if (dws_ErrNo() ==	dws_Kill_CANTUNHOOKISR)
		{
			goto KillIt;
		}
	}
}
