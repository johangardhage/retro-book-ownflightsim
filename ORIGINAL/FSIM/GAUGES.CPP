//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: GAUGES.CPP
// Project:   Flights of Fantasy
// Creation:  January 18, 1991
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
//
// Includes:  mem.h, types.h, gauges.h, screen.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      1-18-1992       1.0     MB          Initial development
//      8-27-1992       1.1b    MB          Includes fixes and accomodated
//                                          reworked overlay system
//      9-26-1992       1.1     MB          Publication release
//      10-26-95        2.1     mrcaky@sirius.com  Fix rpm gauge bug
//
// Description
// -----------
//      Member function definitions for gauge classes defined in GAUGES.H
//
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <mem.h>
#ifndef __TYPES__
	#include "types.h"
#endif
#ifndef __GAUGES__
	#include "gauges.h"
#endif
#ifndef __SCREEN__
	#include "screen.h"
#endif


// Array of points to draw fuel gauge needle to
const byte fuelPoints[9][2]  = {{1, 10}, {2, 6},  {5, 3},
								{9, 1},  {13, 0}, {17, 1},
								{21, 3}, {24, 6}, {25, 10}};

// Array of points to draw altimeter thousands needle to
const byte altTPoints[20][2] = {{19, 6},  {22, 6},  {25, 7},
								{27, 9},  {29, 11}, {30, 15},
								{29, 19}, {27, 21}, {25, 23},
								{22, 24}, {19, 24}, {16, 24},
								{13, 23}, {11, 21}, {9, 19},
								{8, 15},  {9, 11},  {11, 9},
								{13, 7},  {16, 6}};

// Array of points to draw altimeter hundreds needle to
const byte altHPoints[40][2] = {{19, 0},  {22, 0},  {24, 1},
								{26, 1},  {29, 2},  {31, 3},
								{33, 5},  {35, 7},  {36, 9},
								{38, 12}, {38, 15}, {38, 18},
								{36, 21}, {35, 23}, {33, 25},
								{32, 27}, {29, 28}, {26, 29},
								{24, 29}, {22, 30}, {19, 30},
								{16, 30}, {14, 29}, {12, 29},
								{11, 28}, {7, 27},  {5, 25},
								{3, 23},  {2, 21},  {0, 18},
								{0, 15},  {0, 12},  {2, 9},
								{3, 7},   {5, 5},   {7, 3},
								{9, 2},   {12, 1},  {14, 1},
								{16, 0}};

// array of points to draw airspeed needle to
const byte mphPoints[32][2] =  {{21, 0},  {25, 0},  {29, 1},
								{33, 3},  {36, 5},  {38, 7},
								{40, 10}, {42, 13}, {42, 16},
								{42, 19}, {40, 22}, {38, 25},
								{36, 27}, {33, 29}, {29, 31},
								{25, 32}, {21, 32}, {17, 32},
								{13, 31}, {9, 29},  {6, 27},
								{4, 25},  {2, 22},  {0, 19},
								{0, 16},  {0, 13},  {2, 10},
								{4, 7},   {6, 5},   {9, 3},
								{13, 1},  {17, 0}};

const byte rpmPoints[17][2] =  {{13, 0},  {16, 0},  {19, 1},
								{21, 2},  {23, 4},  {25, 7},
								{26, 11}, {25, 15}, {23, 18},
								{19, 21}, {13, 22}, {7, 21},
								{3, 18},  {1, 15},  {0, 11},
								{1, 7},   {3, 4}};

// array of points for locating slip ball
const byte slipPoints[19][2] = {{4, 1},  {10, 2}, {16, 3},
								{22, 4}, {28, 4}, {34, 5},
								{40, 5}, {46, 5}, {51, 5},
								{55, 5}, {60, 5}, {64, 5},
								{69, 5}, {75, 5}, {81, 4},
								{87, 4}, {93, 3}, {99, 3},
								{105, 1}};

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: FuelGauge, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location, tankSize - number of gallons
// held in fuel tank, currentGals - number of gallons presently in fuel tank,
// nColor - needled palette color

FuelGauge::FuelGauge(int origX, int origY, byte tankSize, byte nColor)
	{
	centerX = 13;
	centerY = 10;
	scrOrigX = origX;
	scrOrigY = origY;
	sizeOfTank = tankSize;
	needleColor = nColor;
	stat = gaugeOk;
	return;
	}

// this function sets the instrument reading, newGals - new gallon setting

