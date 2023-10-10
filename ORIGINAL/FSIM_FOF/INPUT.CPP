//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: INPUT.CPP
// Project:   Flights of Fantasy
// Creation:  March 10, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC's and Compatibles
//
// Includes:  dos.h, stdio.h, conio.h, types.h, input.h, aircraft.h, htimer.h
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      1-2-1992        1.0     MB          Initial development
//      8-29-1992       1.1b    MB          First beta
//      9-26-1992       1.1     MB          Publication release, some minor
//                                          changes to layout only
//
// Description
// -----------
//      Reads keyboard and joystick, tracks input control states, calculates
//      control surface deflection indexes.
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <dos.h>
#include <stdio.h>
#include <conio.h>

#ifndef __TYPES__
	#include "types.h"                   // some generic types
#endif
#ifndef __AIRCRAFT__
    #include "aircraft.h"                // state vector definition
#endif
#ifndef __HTIMER__
    #include "htimer.h"                  // hi-res timer class
#endif
#ifndef __INPUT__
	#include "input.h"                   // header file for this module
#endif

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
static boolean esc_pressed = false;      // true if escape pressed
static boolean btn1_pressed = false;     // true if button 1 pressed
static boolean btn2_pressed = false;     // true if button 2 pressed
static boolean aileron_left = false;     // true if left arrow pressed
static boolean aileron_right = false;    // true if right arrow pressed
static boolean elevator_up = false;      // true if down arrow pressed
static boolean elevator_down = false;    // true if up arrow pressed
static boolean rudder_right = false;     // true if '>' pressed
static boolean rudder_left = false;      // true if '<' pressed
static boolean throttle_up = false;      // true if plus key pressed
static boolean throttle_down = false;    // true if minus key pressed
static boolean ignition_change = false;  // true if 'I' (i) pressed
static boolean sound_toggle = false;     // true if sound on/off state chngd
static boolean view_forward = true;      // true if forward view key pressed
static boolean view_right = false;       // true if right view key pressed
static boolean view_left = false;        // true if left view key pressed
static boolean view_rear = false;        // true if rear view key pressed
static boolean brake_tog = false;        // true if brake key pressed

// control sensitivity in amount of motion per 1/20 second

static byte stick_sens;
static byte rudder_sens;
static int rudderPos;                     // current position of rudder
static byte activeAxisMask;               // mask for live jstick axes

// Joystick control variables

static boolean usingStick = false;       // true if joystick in use
static int stickX;                       // current stick x and y position
static int stickY;
static byte stickMaxX;                   // joystick limits, from the
static byte stickMinX;                   // calibrate function
static byte stickCtrX;
static byte stickMaxY;
static byte stickMinY;
static byte stickCtrY;

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

// returns true if joystick button 1 currently pressed

boolean Button1()
	{
	byte port;

	port = inportb(gamePort);             // grab joystick status
	if (port & 0x10)                      // test button 1 bit
		return(false);
	else
		return(true);
	}

// returns true if joystick button 2 currently pressed

