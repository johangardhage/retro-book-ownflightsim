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
LLATDOS_shapedexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_shapedexe = 
BLATDOS_shapedexe = 
CNIEAT_shapedexe = -IC:\BC45\INCLUDE -D
LNIEAT_shapedexe = -x
LEAT_shapedexe = $(LLATDOS_shapedexe)
REAT_shapedexe = $(RLATDOS_shapedexe)
BEAT_shapedexe = $(BLATDOS_shapedexe)

#
# Dependency List
#
Dep_shape = \
   shape.exe

shape : BccDos.cfg $(Dep_shape)
  echo MakeNode 

Dep_shapedexe = \
   shape.obj\
   bresnham.obj\
   screen.obj

shape.exe : $(Dep_shapedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_shapedexe) $(LNIEAT_shapedexe) +
C:\BC45\LIB\c0l.obj+
shape.obj+
bresnham.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

shape.obj :  shape.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_shapedexe) $(CNIEAT_shapedexe) -o$@ shape.cpp
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\bresnham.asm ,bresnham.obj
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\screen.asm ,screen.obj
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
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


