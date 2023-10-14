//
// fof.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrogfx.h"
#include "lib/retroconio.h"
#include "fix.h"
#include "screen.h"
#include "aircraft.h"
#include "evntmgr2.h"
#include "fcontrol.h"
#include "pcx.h"
#include "gauges.h"
#include "loadpoly.h"
#include "poly_fof.h"
#include "palette.h"
#include "world_fof.h"
#include "polylist.h"
#include "view.h"
#include "viewcntl.h"

AirCraft theUserPlane;      // one AirCraft object
FControlManager steward;     // one flight steward (event manager)

// This block declares world, view, and image objects

Pcx bkground;        // Class instance for bkground image

int checkpt = 0;              // Tracks program progress for use in shutdown

const int MAX_ARGS = 1;           // maximum number of cl parameters

const int MAJ_VER = 2;            // major and minor version numbers
const int MIN_VER = 1;
const int VER_LET = 0;            // letter, if any, to follow minor version num

// called when program ends, or if an error occurs during program execution.
// the systems which are shutdown depend on the value of chekpt, which is
// incremented as the system is set up at program start.
void ShutDown()
{
	if (checkpt >= 2) {
		ViewShutDown();         // viewcntl.cpp
	}
	if (checkpt >= 3) {
		theUserPlane.Shutdown();           // aircraft.cpp
	}
}

// this function provides a dump of the aircraft state vector values (see the
// struct definition in AIRCRAFT.H). If debug is true it is output
// with the proper header and footer for a realtime dump. If debug is false
// it is formatted to be output at the end of the program.
void VectorDump()
{
	int i;

	state_vect tSV;
	tSV = (state_vect)theUserPlane;

	gotoxy(3, 1);
	cprintf("State vector realtime dump:");
	i = 1;
	ViewParamDump(35, i);
	theUserPlane.ACDump(35, i);
	i = 3;
	gotoxy(3, i++);
	cprintf("right aileron: %i       ", -tSV.aileron_pos);
	gotoxy(3, i++);
	cprintf("left aileron:  %i       ", tSV.aileron_pos);
	gotoxy(3, i++);
	cprintf("elevator:      %i       ", tSV.elevator_pos);
	gotoxy(3, i++);
	cprintf("rudder:        %i       ", tSV.rudder_pos);
	gotoxy(3, i++);
	cprintf("throttle:      %i       ", tSV.throttle_pos);
	gotoxy(3, i++);
	cprintf("ignition:      %u       ", tSV.ignition_on);
	gotoxy(3, i++);
	cprintf("engine on:     %i       ", tSV.engine_on);
	gotoxy(3, i++);
	cprintf("prop rpm:      %i       ", tSV.rpm);
	gotoxy(3, i++);
	cprintf("fuel level:    %i       ", tSV.fuel);
	gotoxy(3, i++);
	cprintf("x coordinate:  %i       ", tSV.x_pos);
	gotoxy(3, i++);
	cprintf("y coordinate:  %i       ", tSV.y_pos);
	gotoxy(3, i++);
	cprintf("z coordinate:  %i       ", tSV.z_pos);
	gotoxy(3, i++);
	cprintf("pitch:         %li      ", tSV.pitch);
	gotoxy(3, i++);
	cprintf("eff pitch(Rad):%f       ", tSV.efAOF);
	gotoxy(3, i++);
	cprintf("roll:          %li      ", tSV.roll);
	gotoxy(3, i++);
	cprintf("yaw:           %li      ", tSV.yaw);
	gotoxy(3, i++);
	cprintf("speed (H):     %f       ", tSV.h_speed);
	gotoxy(3, i++);
	cprintf("speed (V):     %f       ", tSV.v_speed);
	gotoxy(3, i++);
	cprintf("rate of climb: %f       ", tSV.climbRate);
	gotoxy(3, i++);
	cprintf("altitude:      %i       ", tSV.altitude);
}

// reports program status at termination. Based on the value of checkpt it
// knows whether this is an abnormal term, and prints the leadin accordingly
void Terminate(const char *msg, const char *loc)
{
	int exit_code;

	ShutDown();
	cprintf("FSIM.EXE ==>\r\n");
	if (checkpt < 4) {
		cprintf("A critical error occured in function ");
		cprintf("%s\r\n", loc);
		cprintf("Error: ");
		cprintf("%s, causing controlled termination\r\n", msg);
		exit_code = 1;
	} else {
		cprintf("%s in ", msg);
		cprintf("%s\r\n", loc);
		exit_code = 0;
	}
	if (theUserPlane.opMode != DEBUG) {
		ReportFrameRate();
	}
	exit(exit_code);
}

// displays the title screen
bool DoTitleScreen()
{
	bool result = true;

	if (bkground.Load("assets/title.pcx")) {
		result = false;
	}
	if (result) {
		putwindow(0, 0, 319, 199, bkground.Image());
		fadepalin(0, 256, bkground.Palette());

		delay(1000);

		fadepalout(0, 256, bkground.Palette());
		SetGfxBuffer(0);
		ClearScr(0);
	}
	return(result);
}

// called from main() at program startup to initialize the control, view,
// and flight model systems
void StartUp()
{
	steward.InitControls();		// input.cpp: initialize controls
	checkpt = 1;
	SetGfxBuffer(0);
	if ((theUserPlane.opMode == FLIGHT) || (theUserPlane.opMode == WALK)) {  // if not debugging or walking...
		ClrPalette(0, 256);             // screenc.cpp: clear the palette
		if (theUserPlane.opMode == FLIGHT) {
			if (!DoTitleScreen()) {       // display the title
				Terminate("error loading title image", "DoTitleScreen()");
			}
		}
	}

	if (!InitView(&bkground)) {
		Terminate("Graphics/View system init failed", "main()");
	}

	checkpt = 2;                         // update progress flag
	if (!theUserPlane.InitAircraft()) {
		Terminate("Aircraft initialization failed", "main()");
	}
	checkpt = 3;                        // update progress flag
}

