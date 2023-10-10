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
//      10-17-95        2.1     mracky      reduced math expressions;            
//                                          changed reset: frame rate
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
#include "fix.h"
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
static const NUM_FRMS = 500;         // number of frame times to keep
static const TANK_SIZE = 8;          // size of fuel tank (in tens of gal.)

// flight model constants

static float const GRAV_C = -0.016;   // gravitational constant in ft./sec.
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

	// REV 2.2 fix stall angle
const int STALL_BACK =(NUMBER_OF_DEGREES/18);
	// stall is >   (30 in 360 degrees)
const int STALL_FORWARD =(NUMBER_OF_DEGREES/12) ;

extern CPU_386;                       // set in main(), true if 396/486

static word frameTimes[NUM_FRMS];     // buffer for 500 elapsed frame times

//=========================
// INLINE conversions
// converts degrees to radians - expects degrees in -179 to +180 format

inline double Rads( double degrees )
{
     if (degrees < 0)
        degrees += NUMBER_OF_DEGREES;
     return( degrees * (PI / HALF_CIRCLE) );
}

// converts radians to degrees where 360 degrees are in a circle.

inline double Degs360( double radians )
{
     return( radians * (180 / PI));
}
//==================================
// this function is called from main() (FSMAIN.CPP) when running in debugging
// dump mode.

