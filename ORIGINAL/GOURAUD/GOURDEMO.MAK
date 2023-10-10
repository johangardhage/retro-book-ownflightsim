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
LLATDOS_gourdemodexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_gourdemodexe = 
BLATDOS_gourdemodexe = 
CNIEAT_gourdemodexe = -IC:\BC45\INCLUDE -DLIGHTING_DEMO
LNIEAT_gourdemodexe = -s
LEAT_gourdemodexe = $(LLATDOS_gourdemodexe)
REAT_gourdemodexe = $(RLATDOS_gourdemodexe)
BEAT_gourdemodexe = $(BLATDOS_gourdemodexe)

#
# Dependency List
#
Dep_gourdemo = \
   gourdemo.exe

gourdemo : BccDos.cfg $(Dep_gourdemo)
  echo MakeNode 

Dep_gourdemodexe = \
   gourdemo.obj\
   drawpolg.obj\
   poly_g.obj\
   palette.obj\
   world_g.obj\
   fix.obj\
   loadp_lt.obj\
   screen.obj

gourdemo.exe : $(Dep_gourdemodexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_gourdemodexe) $(LNIEAT_gourdemodexe) +
C:\BC45\LIB\c0l.obj+
gourdemo.obj+
drawpolg.obj+
poly_g.obj+
palette.obj+
world_g.obj+
fix.obj+
loadp_lt.obj+
screen.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

gourdemo.obj :  gourdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ gourdemo.cpp
|

drawpolg.obj :  drawpolg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ drawpolg.cpp
|

poly_g.obj :  poly_g.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ poly_g.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ palette.cpp
|

world_g.obj :  world_g.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ world_g.cpp
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ fix.cpp
|

loadp_lt.obj :  loadp_lt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_gourdemodexe) $(CNIEAT_gourdemodexe) -o$@ loadp_lt.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\GOURAUD\screen.asm ,screen.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=gourdemo.csm
-R-
-i32
-b-
-vi-
-H-
-ml
-f
| $@


