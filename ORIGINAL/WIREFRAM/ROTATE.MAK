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
LLATDOS_rotatedexe =  -c -Tde
RLATDOS_rotatedexe = 
BLATDOS_rotatedexe = 
CNIEAT_rotatedexe = -IC:\BC45\INCLUDE -D
LNIEAT_rotatedexe = -x
LEAT_rotatedexe = $(LLATDOS_rotatedexe)
REAT_rotatedexe = $(RLATDOS_rotatedexe)
BEAT_rotatedexe = $(BLATDOS_rotatedexe)

#
# Dependency List
#
Dep_rotate = \
   rotate.exe

rotate : BccDos.cfg $(Dep_rotate)
  echo MakeNode 

Dep_rotatedexe = \
   rotate.obj\
   screen.obj\
   bresnham.obj

rotate.exe : $(Dep_rotatedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_rotatedexe) $(LNIEAT_rotatedexe) +
C:\BC45\LIB\c0l.obj+
rotate.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

rotate.obj :  rotate.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_rotatedexe) $(CNIEAT_rotatedexe) -o$@ rotate.cpp
|

screen.obj :  screen.asm
  $(ThSM) @&&|
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
-H=shape.csm
-ml
-f
| $@


