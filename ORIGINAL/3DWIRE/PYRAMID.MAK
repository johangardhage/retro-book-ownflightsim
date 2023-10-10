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
LLATDOS_pyramiddexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_pyramiddexe = 
BLATDOS_pyramiddexe = 
CNIEAT_pyramiddexe = -IC:\BC45\INCLUDE; -D
LNIEAT_pyramiddexe = -x
LEAT_pyramiddexe = $(LLATDOS_pyramiddexe)
REAT_pyramiddexe = $(RLATDOS_pyramiddexe)
BEAT_pyramiddexe = $(BLATDOS_pyramiddexe)

#
# Dependency List
#
Dep_pyramid = \
   pyramid.exe

pyramid : BccDos.cfg $(Dep_pyramid)
  echo MakeNode 

Dep_pyramiddexe = \
   pyramid.obj\
   wire.obj\
   bresnham.obj\
   screen.obj

pyramid.exe : $(Dep_pyramiddexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_pyramiddexe) $(LNIEAT_pyramiddexe) +
C:\BC45\LIB\c0l.obj+
pyramid.obj+
wire.obj+
bresnham.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

pyramid.obj :  pyramid.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_pyramiddexe) $(CNIEAT_pyramiddexe) -o$@ pyramid.cpp
|

wire.obj :  wire.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_pyramiddexe) $(CNIEAT_pyramiddexe) -o$@ wire.cpp
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
-H=pyramid.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


