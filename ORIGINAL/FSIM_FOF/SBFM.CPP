//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: SBFM.CPP
// Project:   Flights of Fantasy
// Creation:  May, 17, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  sbfm.h, mem.h, stlib.h, dos.h, string.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      5-17-1992       1.0     MB          initial development
//      8-29-1992       1.1b    MB          beta release 1
//      9-26-1992       1.1     MB          Publication release
//
// Description
// -----------
//      Provides functions for interface to the Sound Blaster FM sythesizer
//      chip, as prototyped in SBFM.H
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __SBFM__
	#include "sbfm.h"
#endif

#include <mem.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>

// this function is defined in SBFMASM.ASM. It sets the specified register
// to the specified value, including the proper amount of bus dwell time
// between I/O read/writes to the FM chip.

extern "C" SetReg( byte theReg, byte theValue );

// This array is accessible by the application, and defines a 12-note musical
// scale. Access the array using the Scale enumeration defined in SBFM.H. For
// example, to set the output frequency of voice 1 to C# you would use the
// following function call:  SetFreq( 1, fNum[ Cs ]);

word fNum[ 12 ] =
	{ 363, 385, 408, 432, 458, 485, 514, 544, 577, 611,
	  647, 686 };

// This table provides the offsets for cell control registers. The modulator
// cell for each voice is addressable, _within a block of cell control
// registers_, using the offset from this table. The carrier cell for that
// voice is always located at block start + regBaseTable[ n ] + 3. For example,
// to address the attack rate of the modulator cell for voice 3:
//
//   index = 0x60 + regBaseTable[ 2 ];
//   SetReg( index, value );
//
// To set the same value for the carrier cell of voice 3:
//
//   index = 0x60 + regBaseTable[ 2 ] + 3;
//   SetReg( index, value );
//
// This demonstrates direct access of the voice registers. In practice you
// should use the provided functions to modify the instrument definition,
// and then execute a call to ProgramChange() to load the revised definition
// into the voice registers.

static byte regBaseTable[ 9 ] =
	{ 0x0, 0x1, 0x2, 0x8, 0x9, 0xa, 0x10, 0x11, 0x12 };

// This structure holds the SB default instrument definition. It is initialized
// during DetectFM(), and loaded into all 9 voices during ResetFM().

static struct sbInst defInst;

// pseudo-register structure for FM chip registers. This provides a holding
// area for the values of registers which are not contained in the instrument
// definition.

static struct
	{
	byte test;               // the test register, register 1
	byte timer1;             // timer1 register, register 2
	byte timer2;             // timer 2 register, register 3
	byte tmrctrl;            // timer control register, register 4
	byte spchsel;            // speech select/keyboard split, register 8
	byte fnumlow[ 9 ];       // freq. select low, registers 0xa0-0xa8
	byte fnumhikb[ 9 ];      // freq. hi/octave/keyon, registers 0xb0-0xb8
	byte rythmdep;           // rythm select/depth/drum keyon, register 0xbd
	byte feedconn[ 9 ];      // feedback/connection, registers 0xc0-0xc8
	} FMRegs;                // only one of these!

// Wait() approximates a wait in microseconds by reading the bus. Called only
// by the DetectFM() function.

void Wait( int howLong )
	{
	byte dummy;
	int reads = howLong / 10;
	reads *= 16;
	do
		{
		dummy = inportb( 0x388 );
		reads--;
		} while ( reads );
	dummy++;
	return;
	}

// FindFmVect() searches the interrupt vector table from 0x80 to 0xbf to see
// if the Sound Blaster FM driver (SBFMDRV.COM) is resident in the system. It
// returns the interrupt number if the driver is found, or 0 if it is not.
// This is the only function provided to work with the FM driver. All other
// functions write directly to the FM chip.

