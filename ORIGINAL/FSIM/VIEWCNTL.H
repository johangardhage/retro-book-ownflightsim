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

#ifndef TYPES_H
#incldue "types.h"
#endif

#ifndef AIRCRAFT_H
#include "aircraft.h"
#endif

boolean InitView( Pcx* theBGround, int mode);
void ViewShutDown();
void ViewParamDump( int x, int& y );
void ShowCrash();
boolean UpdateView( const AirCraft& AC );

#endif
