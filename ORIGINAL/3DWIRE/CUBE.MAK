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
LLATDOS_cubedexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_cubedexe = 
BLATDOS_cubedexe = 
CNIEAT_cubedexe = -IC:\BC45\INCLUDE; -D
LNIEAT_cubedexe = -x
LEAT_cubedexe = $(LLATDOS_cubedexe)
REAT_cubedexe = $(RLATDOS_cubedexe)
BEAT_cubedexe = $(BLATDOS_cubedexe)

#
# Dependency List
#
Dep_cube = \
   cube.exe

cube : BccDos.cfg $(Dep_cube)
  echo MakeNode 

Dep_cubedexe = \
   cube.obj\
   wire.obj\
   bresnham.obj\
   screen.obj

cube.exe : $(Dep_cubedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_cubedexe) $(LNIEAT_cubedexe) +
C:\BC45\LIB\c0l.obj+
cube.obj+
wire.obj+
bresnham.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

cube.obj :  cube.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_cubedexe) $(CNIEAT_cubedexe) -o$@ cube.cpp
|

wire.obj :  wire.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_cubedexe) $(CNIEAT_cubedexe) -o$@ wire.cpp
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
-H=cube.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


