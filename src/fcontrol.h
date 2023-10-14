#ifndef FCONTROL_H
#define FCONTROL_H

#define MSE_XCENT 320
#define MSE_YCENT 100	

static const int kbd_data = 0x60;            // keyboard data port
static const int kbd_stat = 0x64;            // keyboard status port
static const int kbd_output_full = 1;        // kbd output full mask
static const int gamePort = 0x201;       // joystick port

// control sensitivity in amount of motion per 1/20 second

static unsigned char activeAxisMask;               // mask for live jstick axes

class FControlManager : private EventManager
{
private:
	bool usingStick;       // true if joystick in use
	bool usingMouse;
	int stickX;
	int stickY;
	int stick_sens;  	// stick sensitivity 
	int rudder_sens;	// rudder sensitivity
	int rudderPos;      // current position of rudder

protected:

	// this function is called from GetControls(). It calculates the state of
	// the flight controls that are mapped to the joystick when it is in use.
	// This function is only called when the joystick is in use.
	void CalcStickControls(AirCraft &AC)
	{
		unsigned int tempX, tempY;

		tempX = ReadStickPosit(JOY_X);           // get current x and y axes
		tempY = ReadStickPosit(JOY_Y);           // position

		// map the ideal x,y axis range to reality, then assign the result
		// to stickX and stickY.
		stickX = (tempX * (25500 / (xmax - xmin))) / 100;
		stickY = (tempY * (25500 / (ymax - ymin))) / 100;

		if (stickX > xcent) {                 // map X 0..255 range to
			stickX = (stickX - xcent);       // -128..127 range
		} else {
			stickX = (-(xcent - stickX));
		}

		if (stickY > ycent) {                 // map Y 0..255 range to
			stickY = (stickY - ycent);       // -128..127 range
		} else {
			stickY = (-(ycent - stickY));
		}

		stickX = (-stickX);                      // flip the values so that
		stickY = (-stickY);                      // the control directions make sense

		if (stickX > 127) {
			stickX = 127;          // bounds check the results
		} else if (stickX < -128) {
			stickX = -128;
		}
		if (stickY > 127) {
			stickY = 127;
		} else if (stickY < -128) {
			stickY = -128;
		}

		if (Button1()) {                          // get the buttons states
			AC.button1 = true;                 // and update the state vector
		}
		if (Button2()) {
			AC.button2 = true;
		}
	}

	// This function is called from GetControls(). It checks the state of all
	// the flight control keys, and adjusts the values which would normally be
	// mapped to the joystick if it was in use. GetControls() only calls this
	// function if the joystick is not being used.

