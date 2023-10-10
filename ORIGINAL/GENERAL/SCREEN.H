#ifndef SCREEN_H
#define SCREEN_H

const SCREEN_WIDTH=320,SCREEN_HEIGHT=200;

/* void SetGfxBuffer( void far* theBuffer );
void far *XyToPtr(int x, int y);
void WritePixel(int x, int y, byte color);
void GetImage(int x1, int y1, int x2, int y2, byte *buffer);
void PutImage(int x1, int y1, int x2, int y2, byte *buffer);
void SetGfxBuffer( void far* theBuffer );
void far *XyToPtr(int x, int y);
void WritePixel(int x, int y, byte color);
void GetImage(int x1, int y1, int x2, int y2, byte *buffer);
void PutImage(int x1, int y1, int x2, int y2, byte *buffer);
void Line(int x1, int y1, int x2, int y2, byte color);
void ReadPalette(int start, int number, byte* palette);
void ClrPalette(int start, int number);
void FadePaletteIn(int start, int count, const byte* palette);
void FadePaletteOut(int start, int count);
void WaitVbi();
void ClearScr(byte color);
void BarFill(int x1, int y1, int x2, int y2, byte color);
void Line(int x1, int y1, int x2, int y2, byte color);
void ReadPalette(int start, int number, byte* palette);
void ClrPalette(int start, int number);
void FadePaletteIn(int start, int count, const byte* palette);
void FadePaletteOut(int start, int count);
void WaitVbi();
void ClearScr(byte color);
void BarFill(int x1, int y1, int x2, int y2, byte color);
word BufSize( int x1, int y1, int x2, int y2 ); */

extern "C" {
	void cls(char far *screen_adr);
	void setgmode(int mode);
	void setpalette(char far *color_regs);
	void putwindow(int,int,int,int,char far *);
	void clrwin(int,int,int,int,char far *);
    void blitscreen( void far* buffer );
    void putimage( int x1, int y1, int xdim, int ydim, void far* sbuf,
                   void far* dbuf);
    void transput( int x1, int y1, int x2, int y2, void far* sbuffer,
									 void far* dbuffer );
		void ctransput(void far *sbuffer, void far *dbuffer);
}

#endif