word FindFmVect()
	{
	dword far* vect_ptr;
	test_ptr testptr;
	word int_cntr;
	word got_it = 0;

	vect_ptr = ( dword far* )MK_FP( 0, 0x202 );
	int_cntr = 0x80;
	while(( int_cntr <= 0xbf ) && ( got_it == 0 ))
		{
		testptr = ( test_arr* )MK_FP( *( word* )vect_ptr, 0x103 );
		if ( !strcmp( *testptr, "FMDRV" ))
			{
			got_it = int_cntr;
			}
		else
			{
			int_cntr++;
			vect_ptr++;
			}
		}
	return( got_it );
	}

// DetectFM() returns true if the FM chip is detected in the system, or false
// if it is not. If the chip is detected it initializes some basic registers
// and also sets up the default instrument definition. The default instrument
// is not loaded into the chip, and the voice registers are not initialized.
// This function MUST be called prior to accessing the chip, and it must be
// followed by a call to ResetFM(). Either of these functions may be called
// at any time, though there is little need to call this one more than once.

bool DetectFM()
	{
	byte stat1;
	byte stat2;
	byte result = 0;

	SetReg( 0x1, 0 );                     // initialize test register
	SetReg( 0x4, 0x60 );                  // reset both timers
	SetReg( 0x4, 0x80 );                  // enable timer interrupts
	stat1 = inportb( 0x388 );             // read the status port
	SetReg( 0x2, 0xff );                  // write ff to the timer 1 count
	SetReg( 0x4, 0x21 );                  // start timer 1
	Wait( 90 );                           // wait 90 microseconds
	stat2 = inportb( 0x388 );             // read the status port
	SetReg( 0x4, 0x60 );                  // reset both timers
	SetReg( 0x4, 0x80 );                  // enable timer interrupts
	stat1 &= 0xe0;
	if ( !stat1 )
		{
		stat2 &= 0xe0;
		if ( stat2 == 0xc0 )
			{
			result++;
			_fmemset( ( void* )&defInst, 0, sizeof( defInst ));
			strcpy( defInst.fileId, "SBI" );
			strcpy( defInst.instName, "SBFM Default Voice");
			defInst.modParams = 0x01;
			defInst.carParams = 0x11;
			defInst.modScalOut = 0x4F;
			defInst.carScalOut = 0x0;
			defInst.modAttkDec = 0xF1;
			defInst.carAttkDec = 0xF2;
			defInst.modSustRel = 0x53;
			defInst.carSustRel = 0x74;
			defInst.modWaveSel = 0x0;
			defInst.carWaveSel = 0x0;
			defInst.feedBkConn = 0x0;
			}
		}
	return( result );
	}

// ResetFM() initializes the pseudo-registers, initializes the voice control
// registers, and loads the default instrument definition into the chip for
// channels 1-9. This function MUST be called after DetectFM(), and may be
// called at any time thereafter to reset the voices and intialize the chip.

void ResetFM()
	{
	byte chan;

	memset( &FMRegs, 0, sizeof( FMRegs ));   // initialize the regs struct
	FMRegs.test = 0;
	SetReg( 1, FMRegs.test );
	FMRegs.rythmdep = 0xc0;
	SetReg( 0xbd, FMRegs.rythmdep );
	FMRegs.spchsel = 0x0;
	SetReg( 0x8, FMRegs.spchsel );
	for ( chan = 0; chan < 9; chan++ )
		{
		SetReg( 0xa0 + chan, FMRegs.fnumlow[ chan ] );
		SetReg( 0xb0 + chan, FMRegs.fnumhikb[ chan ] );
		SetReg( 0xc0 + chan, FMRegs.feedconn[ chan ] );
		}
	for ( chan = 1; chan <= 9; chan++ )
		ProgramChange( chan, &defInst );
	return;
	}

// SetMusicMode() puts the FM chip into 9-voice FM music mode

void SetMusicMode()
	{
	FMRegs.spchsel &= 0x7f;
	FMRegs.rythmdep &= 0xdf;
	SetReg( 0x8, FMRegs.spchsel );
	SetReg( 0xbd, FMRegs.rythmdep );
	}

// SetRythmMode() puts the FM chip into 6-voice / 5-rythm mode. In this mode
// the rythm-hit functions can be used.

