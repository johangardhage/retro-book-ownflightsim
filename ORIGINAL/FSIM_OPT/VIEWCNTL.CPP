//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: VIEWCNTL.CPP
// Project:   Flights of Fantasy
// Creation:  September 12, 1992
// Author:    Mark Betz (MB)
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      9-25-1992       1.1     MB          Final release for publication
//      10-20-95        2.1     mracky      tweak; double NUMBER_OF_DEGREES
//		                                      (see MKSIN512.CPP); change 
//                                            double yaw, pitch, roll to long
//                                            remove 360 inner system
//
//
// Description
// -----------
//      Provides funtions for control of the aircraft view
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>
#include <mem.h>
#include <conio.h> // gotoxy
#include <math.h>  // floor

#include "poly2.h"                        // polygons
#include "view.h"                        // view class
#include "pcx.h"                         // pcx handling
#include "gauges.h"                      // instrument classes
#include "screen.h"                      // video control
#include "fix.h"                         // degree system fix-up
#include "viewcntl.h"                    // header for this module

// REV 2.1 doubled NUMBER_OF_DEGREES to provide more precision
// constant for fixed point circle
// HALF_CIRCLE would be 180 degrees in a 360 degree system
//static const HALF_CIRCLE = (NUMBER_OF_DEGREES>>1);
//static const QUARTER_CIRCLE = (NUMBER_OF_DEGREES>>2);
// offsets for view system, forward, right, rear, and left views, offset
// in degrees in the -180 to 180 system

static int rt2lft_ofs[4] = {0, -QUARTER_CIRCLE, HALF_CIRCLE, QUARTER_CIRCLE};

// some constants to refer to value of current_view

enum { forward, right, rear, left };

static byte far* crshTxt;       // pointer to crash bitmap
static Pcx* bkGround;           // pointer to background in main
//World world;                  // Structure for world descriptor
static byte current_view;       // holds current view direction
static int view_ofs;            // offset used for current view
static view_type curview;   	// Structure for view descriptor
static View winview;			// View object
	// REV 2.1 use long instead of double!!
static long acPitch;  			// current aircraft rotations
static long acYaw;
static long acRoll;
static opMode;                  // = 1 if flight, 2 if world walk

// This block of declarations specifies the cockpit instrument objects

static FuelGauge *theFuelGauge;
static Altimeter *theAltimeter;
static KphDial *theKphDial;
static RpmGauge *theRpmGauge;
static SlipGauge *theSlipGauge;
static Compass *theCompass;
static IgnitionSwitch *theIgnitionSwitch;

// pointer to default screen buffer
static void far* screen_ptr = MK_FP( 0xa000, 0 );

// constants for use with 3D viewing system

static const XORIGIN = 160;           // Screen origin in X dimension
static const YORIGIN = 90;            // Screen origin in Y dimension
static const FWINC_X = 159;           // 159 center of front view
static const FWINC_Y = 73;            // 73
static const FWIN_X1 = 0;             // upper left corner of front view
static const FWIN_Y1 = 0;
static const FWIN_X2 = 319;           // lower right corner of front view
static const FWIN_Y2 = 147;

static const AWINC_X = 160;           // center of side and rear views
static const AWINC_Y = 100;
static const SWIN_X1 = 0;             // upper left corner of side view
static const SWIN_Y1 = 0;
static const SWIN_X2 = 319;           // lower right corner of side view
static const SWIN_Y2 = 170;

static const RWIN_X1 = 0;             // upper left corner of rear view
static const RWIN_Y1 = 0;
static const RWIN_X2 = 319;           // lower right corner of rear view
static const RWIN_Y2 = 199;           

static const SKY_CLR = 11;            // palette color for sky
static const GRND_CLR = 105;          // palette color for ground
static const FCL_LEN = 400;           // focal plane distance
         
// instrument constants

