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
LLATDOS_rotate2dexe =  -c -Tde
RLATDOS_rotate2dexe = 
BLATDOS_rotate2dexe = 
CNIEAT_rotate2dexe = -IC:\BC45\INCLUDE -D
LNIEAT_rotate2dexe = -x
LEAT_rotate2dexe = $(LLATDOS_rotate2dexe)
REAT_rotate2dexe = $(RLATDOS_rotate2dexe)
BEAT_rotate2dexe = $(BLATDOS_rotate2dexe)

#
# Dependency List
#
Dep_rotate2 = \
   rotate2.exe

rotate2 : BccDos.cfg $(Dep_rotate2)
  echo MakeNode 

Dep_rotate2dexe = \
   rotate2.obj\
   screen.obj\
   bresnham.obj

rotate2.exe : $(Dep_rotate2dexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_rotate2dexe) $(LNIEAT_rotate2dexe) +
C:\BC45\LIB\c0l.obj+
rotate2.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

rotate2.obj :  rotate2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_rotate2dexe) $(CNIEAT_rotate2dexe) -o$@ rotate2.cpp
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
-H=shape.csm
-ml
-f
| $@


