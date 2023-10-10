//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: AIRCRAFT.CPP
// Project:   Flights of Fantasy
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
// Machine:   IBM PC and Compatibles
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          Initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          Publication release, split out
//                                          sound and view control functions,
//                                          tweaked flight model
//      9-15-95         2.0     mracky      fiddled while rome burned
//
// Description
// -----------
//      This module provides functions for a pseudo flight model
//-------+---------+---------+---------+---------+---------+---------+---------+

#include <conio.h>
#include <math.h>
#include <dos.h>
#include <mem.h>
#include "types.h"          // some generic types
#include "htimer.h"         // hi-res timer
#include "aircraft.h"       // header for this module

//#define __TIMEROFF__         // remove the comments to disable timing
							   // on the flight model loop, and set
							   // loopTime to 100 ms. Useful for debugging
							   // the flight model

//#define __BUTTONBRK__        // if __BUTTONBRK__ is defined a statement
                               // in CalcNewPos() will be executed when
                               // button 1 of the jstick is pressed. This
                               // is a useful place to set a breakpoint for
                               // jumping out at any point.

static const LOOP = 40;        // if the timer is off this value will be
										 // used for the loop timing

// this block declares various data used internally in this module

static HTimer* timer1;               // ptr to hi-res timer object
static dword loopTime;               // elapsed time since last pass
static int frmIndex;                 // index into frame rate save buffer
static boolean frmWrap;              // true when NUM_FRMS frame times saved
static delta_vect deltaVect;         // copy of rotational deltas
static int opMode;                   // = 0 debug, 1 flight, 2 world walk

// miscellaneous constants

static const double PI = 3.1415;     // value of pi
static const NUM_FRMS = 250;         // number of frame times to keep
static const TANK_SIZE = 8;          // size of fuel tank (in tens of gal.)

// flight model constants

static float const GRAV_C = -16.0;   // gravitational constant in ft./sec.
static const SEA_LVL_Y = -20;        // y value of ground level
static const START_Y = SEA_LVL_Y;    // start-up conditions
static const START_Z = -7000;
static const START_X = 0;
static const START_YAW = 0;
static const START_ROL = 0;
static const START_PIT = 0;

static const FLIGHT = 1;              // startup modes:
static const WALK = 2;
static const long WALK_RATE = 100000L; // world walk speed in feet/min

extern CPU_386;                       // set in main(), true if 396/486

static word frameTimes[NUM_FRMS];     // buffer for 500 elapsed frame times

// this function is called from main() (FSMAIN.CPP) when running in debugging
// dump mode.

void AirCraft::ACDump( int x, int& y )
{
   float framesPerSec;

   if (loopTime)
		framesPerSec = 1000.0/loopTime;
   else
      	framesPerSec = 0;
   y++;
   gotoxy( x, y++ );
   cprintf("Physical parameters dump:");
   y++;
   gotoxy(x, y++);
   cprintf("delta z (z per frame): %f       ", delta_z);
   gotoxy(x, y++);
   cprintf("delta pitch (deg./ms): %f       ", deltaVect.dPitch);
   gotoxy(x, y++);
   cprintf("delta yaw (deg./ms):   %f       ", deltaVect.dYaw);
   gotoxy(x, y++);
   cprintf("delta roll (deg./ms):  %f       ", deltaVect.dRoll);
	gotoxy(x, y++);
   cprintf("last frame (ms):       %i       ", loopTime);
   gotoxy(x, y++);
   cprintf("frames per sec.:       %f       ", framesPerSec);
   gotoxy(x, y++);
   cprintf("processor:             ");
   if (CPU_386)
      cprintf("80386/80486");
   else
      cprintf("8086/8088/80286");
}

// This function is called at program termination to calculate and print
// the average time between frames in millseconds

void ReportFrameRate()
{
   int i;
   int frames = 100;

   // REV 2.1 I was getting all sorts of false repoerting from the old
   //    routine.  Since this is only called in the program end, I changed
   //    the total from a long to a float so it would cut out any overflow.
   //    also cut the number of frames recorded in half.
   if ((! frmWrap) && frmIndex < 100)
       frames = frmIndex;
   cprintf("Average time per frame (ms):");
   if (frames) {
       float total =0.0;
       for (i = 0; i < frames; i++)
           total += frameTimes[i];
	   total /= frames;
	   loopTime = long(total);
       cprintf(" %i\r\n", loopTime);
       }
   else
       cprintf(" timer disabled\r\n");
}

