'******************************************************************************
'File:      findsb.bas
'Version:   2.20
'Tab stops: every 2 columns
'Project:   FINDSB utility
'Copyright: 1994-1995 DiamondWare, Ltd.  All rights reserved.
'Written:   Erik Lorenzen & Don Lemons
'Purpose:   Example code to autodetect and print out the sound hardware
'History:   94/10/21 KW Started findsb.c
'           94/11/12 DL translated to BASIC
'           95/02/02 EL Cleaned up & Finalized
'           95/03/22 EL Finalized for 1.01
'           95/04/11 EL Finalized for 1.02
'           95/06/06 EL Finalized for 1.03, no changes
'           95/06/06 EL Finalized for 2.00, no changes
'           95/10/07 EL Finalized for 2.10, no changes
'           95/10/18 EL Finalized for 2.20, no changes
'******************************************************************************



'$INCLUDE: 'dws.bi'
'$INCLUDE: 'err.bi'



'DECLARE VARIABLES
	COMMON SHARED dov 	AS dwsDETECTOVERRIDES
	COMMON SHARED dres	AS dwsDETECTRESULTS
	COMMON SHARED ideal AS dwsIDEAL



'START OF MAIN

	PRINT
	PRINT "FINDSB 2.20 is Copyright 1994-95, DiamondWare, Ltd."
	PRINT "All rights reserved."
	PRINT : PRINT : PRINT

	'We need to set every field to -1 in dws_DETECTOVERRIDES struct; this
	'tells the STK to autodetect everything.  Any other value
	'overrides the autodetect routine, and will be accepted on
	'faith, though the STK will verify it if possible.

	dov.baseport = -1
	dov.digdma	 = -1
	dov.digirq	 = -1

	IF dwsDetectHardWare(dov, dres) = 0 THEN
		errDisplay
	END IF

	'Test for FM or DIG
	IF ((dres.capability AND dwscapabilityFM) OR ((dres.baseport <> 904) AND (dres.baseport <> -1))) THEN

		PRINT "Base port is ";HEX$(dres.baseport);" hex."
		PRINT

		IF dres.mixtyp > 1 THEN
			PRINT "The sound hardware supports mixing."
			PRINT
		ELSE
			PRINT "Mixing will be done in software."
			PRINT
		ENDIF

		IF (dres.capability AND dwscapabilityFM) = dwscapabilityFM THEN
			PRINT "The sound hardware supports FM music playback."
			PRINT
		ELSE
			PRINT "Support for FM music playback not found."
			PRINT
		ENDIF

		IF (dres.capability AND dwscapabilityDIG) = dwscapabilityDIG THEN
			'If we got here dws_DetectHardWare got PORT, IRQ, & DMA
			PRINT "The sound hardware supports digitized sound playback."
			PRINT "The sound hardware uses DMA channel";dres.digdma;"and IRQ level";dres.digirq;"."
			PRINT
		ELSEIF ((dres.baseport <> 904) AND (dres.baseport <> -1)) THEN
			'If dres.baseport isn't either 388hex, or -1, then it's a valid
			'baseport.  So if we got here, then we didn't find either IRQ
			'level, and/or DMA channel.  In order to play digitized sounds,
			'we need these settings as well.  In your application, you should
			'ask the user.
			PRINT "The sound hardware supports digitized sound playback,"
			PRINT "but we couldn't find the DMA channel and/or IRQ level."
			PRINT
		ELSE
			PRINT "Support for digitized playback not found."
			PRINT
		END IF

	ELSE
		PRINT "No sound hardware detected."
		PRINT
	ENDIF

END