void SetRythmMode()
	{
	FMRegs.spchsel &= 0x7f;
	FMRegs.rythmdep &= 0xe0;
	FMRegs.rythmdep |= 0x20;
	SetReg( 0x8, FMRegs.spchsel );
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// VibratoDepthOn() turns on increased vibrato depth for all voices.

void VibratoDepthOn()
	{
	FMRegs.rythmdep |= 0x40;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// VibratoDepthOff() turns off increased vibrato depth for all voices

void VibratoDepthOff()
	{
	FMRegs.rythmdep &= 0xbf;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// TremoloDepthOn() turns on increased tremolo depth for all voices

void TremoloDepthOn()
	{
	FMRegs.rythmdep |= 0x80;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// TremoloDepthOff() turns off increased tremolo depth for all voices

void TremoloDepthOff()
	{
	FMRegs.rythmdep &= 0x7f;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// ProgramChange() loads the specified instrument definition into the
// specified voice channel. This function must be called after an instrument
// definition is modified, in order for the modification to take effect. The
// voice channel should be in key-off mode when this function is called

void ProgramChange( byte theChannel, sbInst *theInstTable )
	{
	byte targetReg;
	byte index;

	index = theChannel - 1;

	SetReg( 1, 0x20 );
	targetReg = regBaseTable[ index ] + 0x20;
	SetReg( targetReg, theInstTable->modParams );
	targetReg += 0x3;
	SetReg( targetReg, theInstTable->carParams );
	targetReg += 0x1d;
	SetReg( targetReg, theInstTable->modScalOut );
	targetReg += 0x3;
	SetReg( targetReg, theInstTable->carScalOut );
	targetReg += 0x1d;
	SetReg( targetReg, theInstTable->modAttkDec );
	targetReg += 0x3;
	SetReg( targetReg, theInstTable->carAttkDec );
	targetReg += 0x1d;
	SetReg( targetReg, theInstTable->modSustRel );
	targetReg += 0x3;
	SetReg( targetReg, theInstTable->carSustRel );
	targetReg += 0x5d;
	SetReg( targetReg, theInstTable->modWaveSel );
	targetReg += 0x3;
	SetReg( targetReg, theInstTable->carWaveSel );

	return;
	}

// SetFreq() sets the output frequency of a voice channel. The frequency is
// determined by the low ten bits of the parameter theFNum.

void SetFreq( byte theChannel, word theFNum )
	{
	byte index = theChannel - 1;

	FMRegs.fnumlow[ index ] = (byte)theFNum;
	theFNum >>= 8;
	theFNum &= 3;
	FMRegs.fnumhikb[ index ] |= (byte)theFNum;
	SetReg( ( 0xa0 + index ) , FMRegs.fnumlow[ index ] );
	SetReg(( 0xb0 + index ), FMRegs.fnumhikb[ index ] );
	return;
	}

// SetOct() sets the output octave for the specified voice channel. The octave
// parameter may be in the range 0..7.

void SetOct( byte theChannel, byte theOctave )
	{
	byte index = theChannel - 1;

	if (( theOctave < 8 ) && ( theOctave > -1 ))
		{
		theOctave <<= 2;
		FMRegs.fnumhikb[ index ] &= 0xe3;
		FMRegs.fnumhikb[ index ] |= theOctave;
		SetReg(( 0xb0 + index ), FMRegs.fnumhikb[ index ] );
		}
	return;
	}

// Sets the type of feedback modification used on the voice

void SetFeedBk( byte theChannel, byte theFeedBk)
	{
	byte index = theChannel - 1;

	theFeedBk &= 7;
	theFeedBk <<= 1;
	FMRegs.feedconn[ index ] &= 0xf1;
	FMRegs.feedconn[ index ] |= theFeedBk;
	SetReg( 0xc0 + index, FMRegs.feedconn[ index ] );
	return;
	}

// Specifies how the two operator cells will be connected

void SetConn( byte theChannel, byte theConn )
	{
	byte index = theChannel - 1;

	theConn &= 1;
	FMRegs.feedconn[ index ] &= 0xfe;
	FMRegs.feedconn[ index ] |= theConn;
	SetReg( 0xc0 + index, FMRegs.feedconn[ index ] );
	return;
	}

// KeyOn() turns on a specified voice channel

void KeyOn( byte theChannel )
	{
	byte index = theChannel - 1;

	FMRegs.fnumhikb[ index ] |= 0x20;
	SetReg(( 0xb0 + index ), FMRegs.fnumhikb[ index ] );
	return;
	}

// KeyOff() turns off a specified voice channel

void KeyOff( byte theChannel )
	{
	byte index = theChannel - 1;

	FMRegs.fnumhikb[ index ] &= 0xdf;
	SetReg(( 0xb0 + index ), 0 );
	return;
	}

// RythmKeyOn() operates when the FM chip is in rythm mode, and turns on the
// specified rythm instrument. The rythm enumeration is defined in SBFM.H to
// simplify the call to this function. For example, to voice the Bass Drum
// you would call RythmKeyOn( Bass );

void RythmKeyOn( byte inst )
	{
	byte rythmMask = 0x10;

	rythmMask >>= inst;
	FMRegs.rythmdep |= rythmMask;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// RythmKeyOff() turns off voicing for the specified rythm instrument when
// the FM chip is operating in rythm mode. The rythm enumeration is used as
// in RythmKeyOn() above. For example, to turn off the HiHat you would call
// RythmKeyOff( HiHat );

void RythmKeyOff( byte inst )
	{
	byte rythmMask = 0x10;

	rythmMask >>= inst;
	rythmMask = ~rythmMask;
	FMRegs.rythmdep &= rythmMask;
	SetReg( 0xbd, FMRegs.rythmdep );
	return;
	}

// CopyDefInst() copies the SB default FM instrument definition to the
// specified sbInst type structure.

void CopyDefInst( sbInst& theInst )
	{
	_fmemcpy( &theInst, &defInst, sizeof( defInst ));
	return;
	}

// SetLevel() modifies the output level of either or both cells in the
// specified instrument definition.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell == 1, or both if cell == 2

void SetLevel( sbInst* theInst, byte cell, byte theLevel )
	{
	theLevel &= 0x3f;
	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modScalOut &= 0xc0;
		theInst->modScalOut |= theLevel;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carScalOut &= 0xc0;
		theInst->carScalOut |= theLevel;
		}
	return;
	}

// SelectWave() selects the modifying waveform for either or both cells in
// the specified instrument definition. The waveform type may range from
// 0..3, and selects a modifying waveform as described in the text, which is
// applied to the output of the affected cell. In order for a waveform change
// to take effect bit 5 of the test register (01) must be set before the
// new change is written to the cell control registers. This must be done
// before each change of waveform, and is performed in the ProgramChange()
// function immediately prior to loading the registers, whether the waveform
// has been changed or not.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SelectWave( sbInst* theInst, byte cell, byte theWave )
	{
	theWave &= 0x3;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modWaveSel &= 0xfc;
		theInst->modWaveSel |= theWave;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carWaveSel &= 0xfc;
		theInst->carWaveSel |= theWave;
		}
	return;
	}

// TremoloOn() turns on tremolo for either or both cells of the specified
// instrument definition.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void TremoloOn( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams |= 0x80;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams |= 0x80;
	return;
	}

// TremoloOff() turns off tremolo for either or both cells of the specified
// instrument definition.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void TremoloOff( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams &= 0x7f;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams &= 0x7f;
	return;
	}

// VibratoOn() turns on vibrato for either or both cells of the specified
// instrument definition.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void VibratoOn( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams |= 0x40;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams |= 0x40;
	return;
	}

// VibratoOff() turns off vibrato for either or both cells of the specified
// instrument definition.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void VibratoOff( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams &= 0xbf;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams &= 0xbf;
	return;
	}

