//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: GAUGES.H
// Project:   Flights of Fantasy
// Creation:  January 17, 1992
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
//      1-17-92         1.0     MB          Initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//		10-12-95        2.0     mracky      update;
//		10-22-95        2.1     mracky      changed Set arg to int (formerly word)
// Description
// -----------
//      Class and type definitions for the flight simulator cockpit instrument
//      panel.
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef GAUGES_H
#define GAUGES_H

#ifndef TYPES_H
#include "types.h"
#endif
// Each gauge class contains a Status() member function that can be called at
// any time to get the status of the gauge. Status() returns one of the follow-
// ing codes. The Status() member should be called immediately after an object
// of a gauge class is instanced, in order to be certain the gauge initialized.

enum gaugeStat {gaugeInitFailed, gaugeBroken, gaugeOk};

// Class DialInstruments is a virtual base class from which the specific dial
// type instrument classes are derived below. Note that the presence of the pure
// virtual function DialIntrument::Set() makes this a pure virtual class. As
// such it is illegal to declare an instance of DialInstrument. Set() is redef-
// ined in class-specific ways in the classes below. Also note the inline func-
// tion DialInstrument::Status().

class DialInstrument
	{
	public:                        // PUBLICS
								// REV 2.1 arg changed to int from word
	virtual void Set(int) = 0;    // set a new reading
	gaugeStat Status()
				 {return(stat);};  // returns status of gauge

	protected:                     // PROTECTED
	int scrOrigX;                  // x,y coords where the corner of the...
	int scrOrigY;                  // save rectangle is displayed
	byte needleColor;              // the color used to draw the needle
	int centerX;                   // center of the gauge x coordinate
	int centerY;                   // center of the gauge y coordinate
	gaugeStat stat;                // current gauge status
	};

// Class FuelGauge. Publicly derived from DialInstrument. Control class for the
// cockpit fuel gauge display. The constructor takes the screen origin for the
// gauge in origX and origY, the size of the fuel tank in tankSize, the current
// gauge reading in currentGals, and the desired needle color in nColor.
//
// There is one member function to control the gauge display. Set() is used to
//  update the display. The parameter newGals should contain the current level
// of fuel in the tank. Note that if you pass Set() more gallons then the tank
// can hold it will peg at Full.

class FuelGauge : public DialInstrument
	{
	public:                        // PUBLICS
	// constructor and destructor
	FuelGauge(int origX, int origY, byte tankSize, byte nColor);
	void Set(int newGals);        // set a new reading

	private:                       // PRIVATES
	byte sizeOfTank;               // size of fuel tank in gallons
	};

// Class Altimeter, publicly derived from class DialInstrument. Control class
// for the cockpit altimeter display. Set() accepts the altitude in feet in the
// feet parameter. The Constructor takes the screen origin of the gauge, and
// the needle color.

class Altimeter : public DialInstrument
	{
	public:                        // PUBLICS
	Altimeter(int origX, int origY, byte nColor);
	void Set(int feet);
	};

// Class KphDial, publicly derived from class DialInstrument. Control class
// for the cockpit airspeed display. Set() accepts the speed in MPH in the
// speed parameter. The Constructor takes the screen origin of the gauge, and
// the needle color.

class KphDial : public DialInstrument
	{
	public:                        // PUBLICS
	KphDial(int origX, int origY, byte nColor);
	void Set(int speed);
	};

// Class RpmGauge, publicly derived from class DialInstrument. Control class
// for the cockpit tachometer display. Set() accepts the RPMS in the rpms
// parameter. The Constructor takes the screen origin of the gauge, and the
// needle color.

class RpmGauge : public DialInstrument
	{
	public:
	RpmGauge(int origX, int origY, byte nColor);
	void Set(int rpms);
	};

// Class SlipGauge. Not currently derived from anything, but we may yet get a
// virtual base class that encompasses all the instruments. Control class for
// the cockpit slip indicator display. Constructor takes an x and y screen
// origin for the gauge. Set() takes a deflection value where 0 places the ball
// at rest in the center, -9 is full deflection to the left, and 9 is full def-
// lection to the right.

class SlipGauge
	{
	public:                          // PUBLICS
	SlipGauge(int origX, int origY);
	void Set(int deflection);
	gaugeStat Status() {return (stat);};

	private:                         // PRIVATES
	byte ballFrame[40];              // storage for the ball sprite
	int scrOrigX;                    // x,y coords where the corner of the...
	int scrOrigY;                    // save rectangle is displayed
	gaugeStat stat;                  // current gauge status
	};

// class Compass. Class for control of the compass instrument.

class Compass
	{
	public:
	Compass(int origX, int origY);
	~Compass();
	void Set(int heading);
	gaugeStat Status() {return(stat);};

	private:
	byte *cStrip;                    // storage for the compass strip
	int scrOrigX;                    // x,y coords where the corner of the...
	int scrOrigY;                    // save rectangle is displayed
	int framWid;                     // width of the heading window
	int framDep;                     // depth of the heading window
	int stripWid;                    // width of the compass strip
	int stripDep;                    // depth of the compass strip
    byte lastDir;                    // last valid heading
	gaugeStat stat;                  // current gauge status

	void near BlitBox(word centerX); // used to display the heading
};

// class IgnitionSwitch, for control of the ignition switch and indicator
// light

class IgnitionSwitch
	{
	public:
	IgnitionSwitch();
	~IgnitionSwitch();
	void Set(int onOff);
	gaugeStat Status() {return( stat );};

	private:
	gaugeStat stat;
	byte *switch_on_map;
	byte *light_on_map;
	};

#endif













