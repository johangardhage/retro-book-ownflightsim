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
LLATDOS_mountaindexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_mountaindexe = 
BLATDOS_mountaindexe = 
CNIEAT_mountaindexe = -IC:\BC45\INCLUDE -D
LNIEAT_mountaindexe = -x
LEAT_mountaindexe = $(LLATDOS_mountaindexe)
REAT_mountaindexe = $(RLATDOS_mountaindexe)
BEAT_mountaindexe = $(BLATDOS_mountaindexe)

#
# Dependency List
#
Dep_mountain = \
   mountain.exe

mountain : BccDos.cfg $(Dep_mountain)
  echo MakeNode 

Dep_mountaindexe = \
   mountain.obj\
   screen.obj\
   bresnham.obj

mountain.exe : $(Dep_mountaindexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_mountaindexe) $(LNIEAT_mountaindexe) +
C:\BC45\LIB\c0l.obj+
mountain.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

mountain.obj :  mountain.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_mountaindexe) $(CNIEAT_mountaindexe) -o$@ mountain.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\MOUNTAIN\screen.asm ,screen.obj
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\MOUNTAIN\bresnham.asm ,bresnham.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=mountain.csm
-R-
-i32
-vi-
-H-
-ml
-f
| $@