static const FUE_X = 49;              // fuel gauge location left
static const FUE_Y = 141;             // fuel gauge location top
static const ALT_X = 108;             // altimeter location left
static const ALT_Y = 131;             // altimeter location top
static const KPH_X = 171;             // speed dial location left
static const KPH_Y = 130;             // speed dial location top
static const RPM_X = 245;             // tach location left
static const RPM_Y = 140;             // tach location top
static const SLP_X = 102;             // slip gauge location left
static const SLP_Y = 182;             // slip gauge location top
static const COM_X = 33;              // compass location left
static const COM_Y = 184;             // compass location top
static const NEEDLE_CLR = 4;          // palette color for gauge needles

static const GFX_LINE = 320;          // length of a screen line
static const CRSH_TXT_X = 118;        // where the crash sign will go
static const CRSH_TXT_Y = 78;
static const CRSH_TXT_DX = 85;        // dimensions of the crash sign
static const CRSH_TXT_DY = 24;

static const TANK_SIZE = 8;
static const DEBUG = 0;               // operating mode constants
static const FLIGHT = 1;
static const WALK = 2;

static byte gfxLnBuf[GFX_LINE];       // line buffer for various gfx tricks

// instance, initialize, and test status of all the instrument objects

boolean GaugesSetUp()
{
    boolean result = true;

	theRpmGauge = new RpmGauge(RPM_X, RPM_Y, NEEDLE_CLR);
	theFuelGauge = new FuelGauge(FUE_X, FUE_Y, TANK_SIZE, NEEDLE_CLR);
	theAltimeter = new Altimeter(ALT_X, ALT_Y, NEEDLE_CLR);
	theKphDial = new KphDial(KPH_X, KPH_Y, NEEDLE_CLR);
	theSlipGauge = new SlipGauge(SLP_X, SLP_Y);
	theCompass = new Compass(COM_X, COM_Y);
	theIgnitionSwitch = new IgnitionSwitch();

	if (theFuelGauge->Status() != gaugeOk)
		result = false;
	else if (theAltimeter->Status() != gaugeOk)
		result = false;
	else if (theKphDial->Status() != gaugeOk)
		result = false;
	else if (theRpmGauge->Status() != gaugeOk)
		result = false;
	else if (theSlipGauge->Status() != gaugeOk)
		result = false;
	else if (theCompass->Status() != gaugeOk)
		result = false;
	else if (theIgnitionSwitch->Status() != gaugeOk)
		result = false;
	return( result );
}

// called from ACDump(). Prints view system internal parameters to the
// screen.

void ViewParamDump( int x, int& y )
{
    gotoxy(x, y++);
    cprintf("view parameters dump:");
    y++;
    gotoxy(x, y++);
    cprintf("view theta x:          %i       ", curview.xangle);
    gotoxy(x, y++);
    cprintf("view theta y:          %i       ", curview.yangle);
    gotoxy(x, y++);
    cprintf("view theta z:          %i       ", curview.zangle);

    gotoxy(x, y++);
    cprintf("view direction:        ");
    switch (current_view)
       {
       case 0 : cprintf("ahead "); break;
       case 1 : cprintf("right "); break;
       case 2 : cprintf("rear  "); break;
       case 3 : cprintf("left  "); break;
       }
    gotoxy(x, y++);
    cprintf("view offset:           %i       ", view_ofs);
}


// This function uses the Pcx object "bkGround" to get the forward cockpit
// view PCX file into ram. The image field of the struct pointed to by
// bkGround contains a far pointer to the 64,000 byte buffer created by
// the loader to hold the pcx image. This buffer will be used as the
// offscreen view buffer for the duration of execution. After the cockpit
// pcx is loaded the GaugesSetUp() function is called. This initializes the
// cockpit instrument graphics. The function then creates buffers for the
// upper and lower parts of the cockpit and grabs them from the offscreen
// view buffer.

