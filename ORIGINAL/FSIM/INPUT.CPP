//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: INPUT.CPP
// Project:   Flights of Fantasy
// Creation:  March 10, 1992
// Author:    Mark Betz (MB)
// Change History
// ------ -------
//
//      Date            Rev.	Author			Purpose
//      ----            ----    ------      	-------
//      1-2-1992        1.0     MB          	Initial development
//      8-29-1992       1.1b    MB          	First beta
//      9-26-1992       1.1     MB          	Publication release, some minor
//                                          	changes to layout only
//      9-25-1995       2.0   mracky@sirius.com update to C++; integrate with
//                                                 eventmanager class
//      10-20-95        2.1     mracky          tweaks
//
// Description
// -----------
//      Reads keyboard and joystick, tracks input control states, calculates
//      control surface deflection indexes.
//-------+---------+---------+---------+---------+---------+---------+---------+
//#include <stdio.h>
#include <dos.h>
#include <iostream.h>
#include <conio.h>
#include "types.h"                  // some generic types
#include "aircraft.h"               // state vector definition
#include "htimer.h"                 // hi-res timer class
#include "input.h" 					// header file for this module
#include "fcontrol.h"

static HTimer* timer1;
static void interrupt (*Old09Handler)(...);   // pointer to old kbd isr

// pointers to system keyboard ring buffer head/tail
static word* nextBiosChar = (word*)MK_FP(0x0040, 0x001A);
static word* lastBiosChar = (word*)MK_FP(0x0040, 0x001C);

// offset of system keyboard ring buffer start
static word biosBufOfs = 0x001E;

static const kbd_data = 0x60;            // keyboard data port
static const kbd_stat = 0x64;            // keyboard status port
static const kbd_output_full = 1;        // kbd output full mask
static const int gamePort = 0x201;       // joystick port

// control key states - set to true when pressed, set to false when read
// by application

static boolean keypressed = false;       // true if any key pressed
static boolean _keydown[130];           // scancodes
// control sensitivity in amount of motion per 1/20 second

static byte activeAxisMask;               // mask for live jstick axes

// Joystick control variables

//static boolean usingStick = false;       // true if joystick in use
//static int stickX;                       // current stick x and y position
//static int stickY;

void interrupt New09Handler(...);        // forward reference

// returns true if scan code data is waiting at the keyboard port

boolean KeyWaiting()
	{
	byte port;
	port = inportb(kbd_stat);             // get the kbd_controller status
	if (port & kbd_output_full)           // evals to true if buffer full
		return(true);
	else
		return(false);
	}


// returns true if a key has been pressed _since the last call to this
// function_. The boolean variable keypressed is set in New09Handler()

boolean KeyPressed()
	 {
    boolean result;
    result = keypressed;
    keypressed = false;
	 return( result );
    }

// this function should be called in *TEXT* mode. It asks the user if they
// wish to use the joystick, and returns true if the answer is yes

boolean FControlManager::UseStick()
{
	return YesNo(
	 "=> A joystick has been detected... do you wish to use it? (Yy/Nn)");
}

boolean FControlManager::UseMouse()
{

	return YesNo(
		"=> A mouse has been detected... do you wish to use it? (Yy/Nn)");

}

boolean FControlManager::YesNo(char *st)
{
	cout << st << endl;
	char ch = 0;
	while (ch == 0)
	{
		ch = getch();
		if ((ch == 0x59) || (ch == 0x79))
			return(true);
		else if ((ch == 0x4e) || (ch == 0x6e))
			break;
		else
			ch = 0;
	}
	return false;
}

// This function should be called with VGA in text mode! It goes through a
// joystick calibration process. This could be ammended to write a file
// to disk, which could be used to define the joystick limits when the
// program starts up.

void FControlManager::Calibrate()
{
	word tempX, tempY;

	clrscr();
	cout << "-----------------  Calibrating Joystick  -----------------" << endl;
	cout << "\nMove joystick to upper left corner, then press a button..." << endl;
	while( ! EitherButton()) {
		xmin = ReadStickPosit(JOY_X);
		ymin = ReadStickPosit(JOY_Y);
		}
	delay(50);
	while( EitherButton() );
	cout << "\nMove joystick to lower right corner, then press a button.."<< endl;
	while( ! EitherButton()) {
		xmax = ReadStickPosit(JOY_X);
		ymax = ReadStickPosit(JOY_Y);
		}
	delay(50);
	while( EitherButton() );
	cout << "\nCenter the joystick, then press a button..." << endl;
	while( ! EitherButton()) {
		tempX = ReadStickPosit(JOY_X);
		tempY = ReadStickPosit(JOY_Y);
		}
	tempX = tempX * (25500 / (xmax - xmin));
	xcent = tempX/100;
	tempY = tempY * (25500 / (ymax - ymin));
	ycent = tempY/100;
	cout << "\n...calibration complete." << endl;
}

