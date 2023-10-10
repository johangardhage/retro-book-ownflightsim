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
LLATDOS_waitedexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_waitedexe = 
BLATDOS_waitedexe = 
CNIEAT_waitedexe = -IC:\BC45\INCLUDE; -D
LNIEAT_waitedexe = -x
LEAT_waitedexe = $(LLATDOS_waitedexe)
REAT_waitedexe = $(RLATDOS_waitedexe)
BEAT_waitedexe = $(BLATDOS_waitedexe)

#
# Dependency List
#
Dep_waite = \
   waite.exe

waite : BccDos.cfg $(Dep_waite)
  echo MakeNode 

Dep_waitedexe = \
   waite.obj\
   wire.obj\
   bresnham.obj\
   screen.obj

waite.exe : $(Dep_waitedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_waitedexe) $(LNIEAT_waitedexe) +
C:\BC45\LIB\c0l.obj+
waite.obj+
wire.obj+
bresnham.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

waite.obj :  waite.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_waitedexe) $(CNIEAT_waitedexe) -o$@ waite.cpp
|

wire.obj :  wire.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_waitedexe) $(CNIEAT_waitedexe) -o$@ wire.cpp
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\3DWIRE\bresnham.asm ,bresnham.obj
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\3DWIRE\screen.asm ,screen.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=waite.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


