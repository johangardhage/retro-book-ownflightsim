//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright 1991-1992 Betz Associates. All rights reserved.
//
// File Name: FSMAIN.CPP
// Project:   Flights of Fantasy
// Creation:  January 21, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  dos.h, stdlib.h, bios.h, math.h, conio.h, mem.h, types.h,
//            htimer.h, pcx.h, aircraft.h, viewcntl.h, sndcntl.h, input.h,
//            screen.h, detect.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.  Author    Purpose
//      ----            ----    ------      -------
//      1-21-1992       1.0     MB          Initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//
// Description
// -----------
//      Main module for the Waite Group Flight Simulator
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>
#include <stdlib.h>
#include <bios.h>
#include <math.h>
#include <conio.h>
#include <mem.h>

#ifndef __TYPES__
	#include "types.h"            // generic data types
#endif
#ifndef __HTIMER__
		#include "htimer.h"           // hi-res timer class
#endif
#ifndef __PCX__
		#include "pcx.h"
#endif
#ifndef __AIRCRAFT__
		#include "aircraft.h"         // aircraft reaction functions
#endif
#ifndef __VIEWCNTL__
		#include "viewcntl.h"
#endif
#ifndef __SNDCNTL__
		#include "sndcntl.h"
#endif
#ifndef __INPUT__
	#include "input.h"        // event handling
#endif
#ifndef __SCREEN__
		#include "screen.h"       // video functions
#endif
#ifndef __DETECT__
		#include "detect.h"       // cpu detect
#endif

byte huge* TESTPTR = NULL;

boolean CPU_386;              // global flag, true if 386 processor

state_vect tSV;               // the control-state vector

// This block declares world, view, and image objects

pcx_struct bkground;        // Structure for bkground image
Pcx bgloader;         // PCX loader object

int opMode;                   // operating mode flag
int oldVmode;                 // Save area for previous video mode
int checkpt = 0;              // Tracks program progress for use in
															// shutdown
const DEBUG = 0;              // operating mode constants
const FLIGHT = 1;
const WALK = 2;
const HELP = 3;
const VERSION = 4;

const MAX_ARGS = 1;           // maximum number of cl parameters

const MAJ_VER = 1;            // major and minor version numbers
const MIN_VER = 2;
const VER_LET = 0;            // letter, if any, to follow minor version num

// called when program ends, or if an error occurs during program execution.
// the systems which are shutdown depend on the value of chekpt, which is
// incremented as the system is set up at program start.

void ShutDown()
	 {
	 if (checkpt >= 1)
			ControlShutDown();      // input.cpp
	 if (checkpt >= 2)
			ViewShutDown();         // viewcntl.cpp
	 if (checkpt >= 3)
			AcShutdown();           // aircraft.cpp
	 SoundShutDown();           // sndcntl.cpp
	 setgmode( oldVmode );      // screen.asm
	 }

// this function provides a dump of the aircraft state vector values (see the
// struct definition in AIRCRAFT.H). If debug is true it is output
// with the proper header and footer for a realtime dump. If debug is false
// it is formatted to be output at the end of the program.

void VectorDump()
	{
		int i, j;

		gotoxy(3, 1);
		cprintf("State vector realtime dump:");
		i = 1;
		ViewParamDump( 35, i );
		ACDump( 35, i );
		i = 3;
	gotoxy(3, i++);
	cprintf("right aileron: %i       ", -tSV.aileron_pos);
	gotoxy(3, i++);
	cprintf("left aileron:  %i       ", tSV.aileron_pos);
	gotoxy(3, i++);
	cprintf("elevator:      %i       ", tSV.elevator_pos);
	gotoxy(3, i++);
		cprintf("rudder:        %i       ", tSV.rudder_pos);
	gotoxy(3, i++);
	cprintf("throttle:      %i       ", tSV.throttle_pos);
	gotoxy(3, i++);
	cprintf("button 1:      %i       ", tSV.button1);
	gotoxy(3, i++);
	cprintf("button 2:      %i       ", tSV.button2);
	gotoxy(3, i++);
	cprintf("ignition:      %u       ", tSV.ignition_on);
	gotoxy(3, i++);
	cprintf("engine on:     %i       ", tSV.engine_on);
	gotoxy(3, i++);
	cprintf("prop rpm:      %i       ", tSV.rpm);
	gotoxy(3, i++);
	cprintf("fuel level:    %i       ", tSV.fuel);
	gotoxy(3, i++);
	cprintf("x coordinate:  %i       ", tSV.x_pos);
	gotoxy(3, i++);
	cprintf("y coordinate:  %i       ", tSV.y_pos);
	gotoxy(3, i++);
	cprintf("z coordinate:  %i       ", tSV.z_pos);
	gotoxy(3, i++);
	cprintf("pitch:         %f       ", tSV.pitch);
		gotoxy(3, i++);
		cprintf("effect. pitch: %f       ", tSV.efAOF);
	gotoxy(3, i++);
	cprintf("roll:          %f       ", tSV.roll);
		gotoxy(3, i++);
		cprintf("yaw:           %f       ", tSV.yaw);
		gotoxy(3, i++);
		cprintf("speed:         %f       ", tSV.h_speed);
		gotoxy(3, i++);
		cprintf("rate of climb: %f       ", tSV.climbRate);
		gotoxy(3, i++);
		cprintf("altitude:      %i       ", tSV.altitude);
	}