boolean Button2()
	{
	byte port;

	port = inportb(gamePort);             // grab joystick status byte
	if (port & 0x20)                      // test button 2 bit
		return(false);
	else
		return(true);
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

boolean UseStick()
	{
	char ch = 0;

	printf
      ("=> A joystick has been detected... do you wish to use it? (Yy/Nn)\n");
	while (ch == 0)
		{
		ch = getch();
		if ((ch == 0x59) || (ch == 0x79))
			return(true);
		else if ((ch == 0x4e) || (ch == 0x6e))
			return(false);
		else ch = 0;
		}
	}

//-------+---------+---------+---------+---------+---------+---------+---------+
// This function is called once when the joystick system is initialized. It
// tests all four possible axes and returns the number of sticks found. The
// axes are tested by sending a 0 byte to the game port, which sets the axis
// bits to 1. The port is then polled in a loop. If the loop times out before
// the bit resets there is no pot on the other end, or their is a hardware
// error. If no active axis is found the stick is assumed not to exist. It
// also checks the PC model number to determine whether BIOS support exists.
//-------+---------+---------+---------+---------+---------+---------+---------+

byte TestSticks()
	{
	byte result = 1;                   // start assuming one stick
	word i;

	activeAxisMask = 0;

	// test stick 1
	outportb(gamePort, 0);
	for(i = 0; i < 65535; i++)
		if((inportb(gamePort) & 1) == 0)
			break;
	if(i != 65535)
		activeAxisMask = (activeAxisMask | 1);

	outportb(gamePort, 0);
	for(i = 0; i < 65535; i++)
		if((inportb(gamePort) & 2) == 0)
			break;
	if(i != 65535)
		activeAxisMask = (activeAxisMask | 2);

	if(!(activeAxisMask & 3))
		result--;

	return(result);
	}

// This function should be called with VGA in text mode! It goes through a
// joystick calibration process. This could be ammended to write a file
// to disk, which could be used to define the joystick limits when the
// program starts up.

void CalibrateStick()
	{
	word tempX, tempY;

	clrscr();
	printf("-----------------  Calibrating Joystick  -----------------\n");
	printf("\n");
	printf("Move joystick to upper left corner, then press a button...\n");
	while( (!Button1()) && (!Button2()) )
		{
		stickMinX = ReadStickPosit(0);
		stickMinY = ReadStickPosit(1);
		}
	delay(50);
	while( Button1() || Button2() );
	printf("\n");
	printf("Move joystick to lower right corner, then press a button..\n");
	while( (!Button1()) && (!Button2()) )
		{
		stickMaxX = ReadStickPosit(0);
		stickMaxY = ReadStickPosit(1);
		}
	delay(50);
	while(Button1() || Button2());
	printf("\n");
	printf("Center the joystick, then press a button...\n");
	while( (!Button1()) && (!Button2()) )
		{
		tempX = ReadStickPosit(0);
		tempY = ReadStickPosit(1);
		}
	tempX = tempX * (25500 / (stickMaxX - stickMinX));
	stickCtrX = tempX/100;
	tempY = tempY * (25500 / (stickMaxY - stickMinY));
	stickCtrY = tempY/100;
	printf("\n");
	printf("...calibration complete.\n");
	return;
	}

// This function is called at program startup to initialize the control
// input system. Call this function BEFORE putting video system into
// graphics mode, or you'll get a very ugly screen

byte InitControls()
	{
    byte result = 1;

	stick_sens = 15;               // arbitrary value for control sensitivity
	rudder_sens = 15;
    if (( timer1 = new HTimer() ) != NULL )
        {
	    if (TestSticks())             // is there at least 1 active joystick?
		    {
            if (UseStick())           // yes, do they want to use it?
			    {
			    CalibrateStick();     // yes, go calibrate it
			    usingStick = true;    // set the stick-in-use flag
			    }                     // no, don't use the stick
		    }                         // no, there is no joystick
	    Old09Handler = getvect(9);    // hook keyboard hardware interrupt vector
	    setvect(9, New09Handler);
        }
    else
        result = 0;
	return( result );
	}

// Call this function on program termination to do a graceful shutdown of
// the control system. Fail to call it, and a LOCKED MACHINE is a certainty!

byte ControlShutDown()
	{
	setvect(9, Old09Handler);
    if (timer1 != NULL)
       delete timer1;
	return(1);
	}

// Call this function to find out if the user has input an exit command since
// the last call to this function. The only exit command currently defined is
// the escape key. See New09Handler() below

boolean Exit()
	{
	boolean exit_state;

	exit_state = esc_pressed;
	esc_pressed = false;
	return( exit_state );
	}

// grabs scan codes and updates the boolean state data above. Note that the
// function bails out as soon as a valid keycode is detected. The boolean
// state variables are updated on press of each relevant key.
// Note: the command flags stay true until read by GetControls()

void interrupt New09Handler(...)
	{
	byte code;
    if (KeyWaiting())                         // make sure something in port
       {
       code = inportb(kbd_data);              // grab the scan code
       keypressed = true;                     // returned by KeyPressed()
	   if (code == 0x4d)
		   aileron_right = true;              // test for valid key, and
	   else if (code == 0x4b)                 // set pressed flag if found
		   aileron_left = true;
	   else if (code == 0x34)
		   rudder_right = true;
	   else if (code == 0x33)
		   rudder_left = true;
	   else if (code == 0x48)
		   elevator_up = true;
	   else if (code == 0x50)
		   elevator_down = true;
	   else if (code == 0x4e)
		   throttle_up = true;
	   else if (code == 0x4a)
		   throttle_down = true;
	   else if (code == 0x1)
		   esc_pressed = true;
	   else if (code == 0x17)
		   ignition_change = true;
       else if (code == 0x1f)
           sound_toggle = true;
       else if (code == 0x3b)
           view_forward = true;
       else if (code == 0x3c)
           view_right = true;
       else if (code == 0x3d)
           view_rear = true;
       else if (code == 0x3e)
           view_left = true;
       else if (code == 0x30)
           brake_tog = true;

       }
    Old09Handler();                          // chain the old interrupt
    *nextBiosChar = biosBufOfs;              // make sure the bios keyboard
    *lastBiosChar = biosBufOfs;              // buffer stays empty
	}

// This function reads the joystick port and calculates the stick axes
// position. It's basically the same as the IBM bios version, which means
// it could probably be vastly improved.

byte ReadStickPosit(byte axisNum)
	{
    dword elapsedTime;
    word bit16Time;

	asm {
		push bx                  // save the registers we modify
		push cx
		push dx

		// create a mask for the proper axis by setting bit 1 of al and
		// shifting it left by the number of the axis

		mov al, 1                // calculate the axis mask for...
		mov cl, axisNum          // reading the game port and put...
		shl al, cl               // it in bl
		mov bl, al

        push bx
		cli                      // disable ints for accurate timing
        }
    timer1->timerOn();           // start the timer
    asm {
        pop bx
		// read the value of the selected axis by nudging the game port,
		// which sets the four lower bits of the status byte to 1. Exit
		// as soon as the bit for the selected axis resets

		xor ax, ax               // out anything to port 0x201 and the
		mov dx, gamePort         // axis bits (low nibble) get set to 1
		out dx, al               // time to reset indicates position
		mov cx, 65535            // we'll time out after 65535 loops
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
		mov cx, 65535
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
	asm {
		pop dx                   // restore registers
		pop cx
		pop bx
		}
	return(_AL);
	}

// This function is called from GetControls(). It checks the state of all
// the flight control keys, and adjusts the values which would normally be
// mapped to the joystick if it was in use. GetControls() only calls this
// function if the joystick is not being used.

void CalcKeyControls()
	{
	if (aileron_left)
		{
        // adjust the control position
    	stickX += stick_sens;
		if (stickX > 127)
			stickX = 127;
		aileron_left = false;
		}
	else if (aileron_right)
		{
		stickX -= stick_sens;
		if (stickX < -128)
			stickX = -128;
        aileron_right = false;
        }
    else
        stickX = 0;
	if (elevator_up)
		{
		stickY += stick_sens;
		if (stickY > 127)
			stickY = 127;
		elevator_up = false;
        }
	else if (elevator_down)
		{
		stickY -= stick_sens;
		if (stickY < -128)
			stickY = -128;
		elevator_down = false;
        }
	else
        stickY = 0;
	}

// this function is called from GetControls(). It calculates the state of
// the flight controls that are mapped to the joystick when it is in use.
// This function is only called when the joystick is in use.

void CalcStickControls( state_vect& tSV )
    {
    word tempX, tempY;

	tSV.button1 = false;                // reset both button flags
	tSV.button2 = false;
	tempX = ReadStickPosit(0);           // get current x and y axes
	tempY = ReadStickPosit(1);           // position

    // map the ideal x,y axis range to reality, then assign the result
    // to stickX and stickY.
	tempX = tempX * (25500 / (stickMaxX - stickMinX));
	stickX = tempX/100;
	tempY = tempY * (25500 / (stickMaxY - stickMinY));
	stickY = tempY/100;

	if (stickX > stickCtrX)                  // map X 0..255 range to
		stickX = (stickX - stickCtrX);       // -128..127 range
	else
		stickX = (-(stickCtrX - stickX));

	if (stickY > stickCtrY)                  // map Y 0..255 range to
		stickY = (stickY - stickCtrY);       // -128..127 range
	else
		stickY = (-(stickCtrY - stickY));

	stickX = (-stickX);                      // flip the values so that
	stickY = (-stickY);                      // the control directions
                                             // make sense

	if (stickX > 127) stickX = 127;          // bounds check the results
	else if (stickX < -128) stickX = -128;
	if (stickY > 127) stickY = 127;
	else if (stickY < -128) stickY = -128;

	if (Button1())                           // get the buttons states
		tSV.button1 = true;                 // and update the state vector
	if (Button2())
		tSV.button2 = true;
    }

// this function is called from GetControls(). It calculates the state of
// the standard controls which are never mapped to the joystick. This
// function is called on every call to GetControls()

void CalcStndControls( state_vect& tSV )
    {
	if (rudder_right)
		{
		rudderPos += rudder_sens;
		if (rudderPos > 127)
			rudderPos = 127;
		rudder_right = false;
        }
	else if (rudder_left)
		{
		rudderPos -= rudder_sens;
		if (rudderPos < -128)
			rudderPos = -128;
		rudder_left = false;
		}
	else
        rudderPos = 0;
	if ((throttle_up) && (tSV.throttle_pos < 15))
		{
		tSV.throttle_pos++;
		throttle_up = false;
		}
	else if ((throttle_down) && (tSV.throttle_pos > 0))
		{
		tSV.throttle_pos--;
		throttle_down = false;
		}
	if (ignition_change)
		{
		if (tSV.ignition_on)
			tSV.ignition_on = false;
		else
			tSV.ignition_on = true;
		ignition_change = false;
		}
    if (btn1_pressed)
       tSV.button1 = true;
    else
       tSV.button1 = false;
    if (btn2_pressed)
       tSV.button2 = true;
    else
       tSV.button2 = false;
	if (brake_tog)
		{
		if (tSV.brake)
			tSV.brake = false;
		else
			tSV.brake = true;
		brake_tog = false;
		}
    }

// this function is called from GetControls(). It checks the state of the
// sound and view controls, and updates the state vector if required.
// This function is called on every call to GetControls().

void CheckSndViewControls( state_vect& tSV )
    {
    if (sound_toggle)
       {
       sound_toggle = false;
       tSV.sound_chng = true;
       }
    if (view_forward)
       {
       view_forward = false;
       tSV.view_state = 0;
       }
    else if (view_right)
       {
       view_right = false;
       tSV.view_state = 1;
       }
    else if (view_rear)
       {
       view_rear = false;
       tSV.view_state = 2;
       }
    else if (view_left)
       {
       view_left = false;
       tSV.view_state = 3;
       }
    }

// this function is called from GetControls(). It remaps the controls surface
// deflection indexes to a -15 to +15 range.

void ReduceIndices( state_vect& tSV )
    {
    tSV.aileron_pos /= 7;                   // convert all to +/- 16
    if (tSV.aileron_pos > 15)
        tSV.aileron_pos = 15;
    else if (tSV.aileron_pos < -15)
        tSV.aileron_pos = -15;

    tSV.elevator_pos /= 7;
    if (tSV.elevator_pos > 15)
        tSV.elevator_pos = 15;
    else if (tSV.elevator_pos < -15)
        tSV.elevator_pos = -15;

    tSV.rudder_pos /= 7;
    if (tSV.rudder_pos > 15)
        tSV.rudder_pos = 15;
    else if (tSV.rudder_pos < -15)
        tSV.rudder_pos = -15;
    }

// This function is called from the main program, and is passed a pointer
// to the aircraft state vector. It updates this vector based on the current
// state of the control interface.

void GetControls( state_vect& tSV )
	{
	if (usingStick)                          // is the joystick in use?
        CalcStickControls( tSV );            // yes, grab the control state
    else
    	CalcKeyControls();                   // no, get the keyboard controls
    CalcStndControls( tSV );                 // go get the standard controls

    CheckSndViewControls( tSV );
	tSV.aileron_pos = stickX;               // update the state vector
	tSV.elevator_pos = stickY;              // with values calculated in
    tSV.rudder_pos = rudderPos;             // the other functions
    ReduceIndices( tSV );                    // remap the deflections
	return;
	}