// SustainOn() causes either or both cells of a specified instrument
// definition to produce a sustained sound that remains at the level set
// with SetSustain() until a KeyOff() call.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SustainOn( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams |= 0x20;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams |= 0x20;
	return;
	}

// SustainOff() causes either or both cells of a specified instrument
// definition to output a diminishing sound which decays after KeyOn()
// according to it's waveform.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SustainOff( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams &= 0xdf;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams &= 0xdf;
	return;
	}

// ScalingOn() causes either or both cells of the specified instrument
// definition to output a shortened envelope as higher notes on the scale
// are played. Some scaling is one by default. ScalingOn() causes the default
// scaling values to be increased.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void ScalingOn( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams |= 0x10;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams |= 0x10;
	return;
	}

// ScalingOff() causes the envelope scaling factor to use the power-on default
// scale.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void ScalingOff( sbInst* theInst, byte cell )
	{
	if (( cell == 2 ) || ( cell == 0 ))
		theInst->modParams &= 0xef;
	if (( cell == 2 ) || ( cell == 1 ))
		theInst->carParams &= 0xef;
	return;
	}

// SetOutputScaling() sets the level of attentuation which will be applied
// to either or both cells of the specified instrument definition as higher
// notes are played.
// 						0 : no attenuation
//                      1 : 3db / octave
//                      2 : 1.5db / octave
//                      3 : 6db / octave
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetOutputScaling( sbInst* theInst, byte cell, byte theScale )
	{
	theScale &= 0x3;
	theScale <<= 6;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modScalOut &= 0x3f;
		theInst->modScalOut |= theScale;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carScalOut &= 0x3f;
		theInst->carScalOut |= theScale;
		}
	return;
	}

