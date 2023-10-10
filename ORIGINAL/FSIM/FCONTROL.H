#ifndef FCONTROL_H
#define FCONTROL_H

#include "evntmgr2.h"
#include "mio2.h"

class FControlManager : private EventManager
{
	private:
		boolean usingStick;       // true if joystick in use
		boolean usingMouse;
		int stickX;
		int stickY;
		int stick_sens;  	// stick sensitivity 
		int rudder_sens;	// rudder sensitivity
		int rudderPos;    // current position of rudder
	protected:
		void CalcStickControls( AirCraft& AC );
		void CalcKeyControls(AirCraft& AC);
		void CalcStndControls( AirCraft& AC );
		void CalcMouseControls( AirCraft &AC);
		void CheckSndViewControls( AirCraft& AC );
		boolean YesNo(char *st);
		byte ReadStickPosit(byte axisNum);

	public:
		FControlManager();
		int InitControls();
		void Calibrate();
		inline boolean Button1()
			{	return readjbutton(JBUTTON1)?true:false; }
		inline boolean Button2()
			{	return readjbutton(JBUTTON2)?true:false; }
		inline boolean EitherButton()
			{		return readjbutton(JBUTTON1|JBUTTON2)?true:false; }
#ifdef NOTUSED
		inline int ReadStickPosit(int ax)
		{ return readstick(ax); }
#else
		byte ReadStickPosit(int ax);
#endif
		inline int ReadJButton(int x=1)
			{ return readjbutton((x==1)?JBUTTON1:JBUTTON2); }
		inline int ReadStickX()
			{ return readstick(JOY_X); }
		inline int ReadStickY()
			{ return readstick(JOY_Y); }
		void ResetControls();

		void GetControls( AirCraft& tSV );
			// has a keyboard key been pressed?
		boolean KeyPressed();
			// should the joystick be used?
		boolean UseStick();
			// should the mouse be used?
		boolean UseMouse();
			// request to end program?
		boolean Exit();
			// any button or key pressed?
		boolean AnyPress();
		~FControlManager();

};


#endif