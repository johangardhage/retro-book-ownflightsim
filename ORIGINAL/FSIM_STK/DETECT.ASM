; -------+---------+---------+---------+---------+---------+---------+---------+
;  Copyright (c) 1991-1992 Betz Associates. All rights reserved.
;
;  File Name: DETECT.ASM
;  Project:   Flights of Fantasy
;  Creation:  August 10, 1992
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
;       8-29-1992       1.1b    MB          first beta
;       9-26-1992       1.1     MB          publication release
;
;  Description
;  -----------
;       Detects 386 or better CPU
;
; -------+---------+---------+---------+---------+---------+---------+---------+

	   .MODEL  large
	   .CODE
	   PUBLIC  _detect386

_detect386    PROC
       push   bp
       mov    bp, sp
       xor    ax, ax
       push   ax
       popf
       pushf
       pop    ax
       and    ax, 0f000h
       cmp    ax, 0f000h
       je     no_386

       mov    ax, 07000h
       push   ax
       popf
       pushf
       pop    ax
       and    ax, 07000h
       je     no_386
       mov    ax, 1
       jmp    exit
no_386:
       xor    ax, ax
exit:
       pop    bp
       ret
_detect386    ENDP

       END