//
// textdemo.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrogfx.h"
#include "fix.h"
#include "screen.h"
#include "pcx.h"
#include "loadpoly.h"
#include "poly_tx.h"
#include "world_tx.h"

World world;
Pcx texturePcx;

void DEMO_Render(double deltatime)
{
	static int xangle = 0, yangle = 0, zangle = 0; // X,Y&Z angles for rotation  object
	static int xrot = 2, yrot = 2, zrot = 2;       // X,Y&Z rotations increments
	static int zdistance = 800;
	static int scalefactor = 2;
	static int resolution = 30;
	static int curobj = 0;

	// Increment rotation angles
	xangle += xrot;
	yangle += yrot;
	zangle += zrot;

	// Check for 256 degree wrap around:
	if (xangle > SIN_MASK) xangle = 0;
	if (xangle < 0) xangle = SIN_MASK;
	if (yangle > SIN_MASK) yangle = 0;
	if (yangle < 0) yangle = SIN_MASK;
	if (zangle > SIN_MASK) zangle = 0;
	if (zangle < 0) zangle = SIN_MASK;

	inittrans();                            // Initialize translations
	scale(scalefactor);                               // Create scaling matrix
	rotate(xangle, yangle, zangle);           // Create rotation matrices

	translate(0, 0, zdistance);          // Create translation matrix

	RETRO_Blit(texturePcx.Image());

	// Call the Draw world loop
	world.Draw(curobj, RETRO.framebuffer, XORIGIN, YORIGIN, resolution);

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
	if (RETRO_KeyState(SDL_SCANCODE_COMMA)) {
		resolution++;
		if (resolution > 300) {
			resolution = 300;
		}
	}
	if (RETRO_KeyState(SDL_SCANCODE_PERIOD)) {
		resolution--;
		if (resolution < 10) {
			resolution = 10;
		}
	}
}

void DEMO_Initialize(void)
{
	world.LoadPoly("assets/cube.wld");

	texturePcx.Load("assets/textdemo.pcx");

	//Set the objects to the texture :
	for (int oo = 0; oo < world.GetObjectCount(); oo++) {
		(world.GetObjectPtr(oo))->SetTexture(&texturePcx);
	}

	setpalette(texturePcx.Palette());
}
