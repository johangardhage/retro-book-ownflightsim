//
// gourdemo.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrogfx.h"
#include "fix.h"
#include "screen.h"
#include "pcx.h"
#include "loadpoly.h"
#include "palette.h"
#include "poly_g.h"
#include "world_g.h"

World world;

void DEMO_Render(double deltatime)
{
	static int xangle = 0, yangle = 0, zangle = 0; // X,Y&Z angles for rotation object
	static int xrot = 2, yrot = 2, zrot = 2;       // X,Y&Z rotations increments
	static int zdistance = 600;
	static int scalefactor = 1;
	static int curobj = 0;

	xangle += xrot;                           // Increment rotation angles
	yangle += yrot;
	zangle += zrot;
	if (xangle > SIN_MASK) xangle = 0;
	if (xangle < 0) xangle = SIN_MASK;
	if (yangle > SIN_MASK) yangle = 0;
	if (yangle < 0) yangle = SIN_MASK;
	if (zangle > SIN_MASK) zangle = 0;
	if (zangle < 0) zangle = SIN_MASK;

	inittrans();                            // Initialize translations
	scale(scalefactor);                     // Create scaling matrix
	rotate(xangle, yangle, zangle);           // Create rotation matrices
	translate(0, 0, zdistance);                     // Create translation matrix

	world.Draw(curobj, RETRO.framebuffer, XORIGIN, YORIGIN);

	if (RETRO_KeyState(SDL_SCANCODE_RETURN)) {
		curobj++;
		if (curobj >= world.GetObjectCount()) {
			curobj = 0;
		}
	}
	if (RETRO_KeyState(SDL_SCANCODE_7)) {
		xrot++;
	}
	if (RETRO_KeyState(SDL_SCANCODE_4)) {
		xrot = 0;
	}
	if (RETRO_KeyState(SDL_SCANCODE_1)) {
		--xrot;
	}
	if (RETRO_KeyState(SDL_SCANCODE_8)) {
		yrot++;
	}
	if (RETRO_KeyState(SDL_SCANCODE_5)) {
		yrot = 0;
	}
	if (RETRO_KeyState(SDL_SCANCODE_2)) {
		--yrot;
	}
	if (RETRO_KeyState(SDL_SCANCODE_9)) {
		zrot++;
	}
	if (RETRO_KeyState(SDL_SCANCODE_6)) {
		zrot = 0;
	}
	if (RETRO_KeyState(SDL_SCANCODE_3)) {
		--zrot;
	}
	if (RETRO_KeyState(SDL_SCANCODE_0)) {
		zrot = xrot = yrot = 0;
		zangle = xangle = yangle = 0;
	}
	if (RETRO_KeyState(SDL_SCANCODE_PAGEUP)) {
		zdistance += 30;
	}
	if (RETRO_KeyState(SDL_SCANCODE_PAGEDOWN)) {
		if (zdistance > 530) {
			zdistance -= 30;
		}
	}
}

void DEMO_Initialize(void)
{
	if (world.LoadPoly("assets/cube.wld") < 1) {  // Load object description(s)
		RETRO_RageQuit("Cannot load %s\n", "assets/cube.wld");
	}

	world.AdjustLighting();                   // adjust the palette by the source
}
