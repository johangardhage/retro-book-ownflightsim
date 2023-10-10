//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: SBFM.H
// Project:   Flights of Fantasy
// Creation:  May 16, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  Nothing
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      5-16-1992       1.0     MB          initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//
// Description
// -----------
//      This file combines a functional description of the Sound Blaster FM
//      chip, with declarations for structures, functions, and global types for
//      use in SBFM.CPP.
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __SBFM__
#define __SBFM__

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;
typedef int bool;
typedef char test_arr[5];
typedef test_arr far* test_ptr;

// The scale enum provides constants for accessing the fNum[] array defined in
// SBFM.CPP. The fNum array provides the correct 10-bit frequency numbers to
// correspond to the notes of a 12-note scale.

enum Scale{ Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, C };

// The rythm enum is used in calls to RythmKeyOn() and RythmKeyOff to select
// the rythm instrument that will be voiced or unvoiced.

enum rythm{ Bass, Snare, TomTom, TopCym, HiHat };

// fNum[] is defined in SBFM.CPP, and provides frequency numbers for a 12-note
// musical scale.

extern word fNum[ 12 ];

// sbInst is an image of the SBI Sound Blaster instrument file format. Structs
// of this type may be written to an instrument file, or instrument archive
// file, as is. This structure is used throughout the SBFM modules to store
// default and current instrument definitions, and should be used by the app-
// lication to effect program changes to custom instruments. Each SBI format
// instrument is an image of the FM chip's cell control registers, plus some
// character tag data at the front, and some reserved space at the bottom.
// Thus, most of the fields in an sbInst struct correspond directly to a cell
// control register on the chip.
//
// For a detailed explanation of this structure and the registers it maps to
// see the file SBFM.TXT.

struct sbInst
	{
	char fileId[ 4 ];              // Always 'SBI'
	char instName[19];             // 18 characters for null-termed name
	byte modParams;                // Set of parameter bits for modulator
	byte carParams;                // Set of parameter bits for carrier
	byte modScalOut;               // Modulator scaling/output level
	byte carScalOut;               // Carrier scaling/output level
	byte modAttkDec;               // Modulator attack/decay
	byte carAttkDec;               // Carrier attack/decay
	byte modSustRel;               // Modulator sustain/release
	byte carSustRel;               // Carrier sustain/release
	byte modWaveSel;               // Modulator wave select
	byte carWaveSel;               // Carrier wave select
	byte feedBkConn;               // Feedback/Connection
	byte unused[ 5 ];              // Reserved
	};

// Functions for interfacing to the Sound Blaster FM synthesizer chip

// General functions for FM chip operation:

word FindFmVect();               // find the interrupt for SBFMDRV.COM
bool DetectFM();                 // returns true if FM chip present
void ResetFM();                  // resets FM chip to defaults
void SetMusicMode();             // sets FM chip to 9-voice music mode
void SetRythmMode();             // sets FM chip to 6-voice, 5-rythm mode
void VibratoDepthOn();           // enables increased vibrato depth
void VibratoDepthOff();          // disables increased vibrato depth
void TremoloDepthOn();           // enables increased tremolo depth
void TremoloDepthOff();          // disables increased tremolo depth

// Functions for control of channels 1-9

// loads a new instrument definition into the specified voice
void ProgramChange( byte theChannel, sbInst *theInstTable );

// sets the output frequency for a specified voice
void SetFreq( byte theChannel, word theFNum );

// sets the octave of a specified voice
void SetOct( byte theChannel, byte theOctave );

// Sets the type of feedback modification used on the voice
void SetFeedBk( byte theChannel, byte theFeedBk);

// Specifies how the two operator cells will be connected
void SetConn( byte theChannel, byte theConn );

// turns the specified voice on
void KeyOn( byte theChannel );

// turns the specified voice off
void KeyOff( byte theChannel );

// turns on the specified rythm voice
void RythmKeyOn(byte inst);

// turns off the specified rythm voice
void RythmKeyOff(byte inst);

// Functions for modifying an instrument definition. Modifications made with
// these functions become active after a program change. In all functions
// the cell parameter selects the cell(s) the function will operate on. When
// cell == 0 the modulator cell is affected, when cell == 1 the carrier cell
// is affected, when cell == 2 both cells are affected.

// copies the SB default FM instrument definition to the specified sbInst type
// structure.
void CopyDefInst( sbInst& theInst );

// select a waveform for modification of cell output (effective only when
// wave select is turned on)
void SelectWave( sbInst* theInst, byte cell, byte theWave );

// set the amplitude (volume) of a cell(s)
void SetLevel( sbInst* theInst, byte cell, byte theLevel );

// turn tremolo on for a cell(s)
void TremoloOn( sbInst* theInst, byte cell );

// turn tremolo off for a cell(s)
void TremoloOff( sbInst* theInst, byte cell );

// turn vibrato on for a cell(s)
void VibratoOn( sbInst* theInst, byte cell );

// turn vibrato off for a cell(s)
void VibratoOff( sbInst* theInst, byte cell );

// turn sustain on for a cell(s)
void SustainOn( sbInst* theInst, byte cell );

// turn sustain off for a cell(s)
void SustainOff( sbInst* theInst, byte cell );

// turn envelope scaling on for a cell(s)
void ScalingOn( sbInst* theInst, byte cell );

// turn envelope scaling off for a cell(s)
void ScalingOff( sbInst* theInst, byte cell );

// set the level of amplitude scaling
void SetOutputScaling( sbInst* theInst, byte cell, byte theScale );

// set the frequency multiplier for a cell(s)
void SetMultiple( sbInst* theInst, byte cell, byte theMultiple );

// set the attack rate for a cell(s)
void SetAttack( sbInst* theInst, byte cell, byte theRate );

// set the decay rate for a cell(s)
void SetDecay( sbInst* theInst, byte cell, byte theRate );

// set the sustain level for a cell(s)
void SetSustain( sbInst* theInst, byte cell, byte theLevel );

// set the release rate for a cell(s)
void SetRelease( sbInst* theInst, byte cell, byte theRate );


#endif




