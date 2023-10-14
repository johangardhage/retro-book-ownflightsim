//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: VIEWCNTL.H
// Project:   Flights of Fantasy
// Creation:  September 12, 1992
// Author:    Mark Betz (MB)
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      9-12-1992       1.0     MB          initial development, functions
//                                          split off from AIRCRAFT.H
//      9-26-1992       1.1     MB          publication release
//
//      9-10-1995       2.0     mickRacky   update; cleanup
//
// Description
// -----------
//      funtion prototypes for control of the aircraft view
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef VIEWCNTL_H
#define VIEWCNTL_H

#define SLIPGAUGE_RATIO ((AILERON_RANGE / SLIP_RANGE) + 1)

// REV 2.1 doubled NUMBER_OF_DEGREES to provide more precision
// constant for fixed point circle
// HALF_CIRCLE would be 180 degrees in a 360 degree system
//static const HALF_CIRCLE = (NUMBER_OF_DEGREES>>1);
//static const QUARTER_CIRCLE = (NUMBER_OF_DEGREES>>2);
// offsets for view system, forward, right, rear, and left views, offset
// in degrees in the -180 to 180 system

static int rt2lft_ofs[4] = { 0, -QUARTER_CIRCLE, HALF_CIRCLE, QUARTER_CIRCLE };

// some constants to refer to value of current_view

enum { forward, right, rear, left };

static unsigned char *crshTxt;       // pointer to crash bitmap
static Pcx *bkGround;           // pointer to background in main
//World world;                  // Structure for world descriptor
static unsigned char current_view;       // holds current view direction
static int view_ofs;            // offset used for current view
static view_type curview;   	// Structure for view descriptor
static View winview;			// View object
// REV 2.1 use long instead of double!!
static long acPitch;  			// current aircraft rotations
static long acYaw;
static long acRoll;

// This block of declarations specifies the cockpit instrument objects

static FuelGauge *theFuelGauge;
static Altimeter *theAltimeter;
static KphDial *theKphDial;
static RpmGauge *theRpmGauge;
static SlipGauge *theSlipGauge;
static Compass *theCompass;
static IgnitionSwitch *theIgnitionSwitch;

// constants for use with 3D viewing system

static const int FWINC_X = 159;           // 159 center of front view
static const int FWINC_Y = 73;            // 73
static const int FWIN_X1 = 0;             // upper left corner of front view
static const int FWIN_Y1 = 0;
static const int FWIN_X2 = 319;           // lower right corner of front view
static const int FWIN_Y2 = 147;

static const int AWINC_X = 160;           // center of side and rear views
static const int AWINC_Y = 100;
static const int SWIN_X1 = 0;             // upper left corner of side view
static const int SWIN_Y1 = 0;
static const int SWIN_X2 = 319;           // lower right corner of side view
static const int SWIN_Y2 = 170;

static const int RWIN_X1 = 0;             // upper left corner of rear view
static const int RWIN_Y1 = 0;
static const int RWIN_X2 = 319;           // lower right corner of rear view
static const int RWIN_Y2 = 199;

static const int SKY_CLR = 11;            // palette color for sky
static const int GRND_CLR = 105;          // palette color for ground
static const int FCL_LEN = 400;           // focal plane distance

// instrument constants

static const int FUE_X = 49;              // fuel gauge location left
static const int FUE_Y = 141;             // fuel gauge location top
static const int ALT_X = 108;             // altimeter location left
static const int ALT_Y = 131;             // altimeter location top
static const int KPH_X = 171;             // speed dial location left
static const int KPH_Y = 130;             // speed dial location top
static const int RPM_X = 245;             // tach location left
static const int RPM_Y = 140;             // tach location top
static const int SLP_X = 102;             // slip gauge location left
static const int SLP_Y = 182;             // slip gauge location top
static const int COM_X = 33;              // compass location left
static const int COM_Y = 184;             // compass location top
static const int NEEDLE_CLR = 4;          // palette color for gauge needles

static const int GFX_LINE = 320;          // length of a screen line
static const int CRSH_TXT_X = 118;        // where the crash sign will go
static const int CRSH_TXT_Y = 78;
static const int CRSH_TXT_DX = 85;        // dimensions of the crash sign
static const int CRSH_TXT_DY = 24;

