//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: SNDCNTL.H
// Project:   Flights of Fantasy
// Creation:  September 12, 1992
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
//      9-12-1992       1.0     MB          Initial development, functions
//                                          split out of AIRCRAFT.H
//      9-26-1992       1.1     MB          publication release
//      10-11-95        2.0     mracky      update to AirCraft class
// Description
// -----------
//      funtion prototypes for control of the aircraft sound
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __SNDCNTL__
#define __SNDCNTL__

void SoundCheck( AirCraft& tSV );      // check sound once per frame
void SoundCrash( AirCraft& tSV ); 		 // makes a crash sound

#endif