// This function is called at program startup to initialize the control
// input system. Call this function BEFORE putting video system into
// graphics mode, or you'll get a very ugly screen

int FControlManager::InitControls()
{
	 if (( timer1 = new HTimer() ) != NULL ) {
		if (activeAxisMask != 0) {  // is there at least 1 active joystick?
			if (UseStick()) {       // yes, do they want to use it?
				Calibrate();        // yes, go calibrate it
				usingStick = true;    // set the stick-in-use flag
				}                     // no, don't use the stick
			}                         // no, there is no joystick
		if((usingStick==false) && hasMouse) {        // if mouse driver is present
			while(AnyPress());
			if( UseMouse())
			{
				stick_sens = 4; 	// REV 2.1 tweak mouse performance
				usingMouse = true;
			}
		}
		Old09Handler = getvect(9);    // hook keyboard hardware interrupt vector
		setvect(9, New09Handler);
		return( 1 );
	}
	return 0;
}

//++++++++++++++++++++++++++++++++++
// FCONTROLMANAGER CLASS
//  CONSTRUCTOR
//
FControlManager::FControlManager():EventManager()
{
	usingMouse =
	usingStick = false;
	stick_sens = 15;            // arbitrary value for control sensitivity
	rudder_sens = 15;
	activeAxisMask = joydetect();
		// REV 2.1 moved _keydown initialization to reset controls
	ResetControls();
}

void FControlManager::ResetControls()
{
	for(int ii = 0; ii < 128; _keydown[ii++] = false) ;
	keypressed = 0;
}
// Call this function on program termination to do a graceful shutdown of
// the control system. Fail to call it, and a LOCKED MACHINE is a certainty!

FControlManager::~FControlManager()
{
	if(Old09Handler != NULL)
		setvect(9, Old09Handler);
	 if (timer1 != NULL)
		 delete timer1;
}

// Call this function to find out if the user has input an exit command since
// the last call to this function. The only exit command currently defined is
// the escape key. See New09Handler() below

boolean FControlManager::Exit()
	{
	boolean exit_state;

	exit_state = _keydown[ESCAPE];
	_keydown[ESCAPE] = false;
	return( exit_state );
	}

// grabs scan codes and updates the boolean state data above. Note that the
// function bails out as soon as a valid keycode is detected. The boolean
// state variables are updated on press of each relevant key.
// Note: the command flags stay true until read by GetControls()

void interrupt New09Handler(...)
{
	byte code;
	if (KeyWaiting())       {                  // make sure something in port
		code = inportb(kbd_data);              // grab the scan code
		keypressed = true;                     // returned by KeyPressed()
		if( code < 128 )
			_keydown[code] = true;
		else   								 // REV 2.1 tweak
			_keydown[code-128] = false;		 //         keyboard performance
	}
	Old09Handler();                          // chain the old interrupt
	*nextBiosChar = biosBufOfs;              // make sure the bios keyboard
	*lastBiosChar = biosBufOfs;              // buffer stays empty
}


// This function reads the joystick port and calculates the stick axes
// position.
byte FControlManager::ReadStickPosit(int axisNum)
{
	dword elapsedTime;
	word bit16Time;

	asm {
		mov ax, axisNum			 // load the axisnum
		xor ah, ah               // and then push it while timer turned on
		push ax
		cli                      // disable ints for accurate timing
		}
	timer1->timerOn();           // start the timer
	asm {
		pop bx					 // this is the axisnum mask

		// read the value of the selected axis by nudging the game port,
		// which sets the four lower bits of the status byte to 1. Exit
		// as soon as the bit for the selected axis resets

		xor ax, ax               // out anything to port 0x201 and the
		mov dx, gamePort         // axis bits (low nibble) get set to 1
		out dx, al               // time to reset indicates position
		xor cx, cx               // we'll time out after 65535 loops
		}
	readloop1:
	asm {
		in al, dx                // get the byte from the game port
		test al, bl              // test it against the axis mask
		loopne readloop1         // if it's still set loop
		xor ax, ax               // clear ax in case this was a time_out
		jcxz done                // did we time-out? if yes exit, ax == 0
		}
	elapsedTime = timer1->timerOff();
	bit16Time = ( word )elapsedTime;
	asm {
		sti
		// now read the joystick port repeatedly until the other 3 axis
		// timers run down. This is MANDATORY.
		mov dx, gamePort
		xor cx, cx
		mov bl, activeAxisMask   // mask for all active axes
		}
	readloop2:
	asm {
		in al, dx                // same basic operation as in
		test al, bl              // readloop1
		loopne readloop2

		mov bx, bit16Time;       // get elapsed time into bx
		mov cl, 4                // style x,y values, i.e. 0-255
		shr bx, cl
		mov ax, bx               // final result in AX
		xor ah, ah               // don't need the high byte
		}
	done:
	return(_AL);
}

