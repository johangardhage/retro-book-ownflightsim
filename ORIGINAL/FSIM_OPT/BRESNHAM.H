// BRESNHAM.H
// include file containing prototype for assembler implementation of
// Bresenham's line drawing algorithm in BRESNHAM.ASM

#ifndef BRESNHAM_H
#define BRESNHAM_H

extern "C" {
	void linedraw(int,int,int,int,int,char far *);
};

#endif