boolean SetUpACDisplay()
{
   word bufSize;

   boolean result = false;
   if (!(bkGround->load("doodads.pcx"))) {
      SetGfxBuffer( bkGround->Image() );
      if ( GaugesSetUp() )    {
         bufSize = BufSize( 44, 22, 128, 45 );
         if ((crshTxt = new byte[ bufSize ]) != NULL)     {
            GetImage( 44, 22, 128, 45, crshTxt );
            result = true;
         }
      }
   }
   return( result );
}

// this function is called from main() to initialize the view system, the
// pcx object ia passed in from main(), and are the same ones used to load 
// the title screen at the beginning of
// the program. Since we never deal with more than one PCX at a time we
// create a single PCX object and buffer in main() and pass them to other
// parts of the program as required.

boolean InitView( Pcx* theBGround, int mode)
{
   boolean result = false;

   // load the world from the file:
	if( ! winview.SetWorld("fof2.wld") )
	{
		bkGround = theBGround;
		//degree_mul = NUMBER_OF_DEGREES;
		//degree_mul /=360;
		current_view = 5;
    	opMode = mode;
    	if (SetUpACDisplay())
      		result = true;
	}
   	return(result);
}

void ResetPalette()
{
	winview.PaletteReset();
}
// this function is called from main() at program termination to clean-up
// after the view system

void ViewShutDown()
{
   SetGfxBuffer( screen_ptr );
}

// this function flips the side view image so that it looks correct in either
// the right or left cockpit views. Could be done much faster in assembler,
// but, since it's called only when the right view is called for it's not
// really worth the effort at optimization

void FlipFrame( int x1, int y1, int x2, int y2, byte far* buf)
{
   int i, j, ofs, lines, pix;

   lines = (y2 - y1) + 1;
   pix = (x2 - x1) + 1;
   for (i = 0; i < lines; i++)
      {
      ofs = i * pix;
      _fmemcpy(gfxLnBuf, (buf + ofs), pix);
      for (j = (pix - 1); j > -1; j--)
        *(buf + ofs + ((pix - 1) - j)) = gfxLnBuf[j];
      }
}

// This function destroys the current overlay buffer, if any, and allocates
// a new buffer to contain the frontview image. The frontview image is loaded
// into ram and then compressed into an RLE bitmap object which will be
// overlayed onto the 3D view for every frame

static boolean LoadFrontView()
{
   boolean result = false;

   if (!(bkGround->load("ckpit01.pcx")))
      {
      if (bkGround->compress( ))
         {
		 setpalette( bkGround->Palette());     // screen.asm
		 SetGfxBuffer( bkGround->Image());
         result = true;
         }
      }
   return(result);
}

// this function first calls load() to load the left view pcx into
// the raw buffer. FlipFrame() is then called to turn the left view into a
// right view, and finally compress() is called to compress the
// image in the raw buffer into the compressed buffer, creating an RLE bitmap
// object which will be overlayed on the 3D view for each frame.

static boolean LoadRightView()
{
   boolean result = false;

   if (!(bkGround->load("sideview.pcx")))
      {
      FlipFrame(0, 0, 319, 199, (byte far*)bkGround->Image());
      if (bkGround->compress( ))
         {
		 SetGfxBuffer( bkGround->Image() );
         result = true;
         }
      }
   return(result);
   }

// this function first calls load() to load the left view pcx into
// the raw buffer. Finally compress() is called to compress the
// image in the raw buffer into the compressed buffer, creating an RLE bitmap
// object which will be overlayed on the 3D view for each frame.

static boolean LoadLeftView()
{
   boolean result = false;

   // PCX cleans up for itself
   //delete(bkGround->Image());
   //delete(bkGround->cimage);
   if (!(bkGround->load("sideview.pcx")))
      {
      if (bkGround->compress( ))
         {
         SetGfxBuffer( bkGround->Image() );
         result = true;
         }
      }
   return(result);
}

