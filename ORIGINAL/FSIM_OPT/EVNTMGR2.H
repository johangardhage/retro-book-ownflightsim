#ifndef EVNTMGR2_H

#define EVNTMGR2_H

const int NOEVENT=0,LBUTTON=1,RBUTTON=2,UP=4,DOWN=8,LEFT=16,RIGHT=32;
const int MOUSE_EVENTS=1,JOYSTICK_EVENTS=2,KEYBOARD_EVENTS=4;
// scan key codes
const int ESCAPE=1;
const int TABKEY=0x0f;
const int KEY_Y=0x15;  // 'y'
const int KEY_I=0x17;  // 'i' ignition toggle
const int ENTER=0x1c;
const int KEY_S=0x1f;  // 's' sound toggle
const int KEY_B=0x30;  // 'b' brake
const int KEY_N=0x31;  // 'n'
const int KEY_LT=0x33; // '<' rudder left
const int KEY_GT=0x34; // '>' rudder right
const int KEY_F1=0x3b; //     front view
const int KEY_F2=0x3c; //     left side view
const int KEY_F3=0x3d; //     right side view 
const int KEY_F4=0x3e; //     rear view
const int UP_ARROW=0x48;
const int PAD_MINUS=0x4a;
const int LEFT_ARROW=0x4b;
const int RIGHT_ARROW=0x4d;
const int PAD_PLUS=0x4e;
const int DOWN_ARROW=0x50;

#include "types.h"

class EventManager
{
protected:
	boolean hasMouse;
	// int x,y;	// All purpose coordinate variables
	int xmin,xmax,xcent,ymin,ymax,ycent; // Joystick cali-
									 // bration variables
	//int lastkey,keycount;	// Keyboard variables

	void init_events();	// Initialize event manager
public:
	EventManager();

	void setmin();			// Set minimum joystick callibrations
	void setmax();			// Set maximum joystick callibrations
	void setcenter();		// Set center joystick callibrations

	//int getevent(int);	// Get events from selected devices
};

#endif