// reports program status at termination. Based on the value of checkpt it
// knows whether this is an abnormal term, and prints the leadin accordingly

void Terminate( char* msg, char* loc )
		{
		int exit_code;

		ShutDown();
		cprintf("FOF.EXE ==>\r\n");
		if (checkpt < 4)
			 {
			 cprintf( "A critical error occured in function " );
			 cprintf( "%s\r\n", loc );
			 cprintf( "Error: " );
			 cprintf( "%s, causing controlled termination\r\n", msg );
			 exit_code = 1;
			 }
		else
			 {
			 cprintf("%s in ", msg );
			 cprintf("%s\r\n", loc );
			 exit_code = 0;
			 }
		if (opMode != DEBUG)
			 {
			 ReportFrameRate();
			 }
		exit(exit_code);
		}

// displays the title screen and waits for a keypress

boolean DoTitleScreen()
		 {
		 boolean result = true;
		 HTimer pixTimer;

	 if (bgloader.load("title.pcx",&bkground))
				result = false;
		 if (result)
				{
				putwindow( 0, 0, 320, 200, bkground.image );
				fadepalin( 0, 256, bkground.palette );
				KeyPressed();
				pixTimer.timerOn();
				while ( (!KeyPressed()) && (!tSV.button1))
					 {
					 if (pixTimer.getElapsed() > 10000000L)
							break;
					 }
				fadepalout( 0, 256 );
				ClrPalette( 0, 256 );
				ClearScr( 0 );
				}
		 return( result );
		 }

// called from main() at program startup to initialize the control, view,
// and flight model systems

void StartUp()
	 {
	 boolean result = false;
	 CPU_386 = detect386();               // check for 386 processor
	 if (!detectvga())
			Terminate( "No VGA/analog color monitor detected", "main()");
	 if ( !InitControls() )               // input.cpp: initialize controls
			Terminate( "Keyboard/Joystick init failed", "main()" );
	 checkpt = 1;
	 clrscr();                            // conio.h: clear the text screen
	 if ((opMode == FLIGHT) || (opMode == WALK)) // if not debugging or walking...
			{
			setgmode( 0x13 );                 // screen.asm: set graphics mode
			ClrPalette( 0, 256 );             // screenc.cpp: clear the palette
			if (opMode == FLIGHT)
				 if ( !DoTitleScreen() )        // display the title
						Terminate( "error loading title image", "DoTitleScreen()");
			}
	 if ( !InitView( &bgloader, &bkground, opMode ))
			Terminate( "Graphics/View system init failed", "main()" );
	 checkpt = 2;                         // update progress flag
	 if ( !InitAircraft( tSV, opMode ))
			Terminate( "Aircraft initialization failed", "main()" );
	 checkpt = 3;                         // update progress flag
	 InitSound();
	 }

// display control help

void DisplayHelp()
	 {
	 gotoxy(1, 1);
	 cprintf("        The Waite Group's 'Flights of Fantasy' (c) 1992\r\n");
	 cprintf("----------------------------------------------------------------\r\n");
	 cprintf("* cmd line args:     H, h or ? - display this help screen\r\n");
	 cprintf("                     D or d    - enable debugging dump mode\r\n");
	 cprintf("                     W or w    - enable world traverse mode\r\n");
	 cprintf("                     V or v    - diplay program version\r\n");
	 cprintf("\r\n");
	 cprintf("* view control keys: F1        - look forward\r\n");
	 cprintf("                     F2        - look right\r\n");
	 cprintf("                     F3        - look behind\r\n");
	 cprintf("                     F4        - look left\r\n");
	 cprintf("\r\n");
	 cprintf("* engine control:    I or i - toggle ignition/engine on/off\r\n");
	 cprintf("                     +/-    - increase/decrease throttle setting\r\n");
	 cprintf("\r\n");
	 cprintf("* sound control:     S or s - toggle sound on/off\r\n");
	 cprintf("\r\n");
	 cprintf("* aircraft control:  pitch up   - stick back, or down arrow\r\n");
	 cprintf("                     pitch down - stick forwardd, or up arrow\r\n");
	 cprintf("                     left roll  - stick left, or left arrow\r\n");
	 cprintf("                     right roll - stick right, or right arrow\r\n");
	 cprintf("                     rudder     - '<' or '>' keys\r\n");
	 cprintf("                     brake      - 'b' or 'B'\r\n");
	 }

