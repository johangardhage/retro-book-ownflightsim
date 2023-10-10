/******************************************************************************
File: 		 playdwd.c
Version:	 2.20
Tab stops: every 2 columns
Project:	 DWD Player
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Keith Weiner & Erik Lorenzen
Purpose:	 Contains simple example code to show how to load/play a .DWD file
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
#include "err.h"



#define BUFFSIZE 65535		/* If the linker outputs the error */
													/* "stack plus data exceed 64K" */
													/* try reducing BUFFSIZE to about 32K, */
													/* or compile for large model. */
#ifdef __FLAT__
	byte sound[BUFFSIZE]; 	/* _far isn't need for p-mode */
#else
	byte _far sound[BUFFSIZE]; /* in r-mode the STK expects a _far ptr */
#endif


void main(int argc, char **argv)
{
	FILE								*fp;
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;
	dws_IDEAL 					ideal;
	dws_DPLAY 					dplay;
	word								result;

	printf("\nPLAYDWD 2.20 is Copyright 1994-95 DiamondWare, Ltd.\n");
	printf("All rights reserved.\n\n\n");

	if (argc < 2)
	{
		printf("Usage PLAYDWD <dwd-file> \n");
		exit(-1);
	}

	fp = fopen(argv[1], "rb");

	if (fp == NULL)
	{
		printf("Unable to open %s\n", argv[1]);
		exit(-1);
	}

	fread(sound, (size_t)BUFFSIZE, 1, fp);/* if filelen < BUFFSIZE, this works */

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
	}

	if (!(dres.capability & dws_capability_DIG))
	{
		if ((dres.baseport != 0x388) && (dres.baseport != (word)-1))
		{
			printf("The sound hardware supports digitized sound playback.\n");
			printf("We couldn't find the DMA channel and/or IRQ level.\n");
		}
		else
		{
			printf("DIG support not found\n");
		}

		exit(-1);
	}

	/*
	 . The "ideal" struct tells the STK how you'd like it to initialize the
	 . sound hardware.	In all cases, if the hardware won't support your
	 . request, the STK will go as close as possible.  For example, not all
	 . sound boards will support all sampling rates (some only support five or
	 . six discrete rates).
	*/
	ideal.musictyp	 = 0; 	 /* 0=No music, 1=OPL2 */
	ideal.digtyp		 = 8; 	 /* 0=No Dig, 8=8bit */
	ideal.dignvoices = 16;	 /* number of voices (up to 16) */
	ideal.dignchan	 = 1; 	 /* 1=mono */

	/* Set the ideal.digrate, in Hz */
	if (!dws_DGetRateFromDWD(sound, &ideal.digrate))
	{
		err_Display(dws_ErrNo(), err_DWS);
		goto KillIt;
	}

	if (!dws_Init(&dres, &ideal))
	{
		err_Display(dws_ErrNo(), err_DWS);
		exit(-1);
	}

	/* Set Master Volume to about 95% max */
	if (!dws_XMaster(242))
	{
		err_Display(dws_ErrNo(), err_DWS);
	}

	dplay.snd 		 = sound;
	dplay.count 	 = 1; 		 /* 0=infinite loop, else num times to play sound */
	dplay.priority = 1000;
	dplay.presnd	 = 0;

	if (!dws_DPlay(&dplay))
	{
		err_Display(dws_ErrNo(), err_DWS);
		goto KillIt;
	}

	do
	{
		if (!dws_DSoundStatus(dplay.soundnum, &result))
		{
			err_Display(dws_ErrNo(), err_DWS);
			goto KillIt;
		}

	} while (result);


	KillIt:

	if (!dws_Kill())
	{
		/*
		 . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		 . or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		 . must remove his tsr, and dws_Kill must be called again.	If it's
		 . dws_NOTINITTED, there's nothing to worry about at this point.
		*/
		err_Display(dws_ErrNo(), err_DWS);

		if (dws_ErrNo() == dws_Kill_CANTUNHOOKISR)
		{
			goto KillIt;
		}
	}
}
