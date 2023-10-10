
//-------+---------+---------+---------+---------+---------+---------+---------+
// This function is called once when the joystick system is initialized. It
// tests all four possible axes and returns the number of sticks found. The
// axes are tested by sending a 0 byte to the game port, which sets the axis
// bits to 1. The port is then polled in a loop. If the loop times out before
// the bit resets there is no pot on the other end, or there is a hardware
// error. If no active axis is found the stick is assumed not to exist.   
//-------+---------+---------+---------+---------+---------+---------+---------+
#include <dos.h>
const unsigned int gamePort=0x201; // joystick port
int TestSticks()
{
	unsigned int ii;

	int activeAxisMask = 0;

	// test stick 1
	outportb(gamePort, 0);
	for(ii = 0; ii < 65535U; ii++)
		if((inportb(gamePort) & 1) == 0)
			break;
	if(ii != 65535U)
		activeAxisMask = (activeAxisMask | 1);

	outportb(gamePort, 0);
	for(ii = 0; ii < 65535U; ii++)
		if((inportb(gamePort) & 2) == 0)
			break;
	if(ii != 65535U)
		activeAxisMask = (activeAxisMask | 2);

	return (activeAxisMask !=0);
}
