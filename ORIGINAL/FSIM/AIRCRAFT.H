//-------+---------+---------+---------+---------+---------+---------+---------+
// Copyright (c) 1991-1992 Betz Associates. All rights reserved.
//
// File Name: AIRCRAFT.H
// Project:   Flights of Fantasy
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//      8-23-1995       2.0     mickRacky   second edition update
//
// Description
// -----------
//      This file contains definitions for structures, and prototypes of
//      interface functions for the aircraft model.
//
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef AIRCRAFT_H
#define AIRCRAFT_H

// This is the aircraft model state vector type. It holds the current state
// of the aircraft, controls, attitude, and velocities, as well as the
// current view status. This struct is modified by the functions in
// aircraft.cpp, input.cpp, and fsmain.cpp. However, the only declaration
// of this type (in the current version) is in fsmain.cpp. The other modules
// get it by reference during function calls

struct state_vect {
	int aileron_pos;           // aileron position -15 to 15
	int elevator_pos;          // elevator position -15 to 15
	int throttle_pos;          // throttle position 0 to 16
	int rudder_pos;            // rudder position -15 to 15
	boolean button1;           // stick buttons, true if pressed
	boolean button2;
	boolean ignition_on;       // ignition state on/off (true/false)
    boolean engine_on;         // engine running if true
    int rpm;                   // rpm of engine
    byte fuel;                 // gallons of fuel
    byte fuelConsump;          // fuel consumption in gallons/hr.
	int x_pos;                 // current location on world-x
	int y_pos;                 // current location on world-y
	int z_pos;                 // current location on world-z
	double pitch;              // rotation about x 0 to 255
	double yaw;                // rotation about y 0 to 255
	double roll;               // rotation about z 0 to 255
	float h_speed;             // horizontal speed (airspeed, true)
	float v_speed;             // vertical speed during last time slice
    float delta_z;             // z distance travelled in last pass
    float efAOF;               // effective angle of flight
    float climbRate;           // rate of climb in feet per minute
	int altitude;              // altitude in feet
    boolean airborne;          // true if the plane has taken off
    boolean stall;             // true if stall condition
    boolean brake;             // true if brake on
    byte view_state;           // which way is the view pointing
    byte sound_chng;           // boolean true if sound on/off state chngd
};

// struct delta_vect is used by the aircraft modeling functions in
// aircraft.cpp as a container for the current delta values for the
// aircraft rotations.

struct delta_vect
{
    double dPitch;             // delta change in pitch (deg.) per ms
    double dYaw;               // delta change in yaw (deg.) per ms
    double dRoll;              // delta change in roll (deg.) per ms
};


// the AirCraft class
class AirCraft: public state_vect 
{
private:	
	void  CalcPowerDyn();
	void  CalcFlightDyn();
	float CalcTurnRate();
	void  CalcROC();
	void  ApplyRots();
protected:
	void  DoWalk();
public:
	inline boolean AnyButton()
		{ return (button1 || button2) ; }

	// start up the aircraft model. This must be called at program start-up
	boolean InitAircraft( int mode );

	// shut down the aircraft model. You have to call this one at exit. 
	// If you don't the very least that will happen is that the sound 
	// will stay on after the program terminates
	void Shutdown();

	// subseqent functions are the hooks to the rest of the program. 

	// RunFModel() is called to iterate the flight model one step. 
	// It is normally called once per frame, but will work properly 
	// no matter how often you call it per frame (up to some 
	// theoretical limit at which timer inaccuracy at low microsecond 
	// counts screws up the delta rate calculations)
	void RunFModel();


	void ResetACState( );
	void LandAC( );

	// Called from GetControls() to remap surface deflection:
	void ReduceIndices();
	// change ignition to opposite state: off or on
	void ToggleIgnition();
	void ToggleBrakes();

	// ACDump() performs a text-mode screen dump of the flight model's
	// internal data. It is only called when the program is running in
	// debugging mode
void ACDump( int x, int& y );

};
	// ReportFrameRate() reports the average of the last 500 elapsed frame
	// times. Called once at program termination
void ReportFrameRate();

#endif