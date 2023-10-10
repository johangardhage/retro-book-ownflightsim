; -------+---------+---------+---------+---------+---------+---------+---------+
;  Copyright (c) 1991-1992 Betz Associates. All rights reserved.
;
;  File Name: SBFM.ASM
;  Project:   Flights of Fantasy
;  Creation:  May 17, 1992
;  Author:    Mark Betz (MB)
;
;  Machine:   IBM PC and Compatibles
;
;  Includes:  nothing
;
; -------+---------+---------+---------+---------+---------+---------+---------+
;  Change History
;  ------ -------
;
;       Date            Rev.	Author		Purpose
;       ----            ----    ------      -------
;       5-17-1992       1.0     MB          initial development
;       8-29-1992       1.1b    MB          first beta
;       9-26-1992       1.1     MB          publication release
;
;  Description
;  -----------
;       SetReg assembler function for communcating with the Sound Blaster
;       FM synthesizer chip
;
; -------+---------+---------+---------+---------+---------+---------+---------+

			.MODEL LARGE

			.CODE

			PUBLIC _SetReg

; extern "C" SetReg( theReg, theValue )
;
;   destroys: nothing
;   on entry: [bp+6] = theReg
;             [bp+8] = theValue
;
;	This function sets the FM chip register specified in theReg, to the value
;   in theValue.

_SetReg		proc far
	push	bp
	mov		bp, sp
	push	ax
	push    cx
	push	dx
	xor		ax, ax
	mov		dx, 0388h                    ; FM Address Register Port
	mov		al, byte ptr [bp + 6]        ; get the register into al
	out 	dx, al                       ; write it to the Address Reg.
	mov		cx, 7                        ; need to wait 3.3 microsecs here...
wait1:                                   ; which we'll do by reading the
	in		al, dx                       ; status reg 7 times
	loop	wait1
	mov		al, byte ptr [bp + 8]        ; get the data byte into al
	inc		dx                           ; point dx to the Data Register Port
	out		dx, al                       ; write the data
	mov		cx, 35                       ; need to wait 23 microsecs here...
wait2:                                   ; read the port 35 times
	in		al, dx
	loop wait2
	pop		dx
	pop		cx
	pop		ax
	pop		bp
	retf
_SetReg		endp

			END