	void CalcKeyControls(AirCraft &AC)
	{
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			// adjust the control position
			stickX += stick_sens;
			if (stickX > 127) {
				stickX = 127;
			}
		} else if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
			stickX -= stick_sens;
			if (stickX < -128) {
				stickX = -128;
			}
		} else {
			stickX = 0;
		}
		if (RETRO_KeyState(SDL_SCANCODE_UP)) {
			stickY += stick_sens;
			if (stickY > 127) {
				stickY = 127;
			}
		} else if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
			stickY -= stick_sens;
			if (stickY < -128) {
				stickY = -128;
			}
		} else {
			stickY = 0;
		}
		if (RETRO_KeyPressed(SDL_SCANCODE_RETURN)) {
			AC.button1 = true;
		}
		if (RETRO_KeyPressed(SDL_SCANCODE_TAB)) {
			AC.button2 = true;
		}
	}

	// this function is called from GetControls(). It calculates the state of
	// the standard controls which are never mapped to the joystick. This
	// function is called on every call to GetControls()
	void CalcStndControls(AirCraft &AC)
	{
		if (RETRO_KeyPressed(SDL_SCANCODE_D)) {
//			VectorDump();
		}
		if (RETRO_KeyState(SDL_SCANCODE_X)) {
			rudderPos += rudder_sens;
			if (rudderPos > 127) {
				rudderPos = 127;
			}
		} else if (RETRO_KeyState(SDL_SCANCODE_Z)) {
			rudderPos -= rudder_sens;
			if (rudderPos < -128) {
				rudderPos = -128;
			}
		} else {
			rudderPos = 0;
		}
		if (RETRO_KeyState(SDL_SCANCODE_PAGEUP) && (AC.throttle_pos < 15)) {
			AC.throttle_pos++;
		} else if (RETRO_KeyState(SDL_SCANCODE_PAGEDOWN) && (AC.throttle_pos > 0)) {
			AC.throttle_pos--;
		}
		if (RETRO_KeyPressed(SDL_SCANCODE_I)) {
			if (AC.ignition_on) {
				AC.ignition_on = false;
			} else {
				AC.ignition_on = true;
			}
		}
		if (RETRO_KeyPressed(SDL_SCANCODE_B)) {
			if (AC.brake) {
				AC.brake = false;
			} else {
				AC.brake = true;
			}
		}
	}

	void mousepos(int *x, int *y)
	{
	}

	int readmbutton()
	{
		return 0;
	}

	int joydetect()
	{
		return 0;
	}

	void CalcMouseControls(AirCraft &AC)
	{
		int tempX, tempY;
#ifdef NOTUSED
		relpos(&tempX, &tempY);  	// get current x and y axes position
		tempX /= 2; 				//adjust x position to 320x200
		if (tempX > 0) {
			stickX += stick_sens;
		} else if (tempX < 0) {
			stickX -= stick_sens;
		} else if (stickX != 0) {
			stickX /= 2;
		}

		tempY = -tempY; 		// flip mouse coordinate to match joystick emulation
		if (tempY > 0) {
			stickY += stick_sens;
		} else if (tempY < 0) {
			stickY -= stick_sens;
		} else if (stickY != 0) {
			stickY /= 2;
		}
#else							// REV 2.1 mouse status
		mousepos(&tempX, &tempY);  	// get current x and y axes
		tempX -= MSE_XCENT;			// convert to -320 to 320
		tempY -= MSE_YCENT;         // convert to -100 to 100

		stickX = -((tempX * 2) / 5);  // ratio mouse x coords to stick
		stickY = (tempY * 32) / 25;  	// ratio mouse y coords to stick
#endif
		if (stickX > 127) {
			stickX = 127;          // bounds check the results
		} else if (stickX < -128) {
			stickX = -128;
		}
		if (stickY > 127) {
			stickY = 127;
		} else if (stickY < -128) {
			stickY = -128;
		}
		int mouseButtons = readmbutton();
		if (mouseButtons & MBUTTON1) {                          // get the buttons states
			AC.button1 = true;                 // and update the state vector
		}
		if (mouseButtons & MBUTTON2) {
			AC.button2 = true;
		}
	}

	// this function is called from GetControls(). It checks the state of the
	// sound and view controls, and updates the state vector if required.
	// This function is called on every call to GetControls().
	void CheckSndViewControls(AirCraft &AC)
	{
		if (RETRO_KeyPressed(SDL_SCANCODE_S)) {
			AC.sound_chng = true;
		}
		if (RETRO_KeyPressed(SDL_SCANCODE_F1)) {
			AC.view_state = 0;
		} else if (RETRO_KeyPressed(SDL_SCANCODE_F2)) {
			AC.view_state = 1;
		} else if (RETRO_KeyPressed(SDL_SCANCODE_F3)) {
			AC.view_state = 2;
		} else if (RETRO_KeyPressed(SDL_SCANCODE_F4)) {
			AC.view_state = 3;
		}
	}

	bool YesNo(const char *st)
	{
		printf("%s\n", st);
		char ch = 0;
		while (ch == 0) {
			ch = getch();
			if ((ch == 0x59) || (ch == 0x79)) {
				return(true);
			} else if ((ch == 0x4e) || (ch == 0x6e)) {
				break;
			} else {
				ch = 0;
			}
		}
		return false;
	}