static unsigned char gfxLnBuf[GFX_LINE];       // line buffer for various gfx tricks

// instance, initialize, and test status of all the instrument objects
bool GaugesSetUp()
{
	bool result = true;

	theRpmGauge = new RpmGauge(RPM_X, RPM_Y, NEEDLE_CLR);
	theFuelGauge = new FuelGauge(FUE_X, FUE_Y, TANK_SIZE, NEEDLE_CLR);
	theAltimeter = new Altimeter(ALT_X, ALT_Y, NEEDLE_CLR);
	theKphDial = new KphDial(KPH_X, KPH_Y, NEEDLE_CLR);
	theSlipGauge = new SlipGauge(SLP_X, SLP_Y);
	theCompass = new Compass(COM_X, COM_Y);
	theIgnitionSwitch = new IgnitionSwitch();

	if (theFuelGauge->Status() != gaugeOk) {
		result = false;
	} else if (theAltimeter->Status() != gaugeOk) {
		result = false;
	} else if (theKphDial->Status() != gaugeOk) {
		result = false;
	} else if (theRpmGauge->Status() != gaugeOk) {
		result = false;
	} else if (theSlipGauge->Status() != gaugeOk) {
		result = false;
	} else if (theCompass->Status() != gaugeOk) {
		result = false;
	} else if (theIgnitionSwitch->Status() != gaugeOk) {
		result = false;
	}
	return(result);
}