// this function is called from CalcNewPos() to place the last time between
// frames in milliseconds in the frameTimes[] array. The array index wraps
// at 499, so the buffer is continually overwritten. When the program
// terminates ReportFrameRate() reports the average of the last 500 elapsed
// times

void near AddFrameTime()
{
   frameTimes[frmIndex] = (word)loopTime;
   frmIndex++;
   if (frmIndex == NUM_FRMS)   { // just wrap it around
	  frmWrap++;
      frmIndex = 0;
	}
}

// sets up aircraft conditions at start

void AirCraft::ResetACState(  )
{
   	// setup some initial values in the aircraft state vector
 	aileron_pos = elevator_pos = throttle_pos = rudder_pos = 0;
	button1 = button2 = ignition_on = engine_on = stall = false;
	altitude = rpm = 0;
	pitch = yaw = roll = 0.0;
	h_speed = v_speed = delta_z = efAOF = climbRate = 0.0;
    fuel = TANK_SIZE;       // starting fuel level/ units of 10
    fuelConsump = 8;        // consumption in gallons/hr
    x_pos = START_X;        // starting x position
    y_pos = START_Y;        // sea level = world y -40
    z_pos = START_Z;        // starting z position
    yaw = START_YAW;        // starting direction
    pitch = START_PIT;
    roll = START_ROL;
    airborne = false;       // "on the ground" flag
    brake = true;
    deltaVect.dPitch = 0;
	 deltaVect.dRoll = 0;
    deltaVect.dYaw = 0;
	view_state = 0;
	//sound_chng = true;
}

// set aircraft conditions after landing

void AirCraft::LandAC(  )
{
   airborne = false;
   pitch = 0;
   roll = 0;
   y_pos = SEA_LVL_Y;
   deltaVect.dPitch = 0;
	deltaVect.dRoll = 0;
	deltaVect.dYaw = 0;
}

// initializes aircraft statevector and flight model, starts internal timer

boolean AirCraft::InitAircraft( int mode )
{
	 boolean result = true;

	 loopTime = 0;

	 if (( timer1 = new HTimer()) == NULL )
		 result = false;
	 else     {
		 ResetACState( );            // set the starting aircraft state
		 timer1->timerOn();          // don't want 0 on first pass
		 frmWrap = false;            // flag used by frame rate accumulator
	 }
	 opMode = mode;
	return( result );
}

// this function is called from main() to shut down the aircraft model and
// cockpit graphics systems

void AirCraft::Shutdown()
{
    if ( timer1 != NULL )
       delete timer1;
}

// converts degrees to radians - expects degrees in -179 to +180 format

double near Rads( double degrees )
     {
     double result;

     if (degrees < 0)
        degrees += 360;
     result = degrees * (PI / 180);
     return( result );
     }

// converts radians to degrees

double near Degs( double radians )
     {
     double result;
     result = radians * (180 / PI);
     return( result );
     }


// FLIGHT MODEL STEP 1
// This function adjusts the engine rpm for the current iteration of the
// flight model. It also toggles the engine on/off in response to changes
// in the state_vect.ignition_on parameter

void AirCraft::CalcPowerDyn(  )
{
  	if ( ignition_on ) {               // is the ignition on?
  		if (! engine_on)                        // yes, engine running?
        	engine_on = true;                   // no, turn it on

       	// increment or decrement the rpm if it is less than or greater than
       	// nominal for the throttle setting
       	if (rpm < (375 + (throttle_pos * 117)))
        	rpm += loopTime * .5;
       	if (rpm > (375 + (throttle_pos * 117)))
          	rpm -= loopTime * .5;
    }
    else {                            // no, ignition is off
       if (engine_on)                         // is the engine running?
          engine_on = false;                  // yes, shut it off
       if(rpm)                                // rpm > 0 ?
		  rpm -= (int)(loopTime / 2);          // yes, decrement it
    }
    if (rpm < 0)                              // make sure it doesn't
       rpm = 0;                               // end up negative
}

// FLIGHT MODEL STEP 2
// This function calculates the flight dynamics for the current pass
// through the flight model. The function does not attempt to model actual
// aerodynamic parameters. Rather, it is constructed of equations developed
// to produce a reasonable range of values for parameters like lift, speed,
// horizontal acceleration, vertical acceleration, etc.