void FuelGauge::Set(word newGals)
	{
	byte eighthTank = sizeOfTank/8;
	int i, pX, pY;
	for (i = 0; i < 9; i++)
		{
		if ((i * eighthTank) > newGals)
			{
			pX = fuelPoints[i - 1][0];
			pY = fuelPoints[i - 1][1];
			break;
			}
		else if ((i * eighthTank) == newGals)
			{
			pX = fuelPoints[i][0];
			pY = fuelPoints[i][1];
			break;
			}
		}
	Line((scrOrigX + centerX), (scrOrigY + centerY),
		 (scrOrigX + pX), (scrOrigY + pY), needleColor );
	return;
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: Altimeter, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location, nColor - needle palette color

Altimeter::Altimeter(int origX, int origY, byte nColor)
	{
	centerX = 19;
	centerY = 15;
	scrOrigX = origX;
	scrOrigY = origY;
	needleColor = nColor;
	stat = gaugeOk;
	return;
	}

// this function sets the instrument reading, feet - number of feet

void Altimeter::Set(word feet)
	{
	int hPX, hPY, tPX, tPY, fPX, fPY;
	int temp;

	feet = (feet % 10000);
	temp = (feet / 500);
	tPX = altTPoints[temp][0];
	tPY = altTPoints[temp][1];
	feet = (feet % 1000);
	temp = (feet / 50);
	hPX = altTPoints[temp][0];
	hPY = altTPoints[temp][1];
    feet = (feet % 100);
    temp = (feet / 2.5);
    fPX = altHPoints[temp][0];
    fPY = altHPoints[temp][1];
	Line((scrOrigX + centerX), (scrOrigY + centerY),
		 (scrOrigX + hPX), (scrOrigY + hPY), needleColor);
	Line((scrOrigX + centerX), (scrOrigY + centerY),
		 (scrOrigX + tPX), (scrOrigY + tPY), 7);
	Line((scrOrigX + centerX), (scrOrigY + centerY),
		 (scrOrigX + fPX), (scrOrigY + fPY), 12);

	return;
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: KphDial, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location, nColor - needle palette color

KphDial::KphDial(int origX, int origY, byte nColor)
	{
	centerX = 21;
	centerY = 16;
	scrOrigX = origX;
	scrOrigY = origY;
	needleColor = nColor;
	stat = gaugeOk;
	return;
	}

// this function sets the instrument reading, speed - speed in mph

void KphDial::Set(word speed)
	{
	int pX, pY;
	if (speed > 175)
		speed = 175;
	if (speed <= 20)
		speed = 0;
	else if (speed <= 30)
		speed = (speed / 10) - 1;
	else
		speed = (speed / 5) - 4;
	pX = mphPoints[speed][0];
	pY = mphPoints[speed][1];
	Line((scrOrigX + centerX), (scrOrigY + centerY),
		 (scrOrigX + pX), (scrOrigY + pY), needleColor);
	return;
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: RpmGauge, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location, nColor - needle palette color

RpmGauge::RpmGauge(int origX, int origY, byte nColor)
{
	centerX = 13;
	centerY = 11;
	scrOrigX = origX;
	scrOrigY = origY;
	needleColor = nColor;
	stat = gaugeOk;
	return;
}

// this function sets the instrument reading, rpms - rpm in range 0..2000

void RpmGauge::Set(word rpms)
{
		// REV 2.1 the rpm range in AIRCRAFT actually exceeds 2000 when the
		//       looptime gets small enough...
	if( rpms > 2000)
		rpms = 2000;
	rpms = rpms / 125;
	int pX = rpmPoints[rpms][0];
	int pY = rpmPoints[rpms][1];
	Line((scrOrigX + centerX), (scrOrigY + centerY),
				(scrOrigX + pX), (scrOrigY + pY), needleColor);
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: SlipGauge, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location

SlipGauge::SlipGauge(int origX, int origY)
	{
	scrOrigX = origX;
	scrOrigY = origY;
	GetImage((scrOrigX + 55), (scrOrigY + 5), (scrOrigX + 60),
			 (scrOrigY + 10), ballFrame);
	BarFill((scrOrigX + 55), (scrOrigY + 5), (scrOrigX + 61),
			(scrOrigY + 10), 15);
	stat = gaugeOk;
	return;
	}

// this function sets the instrument reading, deflection - ball deflection
// -9..9

void SlipGauge::Set(int deflection)
	{
	int pX, pY;

	if ((deflection < -9) || (deflection > 9))
		return;
	deflection = deflection + 9;
	pX = slipPoints[deflection][0];
	pY = slipPoints[deflection][1];
	PutImage((scrOrigX + pX), (scrOrigY + pY), (scrOrigX + pX + 5),
			 (scrOrigY + pY + 5), ballFrame);
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: Compass, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: origX, origY - screen location

Compass::Compass(int origX, int origY)
	{
	framWid = 25;
	framDep = 5;
	stripWid = 120;
	stripDep = 5;
	if ((cStrip = new byte[(stripWid + 1) * (stripDep + 1)]) == NULL)
		stat = gaugeInitFailed;
	else
		{
		scrOrigX = origX;
		scrOrigY = origY;
		GetImage(100, 95, 219, 100, cStrip);
		stat = gaugeOk;
		}
	return;
	}

// destructor: frees up allocated ram

Compass::~Compass()
	{
	if (cStrip != NULL)
		delete(cStrip);
	return;
	}

// this function sets the instrument reading, heading - heading 0..359

void Compass::Set(word heading)
	{
	if ((heading <= 359) && ((heading % 3) == 0))
        {
		if (heading)
			heading = (heading / 3);
		heading += 60;
		if (heading > 120)
			heading -= 120;
        lastDir = heading;
        }
	BlitBox(lastDir);
	return;
	}

// this function moves the correct portion of the compass direction strip
// to the display. It's slow and kludgy, but not a large part of the program
// overhead at this point in time.

void near Compass::BlitBox(word centerX)
	{
	int fWinStrt;                           // left x of strip blit window
    int i, j;                               // counters
	word lineLen;                           // length of line copied from strip
	boolean blit2 = false;                  // true if need to blit twice
	word blit2Len;                          // length of 2nd line if any
	byte *pixGet;                           // pointer into strip
	byte *pixPut;                           // pointer into compass window
	word putLnWrap;                         // value to wrap to next put line
	word getLnWrap;                         // value to wrap to next get line

	fWinStrt = centerX - (framWid / 2);     // fwinstart = x1?
	if (fWinStrt < 0)                       // calculate left edge of strip
		fWinStrt = stripWid + fWinStrt;
	if ((fWinStrt + framWid) > stripWid)    // if it extends past end of strip
		{
		lineLen = stripWid - fWinStrt;      // roll back line length
		blit2 = true;                       // set two-blit flag
		blit2Len = framWid - lineLen;       // calculate second line length
		}
	else
		lineLen = framWid;
	pixGet = cStrip + fWinStrt;             // set-up starting pointers
	pixPut = (byte *)XyToPtr(scrOrigX, scrOrigY);
	putLnWrap = 320 - lineLen;              // calculate line-wrap values
	getLnWrap = stripWid - lineLen;

	for (i = 0; i < framDep; i++)           // cycle through lines
		{
		for (j = 0; j < lineLen; j++)       // cycle through pixels
			{
			*pixPut = *pixGet;              // should at least set this up
			pixPut++;                       // as a line move for next ver.
			pixGet++;
			}
		pixPut += putLnWrap;                // wrap to the next get/put line
		pixGet += getLnWrap;
		}

	if (blit2)                              // do the second blit, if any,
		{                                   // the same way
		pixGet = cStrip;
		pixPut = (byte *)XyToPtr(scrOrigX + lineLen, scrOrigY);
		putLnWrap = 320 - blit2Len;
		getLnWrap = stripWid - blit2Len;
		for (i = 0; i < framDep; i++)
			{
			for (j = 0; j < blit2Len; j++)
				{
				*pixPut = *pixGet;
				pixPut++;
				pixGet++;
				}
			pixPut += putLnWrap;
			pixGet += getLnWrap;
			}
		}
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// CLASS: IgnitionSwitch, member function definitions
//-------+---------+---------+---------+---------+---------+---------+---------+

// constructor: no args, allocates memory for switch bitmaps, then grabs
// them

IgnitionSwitch::IgnitionSwitch()
	{
	if (((switch_on_map = new byte[210]) == NULL) ||
		((light_on_map = new byte[105]) == NULL))
		{
		stat = gaugeInitFailed;
		return;
		}
	GetImage(160, 64, 171, 80, switch_on_map);
	GetImage(142, 64, 151, 73, light_on_map);
	stat = gaugeOk;
	return;
	}

// destructor: frees up allocated memory

IgnitionSwitch::~IgnitionSwitch()
	{
	if (light_on_map != NULL)
		delete(light_on_map);
	if (switch_on_map != NULL)
		delete(switch_on_map);
	return;
	}

// this function sets the instrument reading, onOff - boolean true = on

void IgnitionSwitch::Set(byte onOff)
	{
	if (onOff)
		{
		PutImage(279, 177, 290, 193, switch_on_map);
		PutImage(287, 161, 296, 170, light_on_map);
		}
	return;
	}

