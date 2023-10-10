/******************************************************************************
File: 		 findsb.c
Version:	 2.20
Tab stops: every 2 columns
Project:	 FINDSB utility
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Keith Weiner & Erik Lorenzen
Purpose:	 Example code to autodetect and print out the sound hardware
History:	 94/10/21 KW		Started
					 95/02/01 KW/EL Finalized
					 95/03/22 EL		Finalized for 1.01
					 95/04/11 EL		Finalized for 1.02
					 95/06/13 EL		Finalized for 1.03, no changes
					 95/06/16 EL		Finalized for 2.00, no changes
					 95/10/05 EL		Finalized for 2.10, added modual support for err_
					 95/10/18 EL		Finalized for 2.20, no changes
******************************************************************************/



#include <stdio.h>
#include <stdlib.h>

#include "dws.h"
#include "err.h"



void main(void)
{
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;

	printf("\nFINDSB 2.20 is Copyright 1994-95 DiamondWare, Ltd.\n");
	printf("All rights reserved.\n\n\n");

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

	/* Test for FM or DIG */
	if ((dres.capability & dws_capability_FM) ||
			((dres.baseport != 0x388) && (dres.baseport != (word)-1)))
	{
		printf("Base port is %x hex.\n\n", dres.baseport);

		if (dres.mixtyp > 1)
		{
			printf("The sound hardware supports mixing.\n\n");
		}
		else
		{
			printf("Mixing will be done in software.\n\n");
		}

		if (dres.capability & dws_capability_FM)
		{
			printf("The sound hardware supports FM music playback.\n\n");
		}
		else
		{
			printf("Support for FM music playback not found.\n\n");
		}

		if (dres.capability & dws_capability_DIG)
		{
			/* If we got here dws_DetectHardWare got PORT, IRQ, & DMA */
			printf("The sound hardware supports digitized sound playback.\n\n");
			printf("The sound hardware uses DMA channel %d and IRQ level %d.\n\n",
						 dres.digdma, dres.digirq);
		}
		else if ((dres.baseport != 0x388) && (dres.baseport != (word)-1))
		{
			/*
			 . If dres.baseport isn't either 388hex, or -1, then it's a valid
			 . baseport.	So if we got here, then we didn't find either IRQ
			 . level, and/or DMA channel.  In order to play digitized sounds,
			 . we need these settings as well.	In your application, you should
			 . ask the user.
			*/
			printf("The sound hardware supports digitized sound playback,\n");
			printf("but we couldn't find the DMA channel and/or IRQ level.\n\n");
		}
		else
		{
			printf("Support for digitized playback not found.\n\n");
		}
	}
	else
	{
		printf("No sound hardware detected.\n\n");
	}
}
