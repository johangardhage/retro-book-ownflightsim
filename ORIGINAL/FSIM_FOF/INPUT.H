//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1990-1991 Betz Associates. All rights reserved.
//
// File Name: INPUT.H
// Project:   Flights of Fantasy
// Creation:  March 10, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC's and Compatibles
//
// Includes:  nothing
//
//-------+---------+---------+---------+---------+---------+---------+---------+
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      4-10-1992       1.0     MB          initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//
// Description
// -----------
//      Function prototypes and data definitions for the flight simulator
//      user input routines.
//
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __INPUT__
#define __INPUT__

byte InitControls();                      // start-up control system
void GetControls( state_vect& tSV );      // load state vector w/control data
boolean Exit();                           // return true if esc pressed
byte ControlShutDown();                   // shut down control system
byte ReadStickPosit(byte axisNum);        // read joystick axis position
boolean Button1();                        // return true if button 1 down
boolean Button2();                        // return true if button 2 down
boolean KeyPressed();                     // return true if key pressed
#endif

