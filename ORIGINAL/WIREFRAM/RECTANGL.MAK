#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
USER_ROOT = C:\BYOFS
IDE_LFLAGSDOS =  -LC:\BC45\LIB
IDE_BFLAGS = 
LLATDOS_rectangldexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_rectangldexe = 
BLATDOS_rectangldexe = 
CNIEAT_rectangldexe = -IC:\BC45\INCLUDE -D
LNIEAT_rectangldexe = -x
LEAT_rectangldexe = $(LLATDOS_rectangldexe)
REAT_rectangldexe = $(RLATDOS_rectangldexe)
BEAT_rectangldexe = $(BLATDOS_rectangldexe)

#
# Dependency List
#
Dep_rectangl = \
   rectangl.exe

rectangl : BccDos.cfg $(Dep_rectangl)
  echo MakeNode 

Dep_rectangldexe = \
   rectangl.obj\
   screen.obj\
   bresnham.obj

rectangl.exe : $(Dep_rectangldexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_rectangldexe) $(LNIEAT_rectangldexe) +
C:\BC45\LIB\c0h.obj+
rectangl.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathh.lib+
C:\BC45\LIB\ch.lib

|

rectangl.obj :  rectangl.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_rectangldexe) $(CNIEAT_rectangldexe) -o$@ rectangl.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\screen.asm ,screen.obj
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\bresnham.asm ,bresnham.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=rectangl.csm
-R-
-i32
-N
-r-
-vi-
-H-
-mh
-f
| $@


