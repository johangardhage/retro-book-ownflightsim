'******************************************************************************
'File:      playdwm.bas
'Version:   2.20
'Tab stops: every 2 columns
'Project:   DWD Player
'Copyright: 1994-1995 DiamondWare, Ltd.  All rights reserved.
'Written:   Erik Lorenzen & Don Lemmons
'Purpose:   Contains simple example code to show how to load/play a .DWM file
'History:   94/10/21 KW Started playdwm.c
'           94/11/12 DL Translated to BASIC
'           95/01/12 EL Cleaned up & Finalized
'           95/03/22 EL Finalized for 1.01
'           95/04/11 EL Finalized for 1.02
'           95/06/06 EL Finalized for 1.03, no changes
'           95/06/06 EL Finalized for 2.00, no changes
'           95/10/07 EL Finalized for 2.10, no changes
'           95/10/18 EL Finalized for 2.20, changed vol's to 95%
'
'Notes
'-----
'This code isn't really robust when it comes to standard error checking
'and particularly recovery, software engineering technique, etc.  A buffer
'is statically allocated.  A better technique would be to use fstat() or stat()
'to determine the file's size then malloc(size).	The STK will handle songs
'larger than 64K (but not digitized sounds).  Obviously, you'd need to fread()
'such a file in chunks, or write some sort of hfread() (huge fread).  Also,
'exitting and cleanup is not handled robustly in this code.  The code below can
'only be validated by extremely careful scrutiny to make sure each case is
'handled properly.  A better method would the use of C's atexit function.
'
'But all such code would make this example file less clear; its purpose was
'to illustrate how to call the STK, not how to write QA-proof software.
'******************************************************************************/



'$INCLUDE: 'dws.bi'
'$INCLUDE: 'dwt.bi'
'$INCLUDE: 'err.bi'



TYPE BUFFTYP
	buf AS STRING * 32767
END TYPE



'DECLARE VARIABLES
	COMMON SHARED dov 	AS dwsDETECTOVERRIDES
	COMMON SHARED dres	AS dwsDETECTRESULTS
	COMMON SHARED ideal AS dwsIDEAL
	COMMON SHARED mplay AS dwsMPLAY



DIM SHARED buffer(0) AS BUFFTYP 'set aside string area for song to load into
																'by doing it this way we give QBasic the
																'opportunity to place the song into far mem
'START OF MAIN

	PRINT
	PRINT "PLAYDWM 2.20 is Copyright 1994-95, DiamondWare, Ltd."
	PRINT "All rights reserved."
	PRINT : PRINT : PRINT

	musvol% 		= 255

	filename$ = LTRIM$(RTRIM$(COMMAND$))
	IF filename$ = "" THEN
		PRINT "Usage PLAYDWD <dwd-file>"
		GOTO ProgramExit
	END IF

	OPEN filename$ FOR BINARY AS #1 LEN = 1
	filelen = LOF(1)
	CLOSE #1

	IF filelen = 0 THEN
		PRINT "File Not Found"
		GOTO ProgramExit
	END IF

	IF filelen > 32767 THEN
		PRINT "File Too Big"
		GOTO ProgramExit
	END IF

	OPEN filename$ FOR BINARY AS #1 LEN = 1
	GET #1, 1, buffer(0).buf
	CLOSE #1

	'We need to set every field to -1 in dwsDETECTOVERRIDES struct; this
	'tells the STK to autodetect everything.  Any other value
	'overrides the autodetect routine, and will be accepted on
	'faith, though the STK will verify it if possible.

	dov.baseport = -1
	dov.digdma	 = -1
	dov.digirq	 = -1

	IF DWSDetectHardWare(dov, dres) = 0 THEN
		errDisplay
		GOTO ProgramExit
	END IF

	IF (dres.capability AND dwscapabilityFM) <> dwscapabilityFM THEN
		PRINT"FM support not found"
		GOTO ProgramExit
	END IF

	'The "ideal" struct tells the STK how you'd like it to initialize the
	'sound hardware.      In all cases, if the hardware won't support your
	'request, the STK will go as close as possible.  For example, not all
	'sound boards will support al sampling rates (some only support 5 or
	'6 discrete rates).

	ideal.musictyp	 = 1					'0=No music, 1=OPL2
	ideal.digtyp		 = 0					'0=No Dig, 8=8bit, 16=16bit
	ideal.digrate 	 = 0					'sampling rate, in Hz
	ideal.dignvoices = 0					'number of voicws.bies (up to 16)
	ideal.dignchan	 = 0					'1=mono, 2=stereo

	IF dwsInit(dres, ideal) = 0 THEN
		errDisplay
		GOTO ProgramKill
	END IF

	'Set music vol to about 95% of max
	musvol% = 242
	IF dwsXMusic(musvol%) = 0 THEN
		errDisplay
	END IF

	'72.8Hz is a decent compromise.  It will work in a Windows DOS box
	'without any problems, and yet it allows music to sound pretty good.
	'In my opinion, there's no reason to go lower than 72.8 (unless you
	'don't want the hardware timer reprogrammed)--music sounds kinda chunky
	'at lower rates.  You can go to 145.6 Hz, and get smoother (very
	'subtly) sounding music, at the cost that it will NOT run at the correct
	'(or constant) speed in a Windows DOS box.

	dwtInit(dwt728HZ)

	soundseg% = VARSEG(buffer(0).buf)
	soundoff% = VARPTR(buffer(0).buf)
	pointer&	= soundseg% * 256 ^ 2 + soundoff%  'make pointer

	mplay.track = pointer&
	mplay.count = 1 				 '0=infinite loop, 1-N num times to play sound

	IF dwsMPlay(mplay) = 0 THEN
		errDisplay
		GOTO ProgramKill
	END IF

	'We're playing.  Let's exit when the song is over, and allow the user
	'to fiddle with the volume level (mixer) in the meantime

	PRINT"Press + or - to change playback volume"

	result% = dwsMSONGSTATUSPLAYING
	DO UNTIL (result%  AND dwsMSONGSTATUSPLAYING) <> dwsMSONGSTATUSPLAYING
		inpt$ = INKEY$

		IF inpt$ = "+" THEN
			musvol% = musvol% + 1

			PRINT"Music Volume is ";musvol%

			IF dwsXMusic(musvol%) = 0 THEN
				errDisplay
			END IF
		END IF

		IF inpt$ = "-" THEN
			musvol% = musvol% - 1

			PRINT"Music Volume is ";musvol%

			IF dwsXMusic(musvol%) = 0 THEN
				errDisplay
			END IF
		END IF

		IF inpt$ = "q" OR inpt$ = "q" OR inpt$ = chr$(27) THEN
			GOTO ProgramKill
		END IF

		IF dwsMSongStatus(result%) = 0 THEN
			errDisplay
			GOTO ProgramKill
		END IF
	LOOP

	ProgramKill:

	'If dwt is not inited calling dwt_Kill will have no effect
	dwtKill

	IF dwsKill = 0 THEN
		errnum = dwsErrNo
		errDisplay

		'If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		'or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		'must remove his tsr, and dws_Kill must be called again.  If it's
		'dws_NOTINITTED, there's nothing to worry about at this point.
		IF errnum = dwsKillCANTUNHOOKISR THEN
			GOTO ProgramKill
		END IF
	END IF

	ProgramExit:

END
