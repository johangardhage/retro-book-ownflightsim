//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: SNDCNTL.CPP
// Project:   Flights of Fantasy
// Creation:  September 12, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  dos.h, types.h, sbfm.h, aircraft.h, sndcntl.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      9-12-1992       1.0     MB          Initial development, functions
//                                          moved over from AIRCRAFT.CPP
//      9-26-1992       1.1     MB          Publication release, cleaned up
//                                          comments
//
// Description
// -----------
//      Provides funtions for control of the aircraft sound
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>

#ifndef __TYPES__
   #include "types.h"                 // generic types
#endif
#ifndef __SBFM__
   #include "sbfm.h"                  // FM chip control functions
#endif
#ifndef __AIRCRAFT__
   #include "aircraft.h"              // need state vector definition
#endif
#ifndef __SNDCNTL__
   #include "sndcntl.h"               // header for this module
#endif

// This block declares structures and variables used for engine sound

static struct sbInst engine1;         // tone definition for harmonic one
static struct sbInst engine2;         // tone definition for harmonic two
static struct sbInst engine3;         // tone definition for harmonic three
static boolean FMSound = false;       // true if Adlib/SB FM installed
static word engineFreq1;              // frequency for tone one
static word engineFreq2;              // frequency for tone two
static word engineFreq3;              // frequency for tone three
static byte engineOct1;               // octave of tone one
static byte engineOct2;               // octave of tone two
static byte engineOct3;               // octave of tone three
static boolean sound_on;              // true if sound enabled
static boolean stall_alarm;           // true if stall alarm on
static boolean engn_running;          // true if engine running

// FM sound system constants

static const ENG_BASE_FREQ1 = 325;    // base freq. for engine sound 1
static const ENG_BASE_FREQ2 = 400;    // base freq. for engine sound 2
static const ENG_BASE_FREQ3 = 460;    // base freq. for engine sound 3
static const ENG_FREQ_DIV = 9;        // rpm divisor for engine freq.

// this function defines the three voices we'll use for the engine sound

void DefineVoices()
	{
// ************ Engine harmonic 1

	SetAttack( &engine1, 0, 0xf );    // sbfm.cpp
	SetAttack( &engine1, 1, 0xf );    // |
                                      // |
	SetDecay( &engine1, 0, 0 );       // V
	SetDecay( &engine1, 1, 0 );

	SetSustain( &engine1, 0, 0 );
	SetSustain( &engine1, 1, 0 );

	SetRelease( &engine1, 0, 5 );
	SetRelease( &engine1, 1, 5 );

	SetMultiple( &engine1, 0, 0 );
	SetMultiple( &engine1, 1, 0 );

	SelectWave( &engine1, 0, 0 );
	SelectWave( &engine1, 1, 0 );

	SetLevel( &engine1, 0, 0 );
	SetLevel( &engine1, 1, 0 );

	SustainOn( &engine1, 2 );

// ************ Engine harmonic 2

	SetAttack( &engine2, 0, 0xf );
	SetAttack( &engine2, 1, 0xf );

	SetDecay( &engine2, 0, 0 );
	SetDecay( &engine2, 1, 0 );

	SetSustain( &engine2, 0, 0 );
	SetSustain( &engine2, 1, 0 );

	SetRelease( &engine2, 0, 5 );
	SetRelease( &engine2, 1, 5 );

	SetMultiple( &engine2, 0, 1 );
	SetMultiple( &engine2, 1, 1 );

	SelectWave( &engine2, 0, 0 );
	SelectWave( &engine2, 1, 0 );

	SetLevel( &engine2, 0, 10 );
	SetLevel( &engine2, 1, 10 );

	SustainOn( &engine2, 2 );

// ************ Engine harmonic 3

	SetAttack( &engine3, 0, 0xf );
	SetAttack( &engine3, 1, 0xf );

	SetDecay( &engine3, 0, 0 );
	SetDecay( &engine3, 1, 0 );

	SetSustain( &engine3, 0, 0 );
	SetSustain( &engine3, 1, 0 );

	SetRelease( &engine3, 0, 5 );
	SetRelease( &engine3, 1, 5 );

	SetMultiple( &engine3, 0, 1 );
	SetMultiple( &engine3, 1, 1 );

	SelectWave( &engine3, 0, 0 );
	SelectWave( &engine3, 1, 0 );

	SetLevel( &engine3, 0, 10 );      // ^
	SetLevel( &engine3, 1, 10 );      // |
                                      // |
	SustainOn( &engine3, 2 );         // sbfm.cpp
	}