// This function calls the PCX
// object's load() function to load the rear view raw image into the raw
// image buffer. Lastly compress() is called to create the RLE
// bitmap object which will bve overlayed onto the 3D view for each frame.

static boolean LoadRearView()
   {
   boolean result = false;

   	// PCX now cleans up after itself
   //delete(bkGround->Image());
   //delete(bkGround->cimage);
   if (!(bkGround->load("tail.pcx")))
      {
      if (bkGround->compress( ))
         {
         SetGfxBuffer( bkGround->Image() );
         result = true;
         }
      }
   return(result);
   }

// This function is called from UpdateDisplay() to update the direction
// of view, if any change has been made since the last pass

static boolean ViewCheck( int view_state )
   {
   boolean result = false;
   if (view_state != current_view)
      {
      current_view = view_state;
      view_ofs = rt2lft_ofs[ current_view ];
      if (current_view == 0)
         {
         if (LoadFrontView())
            result = true;
         }
      else if (current_view == 1)
         {
         if (LoadRightView())
            result = true;
         }
      else if (current_view == 3)
         {
         if (LoadLeftView())
            result = true;
			}
		else
			{
			if (LoadRearView())
				result = true;
			}
		if (opMode == WALK)
		  winview.SetView( AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2,
									 RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR,
							bkGround->Image() );
		else if (current_view == 0)
		  winview.SetView( FWINC_X, FWINC_Y, FWIN_X1, FWIN_Y1, FWIN_X2,
                            FWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR,
							bkGround->Image() );
      else if (current_view == 1)
         winview.SetView( AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2,
                            SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR,
							bkGround->Image() );
      else if (current_view == 3)
         winview.SetView( AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2,
                            SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR,
							bkGround->Image() );
      else
		  winview.SetView( AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2,
                            RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR,
							bkGround->Image() );
      }
   else result = true;
   return(result);
   }

// This function updates the cockpit instrument display

static void UpdateInstruments( const AirCraft & AC )
{

   	theKphDial->Set( AC.h_speed );
   	theRpmGauge->Set( AC.rpm );
   	theFuelGauge->Set( AC.fuel );
   	theAltimeter->Set(AC.altitude);
		// REV 2.1 changed yaw to long
   	//int direction = floor(AC.yaw);
   	//if ( direction < 0)
	//  direction += 360;
    //if (direction)
	//  direction = 360 - direction;
	long direction;
   	if( AC.yaw > 0)
		direction = (long)(NUMBER_OF_DEGREES - AC.yaw); 
	else
	   	direction = (long)(-AC.yaw); // invert sign,(no effect on zero)

		// its a 360 degree compass
   theCompass->Set( (int)(direction*360/NUMBER_OF_DEGREES) );

   		// REV 2.1 change aileron_pos to control range of {105 .. -105}
		// original control range was { 15..15 } AC.aileron_pos/2
		//  7.5 = _RANGE/x ; 2*_RANGE/15 = x
#define SLIPGAUGE_RATIO ((AILERON_RANGE/SLIP_RANGE)+1)
   theSlipGauge->Set( -(AC.aileron_pos / SLIPGAUGE_RATIO) );  // slip gauge shows controls
   theIgnitionSwitch->Set( AC.ignition_on );
   if ((AC.brake) && (current_view == 0))
	  Line(23,161,23,157,12);
   else
	  Line(23,161,23,157,8);
}

// This function performs a final rotation on the view angles to get the
// proper viewing direction from the cockpit. Note that the rotation values
// in the state vector are assigned to the file scope variables acPitch,
// acRoll, and acYaw. Final view calculation is done using these variables so
// that no changes have to be made to the state vector data.