// This function is called from GetControls(). It checks the state of all
// the flight control keys, and adjusts the values which would normally be
// mapped to the joystick if it was in use. GetControls() only calls this
// function if the joystick is not being used.

void FControlManager::CalcKeyControls(AirCraft& AC)
{
	if (_keydown[LEFT_ARROW]) {
		// adjust the control position
		stickX += stick_sens;
		if (stickX > 127)
			stickX = 127;
		/* _keydown[LEFT_ARROW] = false;*/ // REV 2.1 keyboard tweak
		}
	else if (_keydown[RIGHT_ARROW]) {
		stickX -= stick_sens;
		if (stickX < -128)
			stickX = -128;
		  /* _keydown[RIGHT_ARROW] = false;*/ // REV 2.1 keyboard tweak
        }
	else 
		stickX = 0;
	if (_keydown[UP_ARROW]) {
		stickY += stick_sens;
		if (stickY > 127)
			stickY = 127;
		/* _keydown[UP_ARROW] = false;*/   // REV 2.1 keyboard tweak
        }
	else if (_keydown[DOWN_ARROW]) {
		stickY -= stick_sens;
		if (stickY < -128)
			stickY = -128;
		/* _keydown[DOWN_ARROW] = false;*/ // REV 2.1 keyboard tweak
		  }
	else
		stickY = 0;
	if( _keydown[ENTER] == true) {
		AC.button1 = true;
		/* _keydown[ENTER] = false;*/  // REV 2.1 keyboard tweak
		}
	if( _keydown[TABKEY] == true) {
		AC.button2 = true;
		/* _keydown[TABKEY] = false;*/ // REV 2.1 keyboard tweak
		}

}

// this function is called from GetControls(). It calculates the state of
// the flight controls that are mapped to the joystick when it is in use.
// This function is only called when the joystick is in use.

void FControlManager::CalcStickControls( AirCraft& AC )
{
    word tempX, tempY;

	tempX = ReadStickPosit(JOY_X);           // get current x and y axes
	tempY = ReadStickPosit(JOY_Y);           // position

	 // map the ideal x,y axis range to reality, then assign the result
    // to stickX and stickY.
	tempX = tempX * (25500 / (xmax - xmin));
	stickX = tempX/100;
	tempY = tempY * (25500 / (ymax - ymin));
	stickY = tempY/100;

	if (stickX > xcent)                  // map X 0..255 range to
		stickX = (stickX - xcent);       // -128..127 range
	else
		stickX = (-(xcent - stickX));

	if (stickY > ycent)                  // map Y 0..255 range to
		stickY = (stickY - ycent);       // -128..127 range
	else
		stickY = (-(ycent - stickY));

	stickX = (-stickX);                      // flip the values so that
	stickY = (-stickY);                      // the control directions
                                             // make sense

	if (stickX > 127) stickX = 127;          // bounds check the results
	else if (stickX < -128) stickX = -128;
	if (stickY > 127) stickY = 127;
	else if (stickY < -128) stickY = -128;

	if (Button1())                           // get the buttons states
		AC.button1 = true;                 // and update the state vector
	if (Button2())
		AC.button2 = true;
}