// called from ACDump(). Prints view system internal parameters to the screen.
void ViewParamDump(int x, int &y)
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
	switch (current_view) {
	case 0: cprintf("ahead "); break;
	case 1: cprintf("right "); break;
	case 2: cprintf("rear  "); break;
	case 3: cprintf("left  "); break;
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
bool SetUpACDisplay()
{
	unsigned int bufSize;

	bool result = false;
	if (!(bkGround->Load("assets/doodads.pcx"))) {
		SetGfxBuffer(bkGround->Image());
		if (GaugesSetUp()) {
			bufSize = BufSize(44, 22, 128, 45);
			if ((crshTxt = new unsigned char[bufSize]) != NULL) {
				GetImage(44, 22, 128, 45, crshTxt);
				result = true;
			}
		}
	}
	return(result);
}

// this function is called from main() to initialize the view system, the
// pcx object ia passed in from main(), and are the same ones used to load 
// the title screen at the beginning of
// the program. Since we never deal with more than one PCX at a time we
// create a single PCX object and buffer in main() and pass them to other
// parts of the program as required.
bool InitView(Pcx *theBGround)
{
	bool result = false;

	// load the world from the file:
	if (!winview.SetWorld("assets/fof2.wld")) {
		bkGround = theBGround;
		//degree_mul = NUMBER_OF_DEGREES;
		//degree_mul /=360;
		current_view = 5;
		if (SetUpACDisplay()) {
			result = true;
		}
	}
	return(result);
}

// this function is called from main() at program termination to clean-up
// after the view system
void ViewShutDown()
{
	SetGfxBuffer(RETRO.framebuffer);
}

// this function flips the side view image so that it looks correct in either
// the right or left cockpit views. Could be done much faster in assembler,
// but, since it's called only when the right view is called for it's not
// really worth the effort at optimization

void FlipFrame(int x1, int y1, int x2, int y2, unsigned char *buf)
{
	int i, j, ofs, lines, pix;

	lines = (y2 - y1) + 1;
	pix = (x2 - x1) + 1;
	for (i = 0; i < lines; i++) {
		ofs = i * pix;
		memcpy(gfxLnBuf, (buf + ofs), pix);
		for (j = (pix - 1); j > -1; j--) {
			*(buf + ofs + ((pix - 1) - j)) = gfxLnBuf[j];
		}
	}
}

// This function destroys the current overlay buffer, if any, and allocates
// a new buffer to contain the frontview image. The frontview image is loaded
// into ram and then compressed into an RLE bitmap object which will be
// overlayed onto the 3D view for every frame
static bool LoadFrontView()
{
	bool result = false;

	if (!(bkGround->Load("assets/ckpit01.pcx"))) {
		if (bkGround->Compress()) {
			setpalette(bkGround->Palette());     // screen.asm
			SetGfxBuffer(bkGround->Image());
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

static bool LoadRightView()
{
	bool result = false;

	if (!(bkGround->Load("assets/sideview.pcx"))) {
		FlipFrame(0, 0, 319, 199, (unsigned char *)bkGround->Image());
		if (bkGround->Compress()) {
			SetGfxBuffer(bkGround->Image());
			result = true;
		}
	}
	return(result);
}

// this function first calls load() to load the left view pcx into
// the raw buffer. Finally compress() is called to compress the
// image in the raw buffer into the compressed buffer, creating an RLE bitmap
// object which will be overlayed on the 3D view for each frame.

static bool LoadLeftView()
{
	bool result = false;

	// PCX cleans up for itself
	//delete(bkGround->Image());
	//delete(bkGround->cimage);
	if (!(bkGround->Load("assets/sideview.pcx"))) {
		if (bkGround->Compress()) {
			SetGfxBuffer(bkGround->Image());
			result = true;
		}
	}
	return(result);
}

// This function calls the PCX
// object's load() function to load the rear view raw image into the raw
// image buffer. Lastly compress() is called to create the RLE
// bitmap object which will bve overlayed onto the 3D view for each frame.

static bool LoadRearView()
{
	bool result = false;

	// PCX now cleans up after itself
 //delete(bkGround->Image());
 //delete(bkGround->cimage);
	if (!(bkGround->Load("assets/tail.pcx"))) {
		if (bkGround->Compress()) {
			SetGfxBuffer(bkGround->Image());
			result = true;
		}
	}
	return(result);
}

// This function is called from UpdateDisplay() to update the direction
// of view, if any change has been made since the last pass

static bool ViewCheck(const AirCraft &AC)
{
	bool result = false;
	if (AC.view_state != current_view) {
		current_view = AC.view_state;
		view_ofs = rt2lft_ofs[current_view];
		if (current_view == 0) {
			if (LoadFrontView()) {
				result = true;
			}
		} else if (current_view == 1) {
			if (LoadRightView()) {
				result = true;
			}
		} else if (current_view == 3) {
			if (LoadLeftView()) {
				result = true;
			}
		} else {
			if (LoadRearView()) {
				result = true;
			}
		}
		if (AC.opMode == WALK) {
			winview.SetView(AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2, RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, bkGround->Image());
		} else if (current_view == 0) {
			winview.SetView(FWINC_X, FWINC_Y, FWIN_X1, FWIN_Y1, FWIN_X2, FWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, bkGround->Image());
		} else if (current_view == 1) {
			winview.SetView(AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2, SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, bkGround->Image());
		} else if (current_view == 3) {
			winview.SetView(AWINC_X, AWINC_Y, SWIN_X1, SWIN_Y1, SWIN_X2, SWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, bkGround->Image());
		} else {
			winview.SetView(AWINC_X, AWINC_Y, RWIN_X1, RWIN_Y1, RWIN_X2, RWIN_Y2, FCL_LEN, GRND_CLR, SKY_CLR, bkGround->Image());
		}
	} else {
		result = true;
	}
	return(result);
}

// This function updates the cockpit instrument display
static void UpdateInstruments(const AirCraft &AC)
{
	theKphDial->Set(AC.h_speed);
	theRpmGauge->Set(AC.rpm);
	theFuelGauge->Set(AC.fuel);
	theAltimeter->Set(AC.altitude);
	// REV 2.1 changed yaw to long
	//int direction = floor(AC.yaw);
	//if ( direction < 0)
	//  direction += 360;
	//if (direction)
	//  direction = 360 - direction;
	long direction;
	if (AC.yaw > 0) {
		direction = (long)(NUMBER_OF_DEGREES - AC.yaw);
	} else {
		direction = (long)(-AC.yaw); // invert sign,(no effect on zero)
	}

	// its a 360 degree compass
	theCompass->Set((int)(direction * 360 / NUMBER_OF_DEGREES));

	// REV 2.1 change aileron_pos to control range of {105 .. -105}
	// original control range was { 15..15 } AC.aileron_pos/2
	//  7.5 = _RANGE/x ; 2*_RANGE/15 = x
	theSlipGauge->Set(-(AC.aileron_pos / SLIPGAUGE_RATIO));  // slip gauge shows controls
	theIgnitionSwitch->Set(AC.ignition_on);
	if ((AC.brake) && (current_view == 0)) {
		Line(23, 161, 23, 157, 12);
	} else {
		Line(23, 161, 23, 157, 8);
	}
}

// This function performs a final rotation on the view angles to get the
// proper viewing direction from the cockpit. Note that the rotation values
// in the state vector are assigned to the file scope variables acPitch,
// acRoll, and acYaw. Final view calculation is done using these variables so
// that no changes have to be made to the state vector data.
static void ViewShift(const AirCraft &AC)
{
	acPitch = AC.pitch;
	acYaw = AC.yaw;
	acRoll = AC.roll;

	acYaw += view_ofs;
	switch (current_view) {
	case 1:
	{
		int temp = acRoll;
		acRoll = acPitch;
		acPitch = -temp;
		break;
	}
	case 2:
		acPitch = -(acPitch);
		acRoll = -(acRoll);
		break;
	case 3:
	{
		int temp = acRoll;
		acRoll = -(acPitch);
		acPitch = temp;
		break;
	}
	}

	// handle bounds checking on roll and yaw at 180 or -180
	if (acRoll > HALF_CIRCLE) {
		acRoll = acRoll - NUMBER_OF_DEGREES;
	} else if (acRoll < -HALF_CIRCLE) {
		acRoll = acRoll + NUMBER_OF_DEGREES;
	}
	if (acYaw > HALF_CIRCLE) {
		acYaw = acYaw - NUMBER_OF_DEGREES;
	} else if (acYaw < -HALF_CIRCLE) {
		acYaw = acYaw + NUMBER_OF_DEGREES;
	}

	// handle special case when aircraft pitch passes the vertical
	if ((acPitch > QUARTER_CIRCLE) || (acPitch < -QUARTER_CIRCLE)) {
		if (acRoll >= 0) {
			acRoll -= HALF_CIRCLE;
		} else if (acRoll < 0) {
			acRoll += HALF_CIRCLE;
		}
		if (acYaw >= 0) {
			acYaw -= HALF_CIRCLE;
		} else if (acYaw < 0) {
			acYaw += HALF_CIRCLE;
		}
		if (acPitch > 0) {
			acPitch = (HALF_CIRCLE - acPitch);
		} else if (acPitch < 0) {
			acPitch = (-HALF_CIRCLE - acPitch);
		}
	}
}

// This function maps the rotation system used in the flight model to the
// rotations used in the 3D viewing system.
static void MapAngles()
{

	// map the -HALF_CIRCLE .. HALF_CIRCLE rotation system being used in the flight model
	// to the rotation range in NUMBER_OF_DEGREES (fix.h) used in the
	// view system. The value of degree_mul is given by NUMBER_OF_DEGREES/360,
	// and is calculated once in InitAircraft()

	// REV 2.1 remove double and conversion...

	if (acPitch < 0) {          // requires conversion if negative
		acPitch += NUMBER_OF_DEGREES;
	}
	//acPitch *= degree_mul;
	if (acRoll < 0) {
		acRoll += NUMBER_OF_DEGREES;
	}
	// acRoll *= degree_mul;
	if (acYaw < 0) {
		acYaw += NUMBER_OF_DEGREES;
	}
	//acYaw *= degree_mul;

	// stuff the rotations fields of the struct we'll be passing to the 3D view generation system
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
	transput(CRSH_TXT_X, CRSH_TXT_Y, (CRSH_TXT_X + CRSH_TXT_DX) - 1, (CRSH_TXT_Y + CRSH_TXT_DY) - 1, crshTxt, bkGround->Image());
	blitscreen(bkGround->Image());
	RETRO_Flip();
	delay(3000);
}

// this function is called from main() to update the offscreen image buffer
// with the current aircraft instrument display, and view overlay. It also
// checks for changes in sound state, and toggles sound on/off in response
bool UpdateView(const AirCraft &AC)
{
	bool result = false;

	ViewShift(AC);
	// stuff the struct we'll send to the view system
	MapAngles();
	curview.copx = AC.x_pos;
	curview.copy = AC.y_pos;
	curview.copz = AC.z_pos;

	if (ViewCheck(AC)) {
		result = true;
		winview.Display(curview, 1);
		if (AC.opMode != WALK) {
			ctransput(bkGround->Cimage(), bkGround->Image());
			if (current_view == 0) {
				UpdateInstruments(AC);
			}
		}
	}
	return(result);
}

#endif
