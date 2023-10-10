
; SCREEN.ASM
;
; A set of assembly language functions for
; accessing the mode 13h display
;
; Copyright 1993 by Christopher Lampton and
; The Waite Group Press

		.MODEL	large
		.CODE
;		.386
		PUBLIC	_cls,_setgmode,_setpalette

SCREEN_WIDTH	EQU	320
SCREEN_HEIGHT	EQU 200

; cls(char far *screen_adr)
;   Clear video buffer at offset, segment

_cls			PROC
		ARG		scr_off:WORD, scr_seg:WORD
		push	bp
		mov		bp,sp
		push	di
		mov		di,scr_seg
		mov		es,di
		mov		di,scr_off
		mov		cx,SCREEN_WIDTH/2 * SCREEN_HEIGHT
		mov		ax,0
		rep		stosw
		pop		di
		pop		bp
		ret
_cls			ENDP

_setgmode	PROC
		ARG		mode:WORD
		push	bp
		mov		bp,sp
		mov		ax,[mode]
		mov		ah,0
		int		10h
		pop		bp
		ret
_setgmode	ENDP

; setpalette(char far *color_regs)
;   Set VGA color registers to the color values in COLOR_REGS

_setpalette	PROC
		ARG   regoff:WORD,regseg:WORD
		push	bp
		mov		bp,sp
		mov		dx,es
		push	dx
		mov		dx,regseg
		mov		es,dx
		mov		ah,10h
		mov		al,12h
		mov		bx,0
		mov		cx,100h
		mov		dx,regoff
		int		10h
		pop		dx
		mov		es,dx
		pop		bp
		ret
_setpalette	ENDP

		END