void AirCraft::ACDump( int x, int& y )
{
   float framesPerSec;

   if (loopTime)    {
	  framesPerSec = 1000/loopTime;
   }
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
      cprintf("80386/80486/pentium");
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
   if (frames) {
       float total =0.0;
   	   cprintf("Frame count:%d\n",frames);
       for (i = 0; i < frames; i++)	   {
           total += frameTimes[i];
		   cprintf("%d  ",frameTimes[i]);
		   if((i%6)==5) cprintf("\r\n");
		}
	   total /= frames;
	   loopTime = long(total);
   	   cprintf("\r\nAverage time per frame (ms):");
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
	frameTimes[frmIndex++] = (word)loopTime;
   	if (frmIndex == NUM_FRMS)    {
      frmIndex = 0;
      frmWrap = true;
    }
}

// sets up aircraft conditions at start

void AirCraft::ResetACState(  )
{
   	// setup some initial values in the aircraft state vector
 	aileron_pos = 
	elevator_pos = 
	throttle_pos = 
	rudder_pos = 0;
	button1 = 
	button2 = 
	ignition_on = 
	engine_on = 
	stall = false;
	altitude = 
	rpm = 0;
	
	h_speed = v_speed = delta_z = climbRate = 0.0;
	efAOF = 0;
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
	sound_chng = false;
		// REV 2.1 
		// reset frame rate or it will report false time from crash
	frmIndex = 0;
	frmWrap = false;
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
		int tmp;
       	if (rpm <= (tmp =(BASE_RPM + (throttle_pos * RPM_INDEX))))
        	rpm += (loopTime >> 1);  // REV 2.1 shift instead of mult * .5)
       	if (rpm > tmp)               // REV 2.1 only calc throttle * RPM_INDEX once
          	rpm -= (loopTime >> 1);  // REV 2.1 shift intstead of mult by .5
    }
    else {                            // no, ignition is off
       if (engine_on) {                        // is the engine running?
          engine_on = false;                  // yes, shut it off
		  throttle_pos = 0;                   // must re-throttle
	   }
       if(rpm)                                // rpm > 0 ?
		  rpm -= (int)(loopTime >>1);         // yes, decrement it
    }										  // REV 2.1 shift instead of /2
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
	float iSpeed;        // speed ideally produced by x rpm
    float lSpeed;        // modified speed for lift calc.
    float hAccel;        // horizontal acceleration (thrust)
    float lVeloc;        // vertical velocity from lift
    float gVeloc;        // vertical velocity from gravity
    float AOA;           // angle of attack

				           			// calc speed from rpm
    iSpeed = rpm * 0.057143; 		// REV 2.1 formerly divide by 17.5
    iSpeed += (pitch * 1.5);        // modify speed by pitch

				// REV 2.1 formerly  divide by 10000 
				// followed by / 1000
				// followed by *loopTime
    hAccel = loopTime * ((rpm * (iSpeed - h_speed)) * 0.0000001); 
    //	hAccel /= 1000;							   
    // hAccel *= loopTime;

    if ((brake) && (!airborne)) {
       if (h_speed > 0)             // brake above 0 m.p.h.
          h_speed -= 1;             // h_speed is float
       else
          h_speed = 0;              // settle speed at 0 m.p.h.
       }
    else
       h_speed += hAccel;           	// accelerate normally

    //lSpeed = (h_speed / 65) - 1;      // force speed to range -1..1
    lSpeed = (h_speed * 0.01538462) - 1;  // force speed to range -1..1
    if (lSpeed > 1) 
		lSpeed = 1;           			// truncate it at +1
    lVeloc = Degs360(atan(lSpeed));     // lift curve: L = arctan(V)
    lVeloc += 45;         				// force lift to range 0..90
    //lVeloc /= 5.29;                   // shift to range 0..~17
    lVeloc *= 0.18903592;                 // REV 2.1 chngr to mult
    lVeloc *= (-(pitch * .157) + 1);  	// multiply by pitch modifier
    //lVeloc /= 1000;                   // time slice
	lVeloc *= 0.001;					// REV 2.1 convert to mult
    lVeloc *= loopTime;

    gVeloc = loopTime * (GRAV_C );      // grav. constant this loop
    v_speed = gVeloc + lVeloc;          // sum up the vertical velocity
    if ((!airborne) && (v_speed < 0))   // v_speed = 0 at ground level
       v_speed = 0;
    climbRate = v_speed/loopTime;       // save the value in feet/min.
    climbRate *= 60000L;

    //delta_z = h_speed * 5280;         // expand speed to feet/hr
    //delta_z /= 3600000L;              // get feet/millisecond
	delta_z  = h_speed * 0.00146667;     // REV 2.1 combine operations
    delta_z *= loopTime;                // z distance travelled

    if (delta_z)                        // find effective angle of flight
       efAOF = -(atan(v_speed / delta_z));
    else
       efAOF = -(atan(v_speed));      // atan() returns radians

	   // convert effecti e pitch (in Radians) to our fixed degree system
	long fixedAOF = (long)(efAOF*NUMBER_OF_DEGREES/(2*PI));

    // handle a stalling condition

    if (((pitch < fixedAOF) && (fixedAOF < 0)) && (h_speed < 40))   {
			// REV 2.2 fix stall angle
		 if ((pitch - fixedAOF) < -STALL_BACK)
          stall = true;
    }
    if (stall)   {
			// REV 2.2 fix stall angle
       if (pitch > STALL_FORWARD)
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
	 if (deltaVect.dPitch) {
		 // deltaVect.dPitch -= deltaVect.dPitch / 10; // REV 2.1 optimize

		 deltaVect.dPitch *= 0.9;			   // REV 2.1 change to mult	

		 if ( ((deltaVect.dPitch > 0) && (deltaVect.dPitch < .01 )) ||
				((deltaVect.dPitch < 0) && (deltaVect.dPitch > -.01 )) )
			 deltaVect.dPitch = 0;
	 }
	 if (deltaVect.dYaw) {
		 //deltaVect.dYaw -= deltaVect.dYaw / 10;  // REV 2.1 change to mult
		 deltaVect.dYaw *= 0.9;  // REV 2.1 change to mult
		 if ( ((deltaVect.dYaw > 0) && (deltaVect.dYaw < .01 )) ||
				((deltaVect.dYaw < 0) && (deltaVect.dYaw > -.01 )) )
			 deltaVect.dYaw = 0;
	 }
	 if (deltaVect.dRoll) {
		 //deltaVect.dRoll -= deltaVect.dRoll / 10; // REV 2.1 change to mult
		 deltaVect.dRoll *= 0.9;
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
	if( ((roll > 0) && (roll <= QUARTER_CIRCLE)) || 
		((roll < 0) && (roll >= -QUARTER_CIRCLE)) )
		return (roll * .00050);
	return( 0.0 ); 			// no torque
}

// FLIGHT MODEL STEP 5
// This function finds the current rates of change for aircraft motion in
// the three axes, based on control surface deflection, airspeed, and
// elapsed time. It uses the values in the ROC lookup table at the top
// of this file

void AirCraft::CalcROC( )
{
	 float torque;
	// REV 2.1 change division to multiplication
	//          
    // load deltaVect struct with delta change values for roll, pitch, and
    // yaw based on control position and airspeed
    if (airborne)   {
       if (aileron_pos != 0)  {
			 torque = ((h_speed * aileron_pos) * 0.00001429); // /70000L);
          if (deltaVect.dRoll != (torque * loopTime))
             deltaVect.dRoll += torque * 6; // *8
       }
    }
    if ( elevator_pos != 0 )  {
		// REV 2.1 changed elevator_pos range from {15..-15} to {105..-105}
		 torque = ((h_speed * elevator_pos) * 0.00001429); //  /70000L);
		 if ((!airborne) && (torque > 0))
          torque = 0;
       if (deltaVect.dPitch != (torque * loopTime))
          deltaVect.dPitch += torque * 1.5;    //* 4
    }
    if (h_speed)    {
		 torque = 0.0;
       if (rudder_pos != 0)
          torque = -((h_speed * rudder_pos) * 0.00001429); // /70000L);
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
	 float froll = deltaVect.dRoll + float(roll);
	 float fyaw =  deltaVect.dYaw + float(yaw);
	 
	 double radroll = Rads(froll);
	 float fpitch = (deltaVect.dPitch * cos(radroll)) + (float) pitch;
	 fyaw        -= (deltaVect.dPitch * sin(radroll));

	 // handle bounds checking on roll and yaw at 180 or -180
	 if (froll > HALF_CIRCLE)
		 froll -= NUMBER_OF_DEGREES;
	 else if (froll < -HALF_CIRCLE)
		 froll += NUMBER_OF_DEGREES;
	 if (fyaw > HALF_CIRCLE)
		 fyaw -= NUMBER_OF_DEGREES;
	 else if (fyaw < -HALF_CIRCLE)
       fyaw += NUMBER_OF_DEGREES;

    // handle special case when aircraft pitch passes the vertical
    if ((fpitch > QUARTER_CIRCLE) || (fpitch < -QUARTER_CIRCLE))    {
       if (froll >= 0)
          froll -= HALF_CIRCLE;
       else if (froll < 0)
          froll += HALF_CIRCLE;
       if (fyaw >= 0)
          fyaw -= HALF_CIRCLE;
       else if (fyaw < 0)
			 fyaw += HALF_CIRCLE;
       if (fpitch > 0)
			 fpitch = (HALF_CIRCLE - fpitch);
       else if (fpitch < 0)
          fpitch = (-HALF_CIRCLE - fpitch);
	}
    // dampen everything out to 0 if they get close enough

    if ((fpitch > -.5) && (fpitch < .5))
		 fpitch = 0;
    if ((froll > -.5) && (froll < .5))
       froll = 0;
    if ((fyaw > -.5) && (fyaw < .5))
       fyaw = 0;
	roll = (long)froll;
	pitch= (long)fpitch;
	yaw = (long)fyaw;

}

// WORLD WALK MODE ONLY
// this function controls the movement of the view center when the
// program is running in world walk mode. In this mode the flight
// controls move you around the world. This is useful for inspection
// when designing scenery.

void AirCraft::DoWalk(  )
{
   delta_z = (throttle_pos * (WALK_RATE/15));
   delta_z /= 60000L;
   delta_z *= loopTime;
   	// convert to rads
   efAOF = Rads(pitch);
   h_speed = 140;
}

inline void Limit(int& lvar, const int low, const int high)
{
	if(lvar > high) lvar = high;
	if(lvar < low) lvar = low;
}
// this function is called from GetControls(). It remaps the controls surface
// deflection indexes to a -15 to +15 range.
// REV 2.1 there was no reason for such a range.  I grepped all variables and
//            changed range to 105

void AirCraft::ReduceIndices( )
{
		// REV 2.1 
    Limit(aileron_pos , (-AILERON_RANGE), AILERON_RANGE);
	Limit(elevator_pos, (-ELEVATOR_RANGE), ELEVATOR_RANGE);
	Limit(rudder_pos, (-RUDDER_RANGE), RUDDER_RANGE);
}

void AirCraft::ToggleIgnition()
{
	ignition_on = ignition_on?false:true;
}

void AirCraft::ToggleBrakes()
{
	brake = brake?false:true;

}

const long SHIFT_FRACMASK = ((1<<(SHIFT+1))-1);
inline float FixToFloat(long math)
{
	return 
		float((math>>SHIFT)) + float((math&SHIFT_FRACMASK))/SHIFT_MULT;
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
     static float collectX;          // accumulators for delta changes in
     static float collectY;          // x, y, and z world coords; adjusts
     static float collectZ;          // for rounding errors

#ifndef __TIMEROFF__                 // this block controls whether the
     loopTime = timer1->timerOff();  // timer is in use for flight
     timer1->timerOn();              // calculations...
     if (!(loopTime /= 1000)) loopTime = 1;
     AddFrameTime();
#else                                // ...or running at LOOP ms for debugging
     loopTime = LOOP;                // purposes
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

	 //REV 2.1 remove superfluous equations.
	 //        since tmpX and tmpY are initted to 0
	 //        equations are not neccessary...elimination of
	 //        tmpX and tmpY.
	 //float tmpX, tmpY, tmpZ;       // these are used later to preserve
     float newX, newY, newZ;         // position values during conversion

     //tmpX = 0;                       // using temps because we need the
     //tmpY = 0;                       // original data for the next loop
     // tmpZ = delta_z;

	 // REV 2.1 radtemp no longer needed since roll converted to 
	 //          FSIM's NUMBER_OF_DEGREES and sin to fixed pt SIN
	 //
     // note that the order of these rotations is significant
     // rotate the point in Z
	 // double radtemp;
	 //radtemp = Rads(roll);
     //newX = (tmpX * cos(radtemp)) - (tmpY * sin(radtemp));
     //newY = (tmpX * sin(radtemp)) + (tmpY * cos(radtemp));
     //newX = ((tmpX * COS(roll)) - (tmpY * SIN(roll))) >> SHIFT;
     //newY = ((tmpX * SIN(roll)) + (tmpY * COS(roll))) >> SHIFT;
     //tmpX = newX;
     //tmpY = newY;


     // rotate the point in x

     //newY = (tmpY * cos(efAOF)) - (tmpZ * sin(efAOF));
     //newZ = (tmpY * sin(efAOF)) + (tmpZ * cos(efAOF));
     //tmpY = newY;
	 newZ =   delta_z * cos(efAOF);
     newY =  -(delta_z * sin(efAOF));

	 // REV 2.1 efAOF has been changed to stay Radians
     //efAOF = Degs360(efAOF);

     // rotate the point in y
	 //radtemp = Rads(yaw);
     //newX = (tmpZ * sin(radtemp)) + (tmpX * cos(radtemp));
     //newZ = (tmpZ * cos(radtemp)) - (tmpX * sin(radtemp));
     //tmpX = newX;
     //tmpZ = newZ;
	 long math;
	 math = long(newZ) * SIN(yaw);
     newX = FixToFloat(math);
     math = long(newZ) * COS(yaw);
     newZ = FixToFloat(math);

     // translate the rotated point back to where it should be relative to
     // the last position (remember, the starting point for the rotations
     // is an imaginary point at world center)

     collectX += newX;
     if ((collectX > 1) || (collectX < -1)) {
        x_pos -= collectX;
        collectX = 0;
     }
     collectY += newY;
     if ((collectY > 1) || (collectY < -1)) {
        y_pos -= collectY;
        collectY = 0;
     }
     collectZ += newZ;
     if ((collectZ > 1) || (collectZ < -1)) {
        z_pos += collectZ;
        collectZ = 0;
        }

     altitude = -(y_pos - SEA_LVL_Y);

     // set the airborne flag when we first take off
     if ((!airborne) && (altitude))
        airborne = true;
}



