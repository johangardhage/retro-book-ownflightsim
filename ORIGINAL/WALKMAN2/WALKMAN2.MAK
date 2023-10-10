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
LLATDOS_walkman2dexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_walkman2dexe = 
BLATDOS_walkman2dexe = 
CNIEAT_walkman2dexe = -IC:\BC45\INCLUDE; -D
LNIEAT_walkman2dexe = -x
LEAT_walkman2dexe = $(LLATDOS_walkman2dexe)
REAT_walkman2dexe = $(RLATDOS_walkman2dexe)
BEAT_walkman2dexe = $(BLATDOS_walkman2dexe)

#
# Dependency List
#
Dep_walkman2 = \
   walkman2.exe

walkman2 : BccDos.cfg $(Dep_walkman2)
  echo MakeNode 

Dep_walkman2dexe = \
   walkman2.obj\
   pcx.obj\
   sprite.obj\
   screen.obj\
   mio.obj\
   evntmngr.obj

walkman2.exe : $(Dep_walkman2dexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_walkman2dexe) $(LNIEAT_walkman2dexe) +
C:\BC45\LIB\c0h.obj+
walkman2.obj+
pcx.obj+
sprite.obj+
screen.obj+
mio.obj+
evntmngr.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathh.lib+
C:\BC45\LIB\ch.lib

|

walkman2.obj :  walkman2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkman2dexe) $(CNIEAT_walkman2dexe) -o$@ walkman2.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkman2dexe) $(CNIEAT_walkman2dexe) -o$@ pcx.cpp
|

sprite.obj :  sprite.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkman2dexe) $(CNIEAT_walkman2dexe) -o$@ sprite.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WALKMAN2\screen.asm ,screen.obj
|

mio.obj :  mio.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WALKMAN2\mio.asm ,mio.obj
|

evntmngr.obj :  evntmngr.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkman2dexe) $(CNIEAT_walkman2dexe) -o$@ evntmngr.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=walkman2.csm
-R-
-i32
-N
-r-
-vi-
-H-
-mh
-f
| $@