// display control help
void DisplayHelp()
{
	clrscr();
	gotoxy(1, 1);
	cprintf("        The Waite Group's 'Flights of Fantasy' (c) 1992,1995\r\n");
	cprintf("----------------------------------------------------------------\r\n");
//	cprintf("* cmd line args:     H, h or ? - display this help screen\r\n");
//	cprintf("                     D or d    - enable debugging dump mode\r\n");
//	cprintf("                     W or w    - enable world traverse mode\r\n");
//	cprintf("                     V or v    - diplay program version\r\n");
//	cprintf("\r\n");
	cprintf("* view control keys: F1        - look forward\r\n");
	cprintf("                     F2        - look right\r\n");
	cprintf("                     F3        - look behind\r\n");
	cprintf("                     F4        - look left\r\n");
	cprintf("\r\n");
	cprintf("* engine control:    I or i             - toggle ignition/engine on/off\r\n");
	cprintf("                     PageUp/PageDown    - increase/decrease throttle setting\r\n");
	cprintf("\r\n");
//	cprintf("* sound control:     S or s - toggle sound on/off\r\n");
//	cprintf("\r\n");
	cprintf("* aircraft control:  pitch up   - stick back, or down arrow\r\n");
	cprintf("                     pitch down - stick forwardd, or up arrow\r\n");
	cprintf("                     left roll  - stick left, or left arrow\r\n");
	cprintf("                     right roll - stick right, or right arrow\r\n");
	cprintf("                     rudder     - 'z' or 'x' keys\r\n");
	cprintf("                     brake      - 'b' or 'B'\r\n");
}

// this function parses the command line parameters. Accepted command line
// parameters are:
//                   d, D     :  start FOF in debugging dump mode
//                   h, H, ?  :  display a command list before starting
//                   w, W     :  start FOF in world traverse mode
//                   v, V     :  display program version number
void ParseCLP(int argc, char *argv[])
{
	int i;

	if (argc <= (MAX_ARGS + 1)) {
		for (i = 1; i < argc; i++) {
			if ((*argv[i] == 'd') || (*argv[i] == 'D')) {
				theUserPlane.opMode = DEBUG;
			} else if (*argv[i] == '?') {
				theUserPlane.opMode = HELP;
			} else if ((*argv[i] == 'h') || (*argv[i] == 'H')) {
				theUserPlane.opMode = HELP;
			} else if ((*argv[i] == 'w') || (*argv[i] == 'W')) {
				theUserPlane.opMode = WALK;
			} else if ((*argv[i] == 'v') || (*argv[i] == 'V')) {
				theUserPlane.opMode = VERSION;
			} else {
				Terminate("invalid command line parameter", "ParseCLP()");
			}
		}
	} else if (argc > (MAX_ARGS + 1)) {
		Terminate("extra command line parameter", "ParseCLP()");
	}
}

// handles a ground approach by determining from pitch and roll whether the airplane has landed safely or crashed
void GroundApproach()
{
	// handle approaching the ground
	// REV 2.1 change debug mode to reset aircraft.
	if ((theUserPlane.opMode == FLIGHT) || (theUserPlane.opMode == DEBUG)) {
		if ((theUserPlane.airborne) && (theUserPlane.altitude <= 0)) {
			if (((theUserPlane.pitch > 10) || (theUserPlane.pitch < -10)) || ((theUserPlane.roll > 10) || (theUserPlane.roll < -10))) {
				if (theUserPlane.opMode == DEBUG) {
					gotoxy(3, 1);
					cprintf(" CRRAAASSSSHHHHH!!!!");
				} else {
					ShowCrash();               		// viewcntl.cpp
				}
				theUserPlane.ResetACState();    // aircraft.cpp
				delay(200);
			} else {
				theUserPlane.LandAC();    // aircraft.cpp
			}
		}
	}
}

// this function displays the program version number
void DisplayVersion()
{
	clrscr();
	cprintf("*******************************************************\n");
	cprintf("*                                                     *\n");
	cprintf("*******************************************************\n");
	gotoxy(4, 2);
	cprintf("Build Your Own Flight Sym in C++, Version %i.%i%c", MAJ_VER, MIN_VER, VER_LET);
	gotoxy(4, 6);
}

// program entry point
void DEMO_Render(double deltatime)
{
	steward.GetControls(theUserPlane);       // input.cpp: get control settings
	theUserPlane.RunFModel();                // aircraft.cpp: run flight model
	GroundApproach();
	if (!UpdateView(theUserPlane)) {         // aircraft.cpp: make the next frame
		Terminate("View switch file or memory error", "UpdateView()");
	}
	if (theUserPlane.opMode != DEBUG) {            // if not debugging...
		blitscreen(bkground.Image()); // display the new frame
	} else {                            // else if debugging...
		VectorDump();                 // do the screen dump
	}
}

// program entry point
void DEMO_Initialize(void)
{
	theUserPlane.opMode = FLIGHT;                     // assume normal operating mode
	//	ParseCLP(argc, argv);              // parse command line args
	if (theUserPlane.opMode == HELP) {           // if this is a help run
		DisplayHelp();				 // then display the command
		exit(0);					// list and exit
	}
	if (theUserPlane.opMode == VERSION) {
		DisplayVersion();
		exit(0);
	}
	StartUp();
}

// program entry point
void DEMO_Startup(void)
{
	DisplayHelp();
}