void AirCraft:: CalcFlightDyn( )
{
	float iSpeed;                       // speed ideally produced by x rpm
    float lSpeed;                       // modified speed for lift calc.
    float hAccel;                       // horizontal acceleration (thrust)
    float lVeloc;                       // vertical velocity from lift
    float gVeloc;                       // vertical velocity from gravity
    float AOA;                          // angle of attack

    iSpeed = rpm / 17.5;            // calc speed from rpm
    iSpeed += (pitch * 1.5);        // modify speed by pitch

    hAccel = ((rpm * (iSpeed - h_speed)) / 10000);
    hAccel /= 1000;
    hAccel *= loopTime;

    if ((brake) && (!airborne))
       {
       if (h_speed > 0)             // brake above 0 m.p.h.
          h_speed -= 1;
       else
          h_speed = 0;              // settle speed at 0 m.p.h.
       }
    else
       h_speed += hAccel;           // accelerate normally

    lSpeed = (h_speed / 65) - 1;      // force speed to range -1..1
    if (lSpeed > 1) lSpeed = 1;           // truncate it at +1
    lVeloc = Degs(atan(lSpeed));          // lift curve: L = arctan(V)
    lVeloc += 45;                         // force lift to range 0..90
    lVeloc /= 5.29;                       // shift to range 0..~17
    lVeloc *= (-(pitch * .157) + 1);  // multiply by pitch modifier
    lVeloc /= 1000;                       // time slice
    lVeloc *= loopTime;

    gVeloc = loopTime * (GRAV_C / 1000);      // grav. constant this loop
    v_speed = gVeloc + lVeloc;            // sum up the vertical velocity
    if ((!airborne) && (v_speed < 0)) // v_speed = 0 at ground level
       v_speed = 0;
    climbRate = v_speed/loopTime;     // save the value in feet/min.
    climbRate *= 60000L;

    delta_z = h_speed * 5280;         // expand speed to feet/hr
    delta_z /= 3600000L;                   // get feet/millisecond
    delta_z *= loopTime;                  // z distance travelled

    if (delta_z)                          // find effective angle of flight
       efAOF = -(atan(v_speed / delta_z));
    else
       efAOF = -(atan(v_speed));      // atan() returns radians

    AOA = Degs(efAOF);                    // convert to degrees

    // handle a stalling condition

    if (((pitch < AOA) && (AOA < 0)) && (h_speed < 40))
       {
		 if ((pitch - AOA) < -20)
          stall = true;
       }
    if (stall)
       {
       if (pitch > 30)
          stall = false;
       else
          pitch++;
       }
}

// FLIGHT MODEL STEP 3
// This function attempts to simulate inertial damping of the angular rates
// of change. It needs a lot of work, but you can see it's effects now in
// the "momentum" effect when the aircraft is rolled

void near InertialDamp()
{
	 // simulates inertial damping of angular velocities
	 if (deltaVect.dPitch)
		 {
		 deltaVect.dPitch -= deltaVect.dPitch / 10;
		 if ( ((deltaVect.dPitch > 0) && (deltaVect.dPitch < .01 )) ||
				((deltaVect.dPitch < 0) && (deltaVect.dPitch > -.01 )) )
			 deltaVect.dPitch = 0;
		 }
	 if (deltaVect.dYaw)
		 {
		 deltaVect.dYaw -= deltaVect.dYaw / 10;
		 if ( ((deltaVect.dYaw > 0) && (deltaVect.dYaw < .01 )) ||
				((deltaVect.dYaw < 0) && (deltaVect.dYaw > -.01 )) )
			 deltaVect.dYaw = 0;
		 }
	 if (deltaVect.dRoll)
		 {
		 deltaVect.dRoll -= deltaVect.dRoll / 10;
		 if ( ((deltaVect.dRoll > 0) && (deltaVect.dRoll < .01 )) ||
				((deltaVect.dRoll < 0) && (deltaVect.dRoll > -.01 )) )
			 deltaVect.dRoll = 0;
		 }
	 }

// FLIGHT MODEL STEP 4
// this function is called from CalcROC() to calculate the current turn
// rate based on roll

float AirCraft::CalcTurnRate(  )
	{
	float torque = 0.0;

	if ((roll > 0) && (roll <= 90))
		torque = (roll * .00050);                   // (.00026)
	else if ((roll < 0) && (roll >= -90))
		torque = (roll * .00050);
	return( torque );
	}

// FLIGHT MODEL STEP 5
// This function finds the current rates of change for aircraft motion in
// the three axes, based on control surface deflection, airspeed, and
// elapsed time. It uses the values in the ROC lookup table at the top
// of this file

