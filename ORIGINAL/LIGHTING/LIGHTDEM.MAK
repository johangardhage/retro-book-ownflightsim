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
LLATDOS_lightdemdexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_lightdemdexe = 
BLATDOS_lightdemdexe = 
CNIEAT_lightdemdexe = -IC:\BC45\INCLUDE -DLIGHTING_DEMO
LNIEAT_lightdemdexe = -s
LEAT_lightdemdexe = $(LLATDOS_lightdemdexe)
REAT_lightdemdexe = $(RLATDOS_lightdemdexe)
BEAT_lightdemdexe = $(BLATDOS_lightdemdexe)

#
# Dependency List
#
Dep_lightdem = \
   lightdem.exe

lightdem : BccDos.cfg $(Dep_lightdem)
  echo MakeNode 

Dep_lightdemdexe = \
   lightdem.obj\
   poly_lt.obj\
   world.obj\
   palette.obj\
   loadp_lt.obj\
   drawpol1.obj\
   screen.obj\
   fix.obj

lightdem.exe : $(Dep_lightdemdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_lightdemdexe) $(LNIEAT_lightdemdexe) +
C:\BC45\LIB\c0l.obj+
lightdem.obj+
poly_lt.obj+
world.obj+
palette.obj+
loadp_lt.obj+
drawpol1.obj+
screen.obj+
fix.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

lightdem.obj :  lightdem.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ lightdem.cpp
|

poly_lt.obj :  poly_lt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ poly_lt.cpp
|

world.obj :  world.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ world.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ palette.cpp
|

loadp_lt.obj :  loadp_lt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ loadp_lt.cpp
|

drawpol1.obj :  drawpol1.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ drawpol1.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\LIGHTING\screen.asm ,screen.obj
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_lightdemdexe) $(CNIEAT_lightdemdexe) -o$@ fix.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=lightdem.csm
-R-
-i32
-b-
-vi-
-H-
-ml
-f
| $@


