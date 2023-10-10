Mick,


NOTES
-----
Please give David Schultz a credit he provided the music

Please give me a credit (programming)

Please go over the fsimstk.mak & fsimstk.ide and see if you like em



The following is a list of the files DiamondWare will be providing for the
fsim.  They are the files you will want to integrate with your latest
version of the fsim (I hope).


	 DOS MAKE FILES
	 --------------
	 make.bat
	 asmobjs
	 cpps


	 WIN MAKE & IDE PROJECT FILES
	 ----------------------------
	 fsimstk.mak
	 fsimstk.ide
	 fsimstk.dsw


	 SOURCE
	 ------
	 sound.cpp
	 sound.h

	 sbfm.cpp
	 sbfm.h

	 sndcntrl.cpp
	 sndcntrl.h

	 htimestk.cpp
	 soundstk.cpp

	 dwdsp.c
	 dwdsp.h

	 err.c
	 err.h

	 dws.lib
	 dws.h
	 dwt.h


	 MUSIC & SOUNDS
	 --------------
	 moon2.dwm
	 brake.dwd
	 crash.dwd
	 landing.dwd
	 plane.dwd
	 stallbuz.dwd
	 starter.dwd
	 swtch.dwd
	 wind.dwd




QUESTIONS
---------
make.bat is ment to work with mixed directories, the code for both
fsim & fsimstk.  Will you be leaving them combined in the same
source directory?



CODE CHANGES
------------
	There are some minor changes that need to be made in the code you will
be provideing.


The function sound_Kill has been removed, since its kill is not order
dependant the atexit function is ideal.

you will need to remove it in fsmain.cpp

(*
 . fsmain.cpp
*)
void ShutDown()
	 {
	 if (checkpt >= 1)
			ControlShutDown();      // input.cpp
	 if (checkpt >= 2)
			ViewShutDown();         // viewcntl.cpp
	 if (checkpt >= 3)
			AcShutdown(); 					// aircraft.cpp

(******************************************************************
 I have changed sndcntl.cpp API you no longer need the following line
******************************************************************)
	 //SoundShutDown(); 					// sndcntl.cpp

	 setgmode( oldVmode );      // screen.asm
	 }




(*
 . aircraft.cpp
*)

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


(******************************************************************
 I have changed sndcntl.cpp. It will now properly turn sounds on & off.
 You will no longer need the following line
******************************************************************)
	//sound_chng = true;
}






POSSIBLE BUGS??
---------------
When I went looking for the missing crash screen I found seveal quirks.

	1) The garbage that was showing up on my screen was the crash icon
		 and it never go erased
	2) SoundCrash was not being called
	3) there was no wait for a keypress



(*
 . fsmain.cpp
*)

void GroundApproach()
{
	// handle approaching the ground
	if (opMode == FLIGHT) 	 {
		if ( (theUserPlane.airborne) && (theUserPlane.altitude <= 0)) {
			if ( ((theUserPlane.pitch > 10) ||
				  (theUserPlane.pitch < -10)) ||
				 ((theUserPlane.roll > 10) ||
				  (theUserPlane.roll < -10)) )
			{
(*******************************************************************
 Show crash is causeing the garbage to come up on my screen

 I did not have enough time to figure out why.	When you
 get me the next beta of the fsim if the problem still only occurs
 here I'll spend more time looking for it.
*******************************************************************)
				//ShowCrash();							// viewcntl.cpp


				theUserPlane.ResetACState();		// aircraft.cpp


(*******************************************************************
 this used to be SoundCheck()
*******************************************************************)
				SoundCrash(theUserPlane); 	// sndcntl.cpp

				delay(2000);

(*******************************************************************
 The following routine never wait for a keypress they just fall
 through.
*******************************************************************)
				while( !steward.AnyPress() );    // input.cpp

			}
			else
				theUserPlane.LandAC();    // aircraft.cpp
		}
	}
}



(*
 . input.cpp
*)

(*******************************************************************
 The following routine never wait for a keypress they just fall
 through.  I did not have enough time to figure out why.	When you
 get me the next beta of the fsim if the problem still only occurs
 here I'll spend more time looking for it.
*******************************************************************)
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
