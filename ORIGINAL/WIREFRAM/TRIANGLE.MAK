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
LLATDOS_triangledexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_triangledexe = 
BLATDOS_triangledexe = 
CNIEAT_triangledexe = -IC:\BC45\INCLUDE -D
LNIEAT_triangledexe = -x
LEAT_triangledexe = $(LLATDOS_triangledexe)
REAT_triangledexe = $(RLATDOS_triangledexe)
BEAT_triangledexe = $(BLATDOS_triangledexe)

#
# Dependency List
#
Dep_triangle = \
   triangle.exe

triangle : BccDos.cfg $(Dep_triangle)
  echo MakeNode 

Dep_triangledexe = \
   triangle.obj\
   screen.obj\
   bresnham.obj

triangle.exe : $(Dep_triangledexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_triangledexe) $(LNIEAT_triangledexe) +
C:\BC45\LIB\c0h.obj+
triangle.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathh.lib+
C:\BC45\LIB\ch.lib

|

triangle.obj :  triangle.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_triangledexe) $(CNIEAT_triangledexe) -o$@ triangle.cpp
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
-H=triangle.csm
-R-
-i32
-N
-r-
-vi-
-H-
-mh
-f
| $@


