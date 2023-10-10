/******************************************************************************
File: 		 playdsp.c
Version:	 2.20
Tab stops: every 2 columns
Project:	 DWD Player
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Erik Lorenzen
Purpose:	 Contains simple example code to show how to load/play a .DWD file
History:	 95/09/16 EL Converted to playdsp.c from playdwd.c
					 95/10/16 EL Finalized for 2.10
					 95/10/18 EL Finalized for 2.20, changed volume to 95%

Notes
-----
This code isn't really robust when it comes to standard error checking
and particularly recovery, software engineering technique, etc.  The STK will
handle songs larger than 64K (but not digitized sounds).	Obviously, you'd
need to fread() such a file in chunks, or write some sort of hfread() (huge
fread).  Also, exitting and cleanup is not handled robustly in this code.
The code below can only be validated by extremely careful scrutiny to make
sure each case is handled properly.  A better method would the use of C's
atexit function.

But all such code would make this example file less clear; its purpose was
to illustrate how to call the STK, not how to write QA-proof software.
******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>

#include "dws.h"
#include "err.h"
#include "dwdsp.h"



static void LoadFile(char *fname, byte **ptr, dword *len)
{
	FILE *fp=NULL;
	struct stat st;

	if (!stat(fname, &st))										 //get info on the file
	{
		*ptr = (byte *)malloc((size_t)st.st_size);

		if (*ptr == NULL)
		{
			printf("\nOutta memory pal!\n");
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

		if (fclose(fp))
		{
			printf("\nError Closeing File!!! \n");
			//don't need to goto RETFAIL
		}

		*len = (dword)st.st_size;

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


static void InitSTK(byte *sound)
{
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;
	dws_IDEAL 					ideal;

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

	if (!dws_DGetRateFromDWD(sound, &ideal.digrate)) //set ideal.digrate
	{
		err_Display(dws_ErrNo(), err_DWS);
		exit(-1);
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
}


static void KillSTK(void)
{
	if (!dws_Kill())
	{
		/*
		 . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		 . or dws_NOTINITTED.
		*/
		err_Display(dws_ErrNo(), err_DWS);

		if (dws_ErrNo() == dws_Kill_CANTUNHOOKISR)
		{
			/*
			 . Unlikely but if it's dws_Kill_CANTUNHOOKISR you might want to
			 . consider a way of allowing the user to handel the tsr and then
			 . try dws_kill again.
			*/
		}
	}
}


static word GetInput(word *volume, dword *length)
{
	word quit=0;
	word needkey;
	int key;

	do
	{
		needkey = 0;

		key = getch();

		switch (tolower(key))
		{
			case 'q':
			{
				quit = 1;
				break;
			}
			case 'f':
			{
				(*length)--;
				printf("Percent of original sound length %u\n", *length);
				break;
			}
			case 's':
			{
				(*length)++;
				printf("Percent of original sound length %u\n", *length);
				break;
			}
			case 'u':
			{
				(*volume) += 0x10;
				printf("Percent of original sound volume 0x%x\n", *volume);
				break;
			}
			case 'd':
			{
				(*volume) -= 0x10;
				printf("Percent of original sound volume 0x%x\n", *volume);
				break;
			}
			default:
			{
				needkey = 1;
			}
		}

	} while (needkey);

	return (quit);
}


static void DoDSPExample(byte *sound, dword srclen)
{
	dws_DPLAY dplay;
	byte *soundcopy;
	dword length=100; 												//full length
	word	volume=dwdsp_IDENTITY;							//full volume
	dword newlen;
	word quit;

	printf("q - quit\n");
	printf("f - faster\n");
	printf("s - slower\n");
	printf("u - vol up\n");
	printf("d - vol down\n");

	do
	{
		newlen = (srclen * length) / 100;

		soundcopy = (byte *)malloc((size_t)newlen);

		if (soundcopy == NULL)
		{
			printf("\n\nOutta memory pal!\n\n");
			goto KillIt;
		}

		if (!dwdsp_ChngLen(soundcopy, sound, newlen))
		{
			err_Display(dwdsp_ErrNo(), err_DWDSP);
			goto KillIt;
		}

		if (!dwdsp_ChngVol(soundcopy, soundcopy, volume))
		{
			err_Display(dwdsp_ErrNo(), err_DWDSP);
			goto KillIt;
		}

		dplay.snd 		 = soundcopy;
		dplay.count 	 = 0;
		dplay.priority = 1000;
		dplay.presnd	 = 0;

		if (!dws_DPlay(&dplay))
		{
			err_Display(dws_ErrNo(), err_DWS);
			goto KillIt;
		}

		quit = GetInput(&volume, &length);

		if (!dws_DDiscardAO(soundcopy))
		{
			err_Display(dws_ErrNo(), err_DWS);
			goto KillIt;
		}

		free (soundcopy);

	} while (!quit);

	KillIt:;
}


void main(int argc, char **argv)
{
	dword srclen;
	byte *sound;

	printf("\nPlaydsp 2.20 is Copyright 1994-95 DiamondWare, Ltd.\n");
	printf("All rights reserved.\n\n\n");

	if (argc < 2)
	{
		printf("Usage playdsp <dwd-file>\n");
		exit(-1);
	}

	LoadFile(argv[1], &sound, &srclen);
	InitSTK(sound);
	DoDSPExample(sound, srclen);
	KillSTK();
}
