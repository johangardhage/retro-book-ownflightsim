/******************************************************************************
File: 		 dwt.h
Version:	 2.20
Tab stops: every 2 columns
Project:	 DiamondWare's Sound ToolKit
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:	 Keith Weiner and Erik Lorenzen
Purpose:	 Contains declarations for the DW Timer module
History:	 94/08/24 KW Started
					 95/02/17 EL Finalized for 1.00
					 95/04/12 EL Finalized for 1.02
					 95/06/16 EL Finalized for 1.03
					 95/07/23 EL Finalized for 1.04, no changes
					 95/08/01 EL Finalized for 2.00, added new defines for pmode errors
																					 & calling conventions
					 95/08/29 EL Finalized for 2.10, no changes
					 95/10/27 EL Finalized for 2.20, added #include "dws.h"

NB: This code is not compatible with source profilers
******************************************************************************/



#ifndef dwt_INCLUDE

	#define dwt_INCLUDE


	#include "dws.h"


	#ifdef __FLAT__
		#define _dws_CALLCON _cdecl
	#else
		#define _dws_CALLCON _far _pascal
	#endif



	/*
	 . Below are the timer rates supported by DWT.	Anything in between
	 . the listed values will cause the DOS/BIOS clock to tick erratically
	 . and is thus not allowed.  Any value higher than 145.6 Hz means
	 . you have some very special circumstances; DWT won't fit your needs
	 . anyway.
	*/
	#define dwt_18_2HZ		 0						/* 18.2 Hz */
	#define dwt_36_4HZ		 1						/* 36.4 Hz */
	#define dwt_72_8HZ		 2						/* 72.8 Hz */
	#define dwt_145_6HZ 	 3						/* 145.6 Hz */



	#ifdef __cplusplus
		extern "C" {
	#endif



	/*
	 . See #defines, above
	*/
	void _dws_CALLCON dwt_Init(word rate);


	/*
	 . If the program has called dwt_Init, it _MUST_ call dwt_Kill before it
	 . terminates.
	 .
	 . NB: Trap critical errors.	Don't let DOS put up the
	 .		 "Abort, Retry, Fail?" text.	('sides, it'll destroy your pretty gfx)
	*/
	void _dws_CALLCON dwt_Kill(void);


	/*
	 . The following 2 functions affect the timer, but not the music
	*/
	void _dws_CALLCON dwt_Pause(void);

	void _dws_CALLCON dwt_UnPause(void);


	/*
	 . Number of ticks since Beginning of World
	*/
	dword _dws_CALLCON dwt_MasterTick(void);



	#ifdef __cplusplus
		}
	#endif



#endif
