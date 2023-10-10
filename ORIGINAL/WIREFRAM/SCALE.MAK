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
LLATDOS_scaledexe =  -c -Tde
RLATDOS_scaledexe = 
BLATDOS_scaledexe = 
CNIEAT_scaledexe = -IC:\BC45\INCLUDE -D
LNIEAT_scaledexe = -x
LEAT_scaledexe = $(LLATDOS_scaledexe)
REAT_scaledexe = $(RLATDOS_scaledexe)
BEAT_scaledexe = $(BLATDOS_scaledexe)

#
# Dependency List
#
Dep_scale = \
   scale.exe

scale : BccDos.cfg $(Dep_scale)
  echo MakeNode 

Dep_scaledexe = \
   scale.obj\
   screen.obj\
   bresnham.obj

scale.exe : $(Dep_scaledexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_scaledexe) $(LNIEAT_scaledexe) +
C:\BC45\LIB\c0l.obj+
scale.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

scale.obj :  scale.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scaledexe) $(CNIEAT_scaledexe) -o$@ scale.cpp
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


