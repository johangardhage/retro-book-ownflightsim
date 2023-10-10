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
LLATDOS_rainbowdexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_rainbowdexe = 
BLATDOS_rainbowdexe = 
CNIEAT_rainbowdexe = -IC:\BC45\INCLUDE; -D
LNIEAT_rainbowdexe = -x
LEAT_rainbowdexe = $(LLATDOS_rainbowdexe)
REAT_rainbowdexe = $(RLATDOS_rainbowdexe)
BEAT_rainbowdexe = $(BLATDOS_rainbowdexe)
CLATDOS_rainbowdcpp = 
LLATDOS_rainbowdcpp = 
RLATDOS_rainbowdcpp = 
BLATDOS_rainbowdcpp = 
CEAT_rainbowdcpp = $(CEAT_rainbowdexe) $(CLATDOS_rainbowdcpp)
CNIEAT_rainbowdcpp = -IC:\BC45\INCLUDE; -D
LNIEAT_rainbowdcpp = -x
LEAT_rainbowdcpp = $(LEAT_rainbowdexe) $(LLATDOS_rainbowdcpp)
REAT_rainbowdcpp = $(REAT_rainbowdexe) $(RLATDOS_rainbowdcpp)
BEAT_rainbowdcpp = $(BEAT_rainbowdexe) $(BLATDOS_rainbowdcpp)

#
# Dependency List
#
Dep_rainbow = \
   rainbow.exe

rainbow : BccDos.cfg $(Dep_rainbow)
  echo MakeNode 

Dep_rainbowdexe = \
   rainbow.obj\
   screen.obj

rainbow.exe : $(Dep_rainbowdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_rainbowdexe) $(LNIEAT_rainbowdexe) +
C:\BC45\LIB\c0l.obj+
rainbow.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

rainbow.obj :  rainbow.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_rainbowdcpp) $(CNIEAT_rainbowdcpp) -o$@ rainbow.cpp
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
-H=rainbow.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


