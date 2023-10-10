#ifndef SCREEN_H

#define SCREEN_H

const SCREEN_WIDTH=320,SCREEN_HEIGHT=200;

extern "C" {
	void cls(char far *screen_adr);
	void setgmode(int mode);
	void setpalette(char far *color_regs);
	void clrwin(int,int,int,int,char far *);
	void putwindow(int xpos,int ypos,int xize,int ysize,
		unsigned int offset,unsigned int segment);
};

#endif