// SetMultiple() sets the frequency multiplier for either or both cells of
// the specified instrument definition. Parameter theMultiple must be in the
// range 0..15.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetMultiple( sbInst* theInst, byte cell, byte theMultiple )
	{
	theMultiple &= 0xf;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modParams &= 0xf0;
		theInst->modParams |= theMultiple;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carParams &= 0xf0;
		theInst->carParams |= theMultiple;
		}
	return;
	}

// SetAttack() sets the attack rate for either or both cells of the specified
// instrument definition. Parameter theRate must be in the range 0..63.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetAttack( sbInst* theInst, byte cell, byte theRate )
	{
	theRate <<= 4;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modAttkDec &= 0xf;
		theInst->modAttkDec |= theRate;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carAttkDec &= 0xf;
		theInst->carAttkDec |= theRate;
		}
	return;
	}

// SetDecay() sets the decay rate for either or both cells of the specified
// instrument definition. Parameter theRate must be in the range 0..63.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetDecay( sbInst* theInst, byte cell, byte theRate )
	{
	theRate &= 0xf;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modAttkDec &= 0xf0;
		theInst->modAttkDec |= theRate;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carAttkDec &= 0xf0;
		theInst->carAttkDec |= theRate;
		}
	return;
	}

// SetSustain() sets the sustain level for either or both cells of the specified
// instrument definition. Parameter theLevel must be in the range 0..15.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetSustain( sbInst* theInst, byte cell, byte theLevel )
	{
	theLevel <<= 4;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modSustRel &= 0xf;
		theInst->modSustRel |= theLevel;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carSustRel &= 0xf;
		theInst->carSustRel |= theLevel;
		}
	return;
	}

// SetRelease() sets the release rate for either or both cells of the specified
// instrument definition. Parameter theRate must be in the range 0..63.
//
// The cell parameter selects the cell(s) which will be operated on, either
// the modulator if cell == 0, the carrier if cell = 1, or both if cell == 2

void SetRelease( sbInst* theInst, byte cell, byte theRate )
	{
	theRate &= 0xf;

	if (( cell == 2 ) || ( cell == 0 ))
		{
		theInst->modSustRel &= 0xf0;
		theInst->modSustRel |= theRate;
		}
	if (( cell == 2 ) || ( cell == 1 ))
		{
		theInst->carSustRel &= 0xf0;
		theInst->carSustRel |= theRate;
		}
	return;
	}

