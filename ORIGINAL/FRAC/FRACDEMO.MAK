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
LLATDOS_fracdemodexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_fracdemodexe = 
BLATDOS_fracdemodexe = 
CNIEAT_fracdemodexe = -IC:\BC45\INCLUDE -DLIGHTING_DEMO
LNIEAT_fracdemodexe = -s
LEAT_fracdemodexe = $(LLATDOS_fracdemodexe)
REAT_fracdemodexe = $(RLATDOS_fracdemodexe)
BEAT_fracdemodexe = $(BLATDOS_fracdemodexe)

#
# Dependency List
#
Dep_fracdemo = \
   fracdemo.exe

fracdemo : BccDos.cfg $(Dep_fracdemo)
  echo MakeNode 

Dep_fracdemodexe = \
   fracdemo.obj\
   poly_fr.obj\
   world_fr.obj\
   palette.obj\
   drawpol1.obj\
   loadp_fr.obj\
   screen.obj\
   fix.obj\
   pcx.obj \
   polylist.obj

fracdemo.exe : $(Dep_fracdemodexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_fracdemodexe) $(LNIEAT_fracdemodexe) +
C:\BC45\LIB\c0l.obj+
fracdemo.obj+
poly_fr.obj+
world_fr.obj+
palette.obj+
drawpol1.obj+
loadp_fr.obj+
screen.obj+
fix.obj+
pcx.obj+
polylist.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

fracdemo.obj :  fracdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ fracdemo.cpp
|

poly_fr.obj :  poly_fr.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ poly_fr.cpp
|

world_fr.obj :  world_fr.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ world_fr.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ palette.cpp
|

drawpol1.obj :  drawpol1.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ drawpol1.cpp
|

loadp_fr.obj :  loadp_fr.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ loadp_fr.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\FRAC\screen.asm ,screen.obj
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ fix.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ pcx.cpp
|

polylist.obj :  polylist.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fracdemodexe) $(CNIEAT_fracdemodexe) -o$@ polylist.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=fracdemo.csm
-R-
-i32
-b-
-vi-
-H-
-ml
-f
| $@


