// EVNTMGR2.CPP
//
// A set of high-level functions to monitor input from
// the keyboard, the joystick and the mouse.
//
// Copyright 1993 by Christopher Lampton and The
// Waite Group Press
//
//  8/21/95 	mracky@sirius.com  encapsulate into C++ class
//  10/13/95    mracky

#ifndef EVNTMGR2_H
#define EVNTMGR2_H

const int NOEVENT = 0, LBUTTON = 1, RBUTTON = 2, UP = 4, DOWN = 8, LEFT = 16, RIGHT = 32;
const int MOUSE_EVENTS = 1, JOYSTICK_EVENTS = 2, KEYBOARD_EVENTS = 4;

const int JOY_X = 1, JOY_Y = 2;
const int JBUTTON1 = 0x10;
const int JBUTTON2 = 0x20;
const int MBUTTON1 = 1;
const int MBUTTON2 = 2;

// scan key codes
const int ESCAPE = 1;
const int TABKEY = 0x0f;
const int KEY_Y = 0x15;  // 'y'
const int KEY_I = 0x17;  // 'i' ignition toggle
const int ENTER = 0x1c;
const int KEY_S = 0x1f;  // 's' sound toggle
const int KEY_B = 0x30;  // 'b' brake
const int KEY_N = 0x31;  // 'n'
const int KEY_LT = 0x33; // '<' rudder left
const int KEY_GT = 0x34; // '>' rudder right
const int KEY_F1 = 0x3b; //     front view
const int KEY_F2 = 0x3c; //     left side view
const int KEY_F3 = 0x3d; //     right side view 
const int KEY_F4 = 0x3e; //     rear view
const int UP_ARROW = 0x48;
const int PAD_MINUS = 0x4a;
const int LEFT_ARROW = 0x4b;
const int RIGHT_ARROW = 0x4d;
const int PAD_PLUS = 0x4e;
const int DOWN_ARROW = 0x50;

class EventManager
{
protected:
	bool hasMouse;
	int xmin, xmax, xcent, ymin, ymax, ycent; // Joystick calibration variables

	bool initmouse()
	{
		return 0;
	}

	void rempointer()
	{
	}

	void init_events()
	{
		hasMouse = initmouse();	// Mouse init returns 0 on failure
		rempointer();			// Remove mouse pointer from screen
	}

public:
	EventManager()
	{
		init_events();
	}

	bool readjbutton(int button)
	{
		return false;
	}

	int readstick(int stick)
	{
		return 0;
	}

	// Set minimum joystick coordinates
	void setmin()
	{
		while (!readjbutton(JBUTTON1)); // Loop until joystick button pressed
		xmin = readstick(JOY_X);          // Get x coordinate
		ymin = readstick(JOY_Y);          // Get y coordinate
		while (readjbutton(JBUTTON1));  // Loop until button released
	}

	// Set maximum joystick coordinates
	void setmax()
	{
		while (!readjbutton(JBUTTON1)); // Loop until joystick button pressed
		xmax = readstick(JOY_X);          // Get x coordinate
		ymax = readstick(JOY_Y);          // Get y coordinate
		while (readjbutton(JBUTTON1));  // Loop until button released
	}

	// Set center joystick coordinates
	void setcenter()
	{
		while (!readjbutton(JBUTTON1)); // Loop until joystick button pressed
		xcent = readstick(JOY_X);         // Get x coordinate
		ycent = readstick(JOY_Y);         // Get y coordinate
		while (readjbutton(JBUTTON1));  // Loop until button released
	}

#ifdef QUAINT
	// Get events from devices selected by EVENT_MASK
	int getevent(int event_mask)
	{
		int event_return = NOEVENT;	// Initialize events to NO EVENTS
		// If joystick events requested....
		if (event_mask & JOYSTICK_EVENTS) {
			// ...set left, right, up, down and button events:
			if (readstick(JOY_X) < (xcent - 4)) event_return |= LEFT;
			if (readstick(JOY_X) > (xcent + 10)) event_return |= RIGHT;
			if (readstick(JOY_Y) < (xcent - 4)) event_return |= UP;
			if (readstick(JOY_Y) > (xcent + 10)) event_return |= DOWN;
			if (readjbutton(JBUTTON1)) event_return |= LBUTTON;
			if (readjbutton(JBUTTON2)) event_return |= RBUTTON;
		}

		// If mouse events requested....
		if (event_mask & MOUSE_EVENTS) {
			// ...set left, right, up, down and button events:
			relpos(&x, &y);	// Read relative mouse position
			if (x < 0) event_return |= LEFT;
			if (x > 0) event_return |= RIGHT;
			if (y < 0) event_return |= UP;
			if (y > 0) event_return |= DOWN;
			int b = readmbutton();	// Read mouse button
			if (b & MBUTTON1) event_return |= LBUTTON;
			if (b & MBUTTON2) event_return |= RBUTTON;
		}

		// If keyboard events requested
		if (event_mask & KEYBOARD_EVENTS) {
			// ...set left, right, up, down and "button" events:
			int k = scankey();  // Read scan code of last key pressed
			if (k == 0) {       // If no key pressed
				if (lastkey) {  // Set to last active key
					k = lastkey;
					--keycount;   // Check repeat count
					if (keycount == 0) lastkey = 0;	// If over, deactivate
					//   key
				}
			} else {	// If key pressed...
				lastkey = k;	// ...note which key
				keycount = 20;	// ...set repeat count
			}
			// ...and determine which key event, if any, occurred:
			switch (k) {
			case ENTER: event_return |= LBUTTON; break;
			case TABKEY: event_return |= RBUTTON; break;
			case UP_ARROW: event_return |= UP; break;
			case DOWN_ARROW: event_return |= DOWN; break;
			case LEFT_ARROW: event_return |= LEFT; break;
			case RIGHT_ARROW: event_return |= RIGHT; break;
			}
		}
		return(event_return);
	}
#endif
};

#endif