void FControlManager::CalcMouseControls( AirCraft& AC )
{
	int tempX, tempY;
#ifdef NOTUSED
	relpos(&tempX, &tempY);  	// get current x and y axes
								// position
	tempX /= 2; 				//adjust x position to 320x200
	if( tempX > 0)
		stickX += stick_sens;			   
	else if( tempX < 0)
		stickX -= stick_sens;
	else if( stickX != 0)
		stickX /= 2;

	tempY = -tempY; 		// flip mouse coordinate to match 
							// joystick emulation
	if( tempY > 0)
		stickY += stick_sens;
	else if( tempY < 0)
		stickY -=stick_sens;
	else if( stickY != 0)
		stickY /= 2;
#else							// REV 2.1 mouse status

#define MSE_XCENT 320
#define MSE_YCENT 100	
	mousepos(&tempX, &tempY);  	// get current x and y axes
	tempX -= MSE_XCENT;			// convert to -320 to 320
	tempY -= MSE_YCENT;         // convert to -100 to 100

	stickX = -((tempX * 2)/5);  // ratio mouse x coords to stick
	stickY = (tempY * 32)/25;  	// ratio mouse y coords to stick
#endif
	if (stickX > 127)
		stickX = 127;          // bounds check the results
	else if (stickX < -128)
		stickX = -128;
	if (stickY > 127) 
		stickY = 127;
	else if (stickY < -128)
		stickY = -128;
	int mouseButtons = readmbutton();
	if (mouseButtons& MBUTTON1)                           // get the buttons states
		AC.button1 = true;                 // and update the state vector
	if (mouseButtons & MBUTTON2)
		AC.button2 = true;
}

// this function is called from GetControls(). It calculates the state of
// the standard controls which are never mapped to the joystick. This
// function is called on every call to GetControls()

void FControlManager::CalcStndControls( AirCraft& AC )
{
	if (_keydown[KEY_GT]) {
		rudderPos += rudder_sens;
		if (rudderPos > 127)
			rudderPos = 127;
		/* _keydown[KEY_GT] = false;*/ // REV 2.1 keyboard tweak
		  }
	else if (_keydown[KEY_LT]) {
		rudderPos -= rudder_sens;
		if (rudderPos < -128)
			rudderPos = -128;
		/* _keydown[KEY_LT] = false;*/ // REV 2.1 keyboard tweak
		}
	else
		  rudderPos = 0;
	if ((_keydown[PAD_PLUS]) && (AC.throttle_pos < 15)) {
		AC.throttle_pos++;
		_keydown[PAD_PLUS] = false;
		}
	else if ((_keydown[PAD_MINUS]) && (AC.throttle_pos > 0)) {
		AC.throttle_pos--;
		_keydown[PAD_MINUS] = false; 
		}
	if (_keydown[KEY_I]) {
		if (AC.ignition_on)
			AC.ignition_on = false;
		else
			AC.ignition_on = true;
		_keydown[KEY_I] = false; 
		}
	if (_keydown[KEY_B]) {
		if (AC.brake)
			AC.brake = false;
		else
			AC.brake = true;
		_keydown[KEY_B] = false; 
		}
	 }

// this function is called from GetControls(). It checks the state of the
// sound and view controls, and updates the state vector if required.
// This function is called on every call to GetControls().

void FControlManager::CheckSndViewControls( AirCraft& AC )
{
	 if (_keydown[KEY_S]) {
		 _keydown[KEY_S] = false;
		 AC.sound_chng = true;
	 }
    if (_keydown[KEY_F1]) {
       _keydown[KEY_F1] = false;
       AC.view_state = 0;
       }
    else if (_keydown[KEY_F2]) {
       _keydown[KEY_F2] = false;
       AC.view_state = 1;
		 }
	 else if (_keydown[KEY_F3])	{
		 _keydown[KEY_F3] = false;
       AC.view_state = 2;
		 }
    else if (_keydown[KEY_F4]){
       _keydown[KEY_F4] = false;
       AC.view_state = 3;
	   }
}


// This function is called from the main program, and is passed a pointer
// to the aircraft state vector. It updates this vector based on the current
// state of the control interface.

void FControlManager::GetControls( AirCraft& AC )
{
	AC.button1 = false;                // reset both button flags
	AC.button2 = false;
	if (usingStick)                          // is the joystick in use?
        CalcStickControls( AC );            // yes, grab the control state
	else if (usingMouse)
		CalcMouseControls( AC );
    else
		CalcKeyControls(AC);                   // no, get the keyboard controls
	 CalcStndControls( AC );                 // go get the standard controls

	 CheckSndViewControls( AC );
	AC.aileron_pos = stickX;               // update the state vector
	AC.elevator_pos = stickY;              // with values calculated in
    AC.rudder_pos = rudderPos;             // the other functions
	 AC.ReduceIndices( );                    // remap the deflections
}




boolean FControlManager::AnyPress()
{
	if( EitherButton() || keypressed)
		return true;
	else if( hasMouse) {
		int mouseButtons = readmbutton();
		if(mouseButtons & (MBUTTON1 | MBUTTON2))
			return true;
	}
	return false;
}


