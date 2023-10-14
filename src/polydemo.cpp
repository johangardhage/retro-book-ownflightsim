//
// polydemo.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"
#include "lib/retrogfx.h"
#include "screen.h"
#include "loadpoly.h"
#include "poly_p.h"

world_type world;

void DEMO_Render(double deltatime)
{
	static float xangle = 0, yangle = 0, zangle = 0;         // X,Y&Z angles to rotate shape
	static float xrot = 0.02, yrot = 0.02, zrot = 0.02;         // X,Y&Z rotation increments
	static int zdistance = 800;
	static float scalefactor = 3;

	xangle += xrot;                           // Increment rotation angles
	yangle += yrot;
	zangle += zrot;

	inittrans();                            // Initialize translations
	scale(scalefactor);                               // Create scaling matrix
	rotate(xangle, yangle, zangle);           // Create rotation matrices
	translate(0, 0, zdistance);                      // Create translation matrix

	world.Draw(RETRO.framebuffer);

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
	// Init palette
	RETRO_SetColor(0, 0, 0, 0);
	for (int i = 1; i < RETRO_COLORS; i++) {
		RETRO_SetColor(i, RANDOM(RETRO_COLORS), RANDOM(RETRO_COLORS), RANDOM(RETRO_COLORS));
	}

	world.loadpoly("assets/cube.txt");
}
