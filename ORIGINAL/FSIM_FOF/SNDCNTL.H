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
//
// Description
// -----------
//      funtion prototypes for control of the aircraft sound
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __SNDCNTL__
#define __SNDCNTL__

void DefineVoices();                     // set up the three engine harmonics
void FMSetup();                          // set up the SB/Adlib FM chip
void EngineOn();                         // turn on engine sound
void EngineOff();                        // turn off engine sound
void AdjustEngineSound( int rpm );       // adjust engine sound
void SoundCheck( state_vect& tSV );      // check sound once per frame
void InitSound();                        // initialize sound system
void SoundShutDown();                    // shut down sound system

#endif