void AirCraft::CalcROC( )
	 {
	 float torque;

    // load deltaVect struct with delta change values for roll, pitch, and
    // yaw based on control position and airspeed
    if (airborne)
       {
       if (aileron_pos != 0)
          {
			 torque = ((h_speed * aileron_pos) / 10000);
          if (deltaVect.dRoll != (torque * loopTime))
             deltaVect.dRoll += torque * 6; // *8
          }
       }
    if ( elevator_pos != 0 )
		 {
		 torque = ((h_speed * elevator_pos) / 10000);
		 if ((!airborne) && (torque > 0))
          torque = 0;
       if (deltaVect.dPitch != (torque * loopTime))
          deltaVect.dPitch += torque * 1.5;    //* 4
       }
    if (h_speed)
       {
		 torque = 0.0;
       if (rudder_pos != 0)
          torque = -((h_speed * rudder_pos) / 10000);
       torque += CalcTurnRate( );
       if (deltaVect.dYaw != (torque * loopTime))
          deltaVect.dYaw += torque * 1.5;   // *8
		 }
	 }

// FLIGHT MODEL STEP 6
// This function applies the current angular rates of change to the
// current aircraft rotations, and checks for special case conditions
// such as pitch exceeding +/-i90 degrees

void AirCraft::ApplyRots( )
	 {
	 // transform pitch into components of yaw and pitch based on roll
	 roll += deltaVect.dRoll;
	 yaw += deltaVect.dYaw;
	 pitch += (deltaVect.dPitch * cos(Rads(roll)));
	 yaw += -(deltaVect.dPitch * sin(Rads(roll)));

	 // handle bounds checking on roll and yaw at 180 or -180
	 if (roll > 180)
		 roll = -180 + (roll - 180);
	 else if (roll < -180)
		 roll = 180 + (roll - -180);
	 if (yaw > 180)
		 yaw = -180 + (yaw - 180);
	 else if (yaw < -180)
       yaw = 180 + (yaw - -180);

    // handle special case when aircraft pitch passes the vertical
    if ((pitch > 90) || (pitch < -90))
       {
       if (roll >= 0)
          roll -= 180;
       else if (roll < 0)
          roll += 180;
       if (yaw >= 0)
          yaw -= 180;
       else if (yaw < 0)
			 yaw += 180;
       if (pitch > 0)
			 pitch = (180 - pitch);
       else if (pitch < 0)
          pitch = (-180 - pitch);
		 }
    // dampen everything out to 0 if they get close enough

    if ((pitch > -.5) && (pitch < .5))
		 pitch = 0;
    if ((roll > -.5) && (roll < .5))
       roll = 0;
    if ((yaw > -.5) && (yaw < .5))
       yaw = 0;

    }

// WORLD WALK MODE ONLY
// this function controls the movement of the view center when the
// program is running in world walk mode. In this mode the flight
// controls move you around the world. This is useful for inspection
// when designing scenery.

void AirCraft::DoWalk(  )
{
   delta_z = throttle_pos * (WALK_RATE/15);
   delta_z /= 60000L;
   delta_z *= loopTime;
   efAOF = Rads(pitch);
   h_speed = 140;
}


// this function is called from GetControls(). It remaps the controls surface
// deflection indexes to a -15 to +15 range.

void AirCraft::ReduceIndices( )
{
    aileron_pos /= 7;                   // convert all to +/- 16
    if (aileron_pos > 15)
        aileron_pos = 15;
    else if (aileron_pos < -15)
        aileron_pos = -15;

    elevator_pos /= 7;
    if (elevator_pos > 15)
        elevator_pos = 15;
    else if (elevator_pos < -15)
        elevator_pos = -15;

    rudder_pos /= 7;
    if (rudder_pos > 15)
        rudder_pos = 15;
    else if (rudder_pos < -15)
        rudder_pos = -15;
}

void AirCraft::ToggleIgnition()
{
	ignition_on = ignition_on?false:true;
}

void AirCraft::ToggleBrakes()
{
	brake = brake?false:true;

}