static void ViewShift( const AirCraft& AC )
{
	acPitch = AC.pitch;
	acYaw = AC.yaw;
	acRoll = AC.roll;

	acYaw += view_ofs;
	switch (current_view) {
		case 1:
		{
			int temp=acRoll;
			acRoll=acPitch;
			acPitch=-temp;
			break;
		}
		case 2:
			acPitch=-(acPitch);
			acRoll=-(acRoll);
			break;
		case 3:
		{
			int temp=acRoll;
			acRoll=-(acPitch);
			acPitch=temp;
			break;
		}
	}

	// handle bounds checking on roll and yaw at 180 or -180
	if (acRoll > HALF_CIRCLE)
		 acRoll = acRoll - NUMBER_OF_DEGREES;
	else if (acRoll < -HALF_CIRCLE)
		 acRoll = acRoll + NUMBER_OF_DEGREES;
	if (acYaw > HALF_CIRCLE)
		 acYaw =  acYaw - NUMBER_OF_DEGREES;
	else if (acYaw < -HALF_CIRCLE)
		 acYaw = acYaw + NUMBER_OF_DEGREES;

	// handle special case when aircraft pitch passes the vertical
	if ((acPitch > QUARTER_CIRCLE) || (acPitch < -QUARTER_CIRCLE))
		 {
		 if (acRoll >= 0)
				acRoll -= HALF_CIRCLE;
		 else if (acRoll < 0)
				acRoll += HALF_CIRCLE;
		 if (acYaw >= 0)
				acYaw -= HALF_CIRCLE;
		 else if (acYaw < 0)
				acYaw += HALF_CIRCLE;
		 if (acPitch > 0)
				acPitch = (HALF_CIRCLE - acPitch);
		 else if (acPitch < 0)
				acPitch = (-HALF_CIRCLE - acPitch);
		 }
}

// This function maps the rotation system used in the flight model to the
// rotations used in the 3D viewing system.

static void near MapAngles()
{

    // map the -HALF_CIRCLE .. HALF_CIRCLE rotation system being used in the flight model
    // to the rotation range in NUMBER_OF_DEGREES (fix.h) used in the
    // view system. The value of degree_mul is given by NUMBER_OF_DEGREES/360,
    // and is calculated once in InitAircraft()

	// REV 2.1 remove double and conversion...

    if (acPitch < 0)           // requires conversion if negative
       acPitch += NUMBER_OF_DEGREES;
    //acPitch *= degree_mul;
    if (acRoll < 0)
       acRoll += NUMBER_OF_DEGREES;
    // acRoll *= degree_mul;
    if (acYaw < 0)
       acYaw += NUMBER_OF_DEGREES;
    //acYaw *= degree_mul;

    // stuff the rotations fields of the struct we'll be passing to the
    // 3D view generation system
#ifdef NOTUSED
    curview.xangle = floor(acPitch);
    curview.yangle = floor(acYaw);
    curview.zangle = floor(acRoll);
#else
    curview.xangle = acPitch;
    curview.yangle = acYaw;
    curview.zangle = acRoll;
#endif
}

// this function displays the "CRASH!" icon

void ShowCrash()
	{
	transput( CRSH_TXT_X, CRSH_TXT_Y, (CRSH_TXT_X + CRSH_TXT_DX) - 1,
			  (CRSH_TXT_Y + CRSH_TXT_DY) - 1, crshTxt, screen_ptr );
	}

// this function is called from main() to update the offscreen image buffer
// with the current aircraft instrument display, and view overlay. It also
// checks for changes in sound state, and toggles sound on/off in response

boolean UpdateView( const AirCraft& AC )
{
	boolean result = false;

	ViewShift( AC );
	 // stuff the struct we'll send to the view system
	MapAngles();
	curview.copx = AC.x_pos;
	curview.copy = AC.y_pos;
	curview.copz = AC.z_pos;

	if (ViewCheck( AC.view_state )) {
		result = true;
		winview.Display( curview, 1);
		if (opMode != WALK) {
			ctransput( bkGround->Cimage(), bkGround->Image() );
			if (current_view == 0)
				  UpdateInstruments( AC );
		}
	}
	return(result);
}

