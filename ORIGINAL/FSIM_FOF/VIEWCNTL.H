//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: VIEWCNTL.H
// Project:   Flights of Fantasy
// Creation:  September 12, 1992
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
//      9-12-1992       1.0     MB          initial development, functions
//                                          split off from AIRCRAFT.H
//      9-26-1992       1.1     MB          publication release
//
// Description
// -----------
//      funtion prototypes for control of the aircraft view
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef __VIEWCNTL__
#define __VIEWCNTL__

boolean InitView( Pcx* theLoader, pcx_struct* theBGround, int mode);
boolean ViewShutDown();
void ViewParamDump( int x, int& y );
void ShowCrash();
boolean UpdateView( state_vect& tSV );

#endif