// ENTRY POINT FOR FLIGHT MODEL LOOP
// This function takes as parameters references to a state_vect structure
// containing the control input from the current pass, as well as the
// values for all other aircraft data from the previous pass.
//
// *A special thanks to Peter Rushworth, who called me from England at 4:00
// *in the morning (his time) to help me work on the pitch and roll component
// *calculations.
//
// DEBUGGING NOTE: If __TIMEROFF__ is defined (see top of this module) then
// this function sets the loopTime variable to LOOP ms, else it uses the timer1
// object to time the running of the flight model. The loopTime variable
// is set on entry to this module with the number of elapsed ms since the
// last call, and then used throughout the module for calculations of rate
// of change parameters. Defining __TIMEROFF__ effectively sets the
// performance of the system to match my 486. This lets you step through the
// flight model and get a nice, smooth change in the variables you're
// watching. Otherwise the timer continues to run while you're staring at the
// debugger screen.

void AirCraft::RunFModel( )
{
     float tmpX, tmpY, tmpZ;         // these are used later to preserve
     float newX, newY, newZ;         // position values during conversion
     static float collectX;          // accumulators for delta changes in
     static float collectY;          // x, y, and z world coords; adjusts
     static float collectZ;          // for rounding errors

	 //REV2.1   I changed the next compiiler conditional to positive logic
	 //  
#ifdef __TIMEROFF__                  // this block controls whether the
									 // timer is in use for flight
									 // calculations purposes
                                     // or running at LOOP ms for debugging
									 // when __TIMEROFF__ flag is on
     loopTime = LOOP;           
#else
     loopTime = timer1->timerOff();  
     timer1->timerOn();              
     if (!(loopTime /= 1000)) loopTime = 1;
     AddFrameTime();
#endif

#ifdef __BUTTONBRK__                 // this block allows a "break on button"
     if (button1)
        loopTime = loopTime;         // SET BREAKPOINT HERE FOR BREAK
#endif                               // ON BUTTON 1 PRESS

     // these seven near calls update all current aircraft parameters
     // based on the input from the last pass through the control loop
     // The order in which they are called is critical

     if (opMode == WALK)
        DoWalk();               // traverse the world
     else  {
        CalcPowerDyn();         // calculate the power dynamics
        CalcFlightDyn();        // calculate the flight dynamics
        }

     InertialDamp();                 // apply simulated inertial dampening
     CalcROC( );                 // find the current rates of change
     ApplyRots( );               // apply them to current rotations

     // The rest of this function calculates the new aircraft position

     // start the position calculation assuming a point at x = 0, y = 0,
     // z = distance travelled in the last time increment, assuming that
     // each coordinate in 3-space is equivalent to 1 foot

     tmpX = 0;                       // using temps because we need the
     tmpY = 0;                       // original data for the next loop
     tmpZ = delta_z;

     // note that the order of these rotations is significant
     // rotate the point in Z

	 // REV 2.1 equation always will be 0 since tmpX and tmpY are 
	 //         set equal to 0
	 //
     //newX = 0.0; //(tmpX * cos(Rads(roll))) - (tmpY * sin(Rads(roll)));
     //newY = 0.0; //(tmpX * sin(Rads(roll))) + (tmpY * cos(Rads(roll)));
			     //tmpX = newX;
			     //tmpY = newY;

     // rotate the point in x
	
    		 //newY = (tmpY * cos(efAOF)) - (tmpZ * sin(efAOF));
		     //newZ = (tmpY * sin(efAOF)) + (tmpZ * cos(efAOF));
     newY =  - (tmpZ * sin(efAOF));
     newZ =  (tmpZ * cos(efAOF));
     tmpY = newY;
     tmpZ = newZ;

     efAOF = Degs(efAOF);

     // rotate the point in y

		     //newX = (tmpZ * sin(Rads(yaw))) + (tmpX * cos(Rads(yaw)));
		     //newZ = (tmpZ * cos(Rads(yaw))) - (tmpX * sin(Rads(yaw)));
		     //tmpX = newX;
		     //tmpZ = newZ;
     newX = (tmpZ * sin(Rads(yaw))) ;
     newZ = (tmpZ * cos(Rads(yaw))) ;

     // translate the rotated point back to where it should be relative to
     // the last position (remember, the starting point for the rotations
     // is an imaginary point at world center)

     collectX += newX;
     if ((collectX > 1) || (collectX < -1))  {
        x_pos -= collectX;
        collectX = 0;
        }
     collectY += newY;
     if ((collectY > 1) || (collectY < -1))  {
        y_pos -= collectY;
        collectY = 0;
        }
     collectZ += newZ;
     if ((collectZ > 1) || (collectZ < -1))  {
        z_pos += collectZ;
        collectZ = 0;
        }

     altitude = -(y_pos - SEA_LVL_Y);

     // set the airborne flag when we first take off
     if ((!airborne) && (altitude))
        airborne = true;
}