// use this function in main to suck up heap. Useful for causing allocs
// to fail in a controlled manner

boolean GrabRam( dword howMuch )
	 {
	 if ((TESTPTR = new byte[ howMuch ]) == NULL)
			return( false );
	 else return( true );
	 }

// this function parses the command line parameters. Accepted command line
// parameters are:
//                   d, D     :  start FOF in debugging dump mode
//                   h, H, ?  :  display a command list before starting
//                   w, W     :  start FOF in world traverse mode
//                   v, V     :  display program version number

void ParseCLP( int argc, char* argv[] )
	 {
	 int i;

	 if (argc <= (MAX_ARGS + 1))
			{
			for (i = 1; i < argc; i++)
				 {
				 if ((*argv[i] == 'd') || (*argv[i] == 'D'))
						opMode = DEBUG;
				 else if (*argv[i] == '?')
						opMode = HELP;
				 else if ((*argv[i] == 'h') || (*argv[i] == 'H'))
						opMode = HELP;
				 else if ((*argv[i] == 'w') || (*argv[i] == 'W'))
						opMode = WALK;
				 else if ((*argv[i] == 'v') || (*argv[i] == 'V'))
						opMode = VERSION;
				 else
						Terminate("invalid command line parameter","ParseCLP()");
				 }
			}
	 else if (argc > (MAX_ARGS + 1))
			Terminate("extra command line parameter","ParseCLP()");
	 }

// handles a ground approach by determining from pitch and roll whether
// the airplane has landed safely or crashed

void GroundApproach()
		{
		// handle approaching the ground
		if (opMode == FLIGHT)
			 {
			 if ((tSV.airborne) && (tSV.altitude <= 0))
					{
					if ( ((tSV.pitch > 10) || (tSV.pitch < -10)) ||
							 ((tSV.roll > 10) || (tSV.roll < -10)) )
						 {
						 ShowCrash();               // viewcntl.cpp
						 ResetACState( tSV );       // aircraft.cpp
						 SoundCheck( tSV );         // sndcntl.cpp
						 delay(200);
						 KeyPressed();              // input.cpp
						 while( !KeyPressed() );    // input.cpp
						 }
					else
						 LandAC( tSV );             // aircraft.cpp
					}
			 }
		}

// this function displays the program version number

void DisplayVersion()
	 {
	 cprintf("ษอออออออออออออออออออออออออออออออออออป\r\n");
	 cprintf("บ                                   บ\r\n");
	 cprintf("ศอออออออออออออออออออออออออออออออออออผ\r\n");
	 gotoxy( 4, 2 );
	 cprintf("Flights of Fantasy, Version %i.%i%c", MAJ_VER, MIN_VER, VER_LET);
	 gotoxy( 4, 6 );
	 }

// program entry point

void main( int argc, char* argv[])
	{
	window(1,1,80,25);                   // conio.h: set a text window
	clrscr();                            // conio.h: clear the screen
		textcolor(7);                        // conio.h: set the text color
		oldVmode = *(byte *)MK_FP(0x40,0x49); // store the text mode
		opMode = FLIGHT;                     // assume normal operating mode
		ParseCLP( argc, argv );              // parse command line args
		if (opMode == HELP)                  // if this is a help run
			 {                                 // then display the command
			 DisplayHelp();                    // list and exit
			 exit(0);
			 }
		if (opMode == VERSION)
			 {
			 DisplayVersion();
			 exit(0);
			 }
		StartUp();
		GetControls(tSV);                    // input.cpp: run one control pass
																				 // to initialize the state vector
		// main flight loop

	while(!Exit())                       // input.cpp: check for exit command
		{
				GetControls(tSV);                // input.cpp: get control settings
				RunFModel( tSV );                // aircraft.cpp: run flight model
				SoundCheck( tSV );
				GroundApproach();
				if (!UpdateView( tSV ))          // aircraft.cpp: make the next frame
					 Terminate("View switch file or memory error","UpdateView()");
				if (opMode != DEBUG)             // if not debugging...
					 blitscreen( bkground.image ); // display the new frame
				else                             // else if debugging...
			 VectorDump();                 // do the screen dump
				}
		checkpt = 4;                         // update progress flag
		Terminate("Normal program termination", "main()");
	}