public:
	FControlManager() :EventManager()
	{
		usingMouse = usingStick = false;
		stick_sens = 7;            // arbitrary value for control sensitivity
		rudder_sens = 7;
		activeAxisMask = joydetect();
	}

	~FControlManager()
	{
	}

	// This function is called at program startup to initialize the control
	// input system. Call this function BEFORE putting video system into
	// graphics mode, or you'll get a very ugly screen
	int InitControls()
	{
		if (activeAxisMask != 0) {  // is there at least 1 active joystick?
			if (UseStick()) {       // yes, do they want to use it?
				Calibrate();        // yes, go calibrate it
				usingStick = true;    // set the stick-in-use flag
			}                     // no, don't use the stick
		}                         // no, there is no joystick
		if ((usingStick == false) && hasMouse) {        // if mouse driver is present
			if (UseMouse()) {
				stick_sens = 4; 	// REV 2.1 tweak mouse performance
				usingMouse = true;
			}
		}
		return(1);
	}

	// This function should be called with VGA in text mode! It goes through a
	// joystick calibration process. This could be ammended to write a file
	// to disk, which could be used to define the joystick limits when the
	// program starts up.
	void Calibrate()
	{
		unsigned int tempX, tempY;

		clrscr();

		printf("-----------------  Calibrating Joystick  -----------------\n");
		printf("Move joystick to upper left corner, then press a button...\n");;
		while (!EitherButton()) {
			xmin = ReadStickPosit(JOY_X);
			ymin = ReadStickPosit(JOY_Y);
		}
		delay(50);
		while (EitherButton());
		printf("Move joystick to lower right corner, then press a button..\n");
		while (!EitherButton()) {
			xmax = ReadStickPosit(JOY_X);
			ymax = ReadStickPosit(JOY_Y);
		}
		delay(50);
		while (EitherButton());
		printf("Center the joystick, then press a button...\n");
		while (!EitherButton()) {
			tempX = ReadStickPosit(JOY_X);
			tempY = ReadStickPosit(JOY_Y);
		}
		tempX = tempX * (25500 / (xmax - xmin));
		xcent = tempX / 100;
		tempY = tempY * (25500 / (ymax - ymin));
		ycent = tempY / 100;
		printf("...calibration complete.\n");
	}

	inline bool Button1()
	{
		return readjbutton(JBUTTON1) ? true : false;
	}

	inline bool Button2()
	{
		return readjbutton(JBUTTON2) ? true : false;
	}

	inline bool EitherButton()
	{
		return readjbutton(JBUTTON1 | JBUTTON2) ? true : false;
	}

	inline int ReadStickPosit(int ax)
	{
		return readstick(ax);
	}

	inline int ReadJButton(int x = 1)
	{
		return readjbutton((x == 1) ? JBUTTON1 : JBUTTON2);
	}

	inline int ReadStickX()
	{
		return readstick(JOY_X);
	}

	inline int ReadStickY()
	{
		return readstick(JOY_Y);
	}

	// This function is called from the main program, and is passed a pointer
	// to the aircraft state vector. It updates this vector based on the current
	// state of the control interface.
	void GetControls(AirCraft &AC)
	{
		AC.button1 = false;                // reset both button flags
		AC.button2 = false;
		if (usingStick) {                         // is the joystick in use?
			CalcStickControls(AC);            // yes, grab the control state
		} else if (usingMouse) {
			CalcMouseControls(AC);
		} else {
			CalcKeyControls(AC);                   // no, get the keyboard controls
		}
		CalcStndControls(AC);                 // go get the standard controls

		CheckSndViewControls(AC);
		AC.aileron_pos = stickX;               // update the state vector
		AC.elevator_pos = stickY;              // with values calculated in
		AC.rudder_pos = rudderPos;             // the other functions
		AC.ReduceIndices();                    // remap the deflections
	}

	// should the joystick be used?
	// this function should be called in *TEXT* mode. It asks the user if they
	// wish to use the joystick, and returns true if the answer is yes
	bool UseStick()
	{
		return YesNo("=> A joystick has been detected... do you wish to use it? (Yy/Nn)");
	}

	// should the mouse be used?
	bool UseMouse()
	{
		return YesNo("=> A mouse has been detected... do you wish to use it? (Yy/Nn)");
	}
};

#endif