// sets up the Adlib/Sound Blaster FM chip

void FMSetup()
     {
	 DefineVoices();
	 engineFreq1 = 0;
	 engineFreq2 = 0;
	 engineFreq3 = 0;
	 engineOct1 = 0;
	 engineOct2 = 0;
	 engineOct3 = 0;

	 SetFreq( 1, engineFreq1 );       // sbfm.cpp
	 SetOct( 1, engineOct1 );         // |
	 SetFeedBk( 1, 5 );               // |
	 SetConn( 1, 0 );                 // V

	 SetFreq( 2, engineFreq2 );
	 SetOct( 2, engineOct1 );
	 SetFeedBk( 2, 5 );
	 SetConn( 2, 0 );

	 SetFreq( 3, engineFreq3 );
	 SetOct( 3, engineOct1 );
	 SetFeedBk( 3, 5 );
	 SetConn( 3, 0 );                 // ^
	 ProgramChange( 1, &engine1 );    // |
	 ProgramChange( 2, &engine2 );    // |
	 ProgramChange( 3, &engine3 );    // sbfm.cpp
     }

void InitSound()
   {
   if ( DetectFM() )                  // sbfm.cpp
	   {
	   ResetFM();                     // sbfm.cpp
       FMSetup();
       FMSound = true;
       sound_on = true;
       stall_alarm = false;
       engn_running = false;
	   }
   }

void SoundShutDown()
   {
   if ( FMSound )
       ResetFM();
   else
       nosound();
   }

// this function turns on the engine sound

void EngineOn()
    {
    if (FMSound)
       {
       SetOct( 1, 1 );                // sbfm.cpp
       SetOct( 2, 1 );                // |
       SetOct( 3, 1 );                // |
       SetFreq( 1, ENG_BASE_FREQ1  ); // V
       SetFreq( 2, ENG_BASE_FREQ2  );
       SetFreq( 3, ENG_BASE_FREQ3 );  // ^
       KeyOn( 1 );                    // |
       KeyOn( 2 );                    // |
       KeyOn( 3 );                    // sbfm.cpp
       }
    else
       sound( 20 );
    }

// this function turns off the engine sound

void EngineOff()
    {
    if (FMSound)
        {
        SetFreq( 1, 0 );              // sbfm.cpp
        SetFreq( 2, 0 );              // |
        SetFreq( 3, 0 );              // |
        KeyOff( 1 );                  // V
        KeyOff( 2 );
        KeyOff( 3 );
        }
    else
        nosound();
    }

// this function adjusts the engine sound frequency to the current rpm

void AdjustEngineSound( int rpm )
    {
    if (FMSound)
       {
       SetFreq( 1, rpm / ENG_FREQ_DIV);  // sbfm.cpp
       SetFreq( 2, rpm / ENG_FREQ_DIV);
       }
    else
       sound(rpm/20);
    }

// This function is called from UpdateDisplay() to handle a change in the
// sound on/off state, if any.

void SoundCheck( state_vect& tSV )
   {
   // check sound state
   if (tSV.sound_chng)
      {
      tSV.sound_chng = false;
      if (sound_on)
         {
         EngineOff();
         sound_on = false;
         }
      else
         {
         EngineOn();
         AdjustEngineSound((tSV.rpm * .60) + (tSV.h_speed * 2.0));
         sound_on = true;
         }
      }
   if ((tSV.engine_on) && (sound_on))
      {
      if (!engn_running)
         {
         EngineOn();
         engn_running = true;
         }
      AdjustEngineSound((tSV.rpm * .60) + (tSV.h_speed * 2.10));
      }
   else
      {
      if (engn_running)
         {
         EngineOff();
         engn_running = false;
         }
      }
   if (tSV.stall)
      {
      stall_alarm = true;
      sound(600);
      }
   else
      {
      if (stall_alarm)
         if (FMSound)
            nosound();
      }
   }
