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
LLATDOS_textdemodexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_textdemodexe = 
BLATDOS_textdemodexe = 
CNIEAT_textdemodexe = -IC:\BC45\INCLUDE -DLIGHTING_DEMO
LNIEAT_textdemodexe = -s
LEAT_textdemodexe = $(LLATDOS_textdemodexe)
REAT_textdemodexe = $(RLATDOS_textdemodexe)
BEAT_textdemodexe = $(BLATDOS_textdemodexe)

#
# Dependency List
#
Dep_textdemo = \
   textdemo.exe

textdemo : BccDos.cfg $(Dep_textdemo)
  echo MakeNode 

Dep_textdemodexe = \
   textdemo.obj\
   texture.obj\
	pcx.obj\
	loadp_tx.obj\
   world_tx.obj\
   palette.obj\
   poly_tx.obj\
   drawpolt.obj\
   screen.obj\
   fix.obj

textdemo.exe : $(Dep_textdemodexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_textdemodexe) $(LNIEAT_textdemodexe) +
C:\BC45\LIB\c0l.obj+
textdemo.obj+
texture.obj+
pcx.obj+
loadp_tx.obj+
world_tx.obj+
palette.obj+
poly_tx.obj+
drawpolt.obj+
screen.obj+
fix.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

textdemo.obj :  textdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ textdemo.cpp
|

texture.obj :  texture.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ texture.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ pcx.cpp
|

loadp_tx.obj :  loadp_tx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ loadp_tx.cpp
|

world_tx.obj :  world_tx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ world_tx.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ palette.cpp
|

poly_tx.obj :  poly_tx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ poly_tx.cpp
|

drawpolt.obj :  drawpolt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ drawpolt.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\TEXTURE\screen.asm ,screen.obj
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_textdemodexe) $(CNIEAT_textdemodexe) -o$@ fix.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=textdemo.csm
-R-
-i32
-b-
-vi-
-H-
-ml
-f
| $@


