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
LLATDOS_pcxviewdexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_pcxviewdexe = 
BLATDOS_pcxviewdexe = 
CNIEAT_pcxviewdexe = -IC:\BC45\INCLUDE; -D
LNIEAT_pcxviewdexe = -x
LEAT_pcxviewdexe = $(LLATDOS_pcxviewdexe)
REAT_pcxviewdexe = $(RLATDOS_pcxviewdexe)
BEAT_pcxviewdexe = $(BLATDOS_pcxviewdexe)

#
# Dependency List
#
Dep_pcxview = \
   pcxview.exe

pcxview : BccDos.cfg $(Dep_pcxview)
  echo MakeNode 

Dep_pcxviewdexe = \
   pcxview.obj\
   pcx.obj\
   screen.obj

pcxview.exe : $(Dep_pcxviewdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_pcxviewdexe) $(LNIEAT_pcxviewdexe) +
C:\BC45\LIB\c0l.obj+
pcxview.obj+
pcx.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

pcxview.obj :  pcxview.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_pcxviewdexe) $(CNIEAT_pcxviewdexe) -o$@ pcxview.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_pcxviewdexe) $(CNIEAT_pcxviewdexe) -o$@ pcx.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\GENERAL\screen.asm ,screen.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=pcxview.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


