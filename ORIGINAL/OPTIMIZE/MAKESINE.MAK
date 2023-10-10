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
LLATDOS_makesinedexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_makesinedexe = 
BLATDOS_makesinedexe = 
CNIEAT_makesinedexe = -IC:\BC45\INCLUDE; -D
LNIEAT_makesinedexe = -x
LEAT_makesinedexe = $(LLATDOS_makesinedexe)
REAT_makesinedexe = $(RLATDOS_makesinedexe)
BEAT_makesinedexe = $(BLATDOS_makesinedexe)

#
# Dependency List
#
Dep_makesine = \
   makesine.exe

makesine : BccDos.cfg $(Dep_makesine)
  echo MakeNode 

Dep_makesinedexe = \
   makesine.obj

makesine.exe : $(Dep_makesinedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_makesinedexe) $(LNIEAT_makesinedexe) +
C:\BC45\LIB\c0l.obj+
makesine.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

makesine.obj :  makesine.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_makesinedexe) $(CNIEAT_makesinedexe) -o$@ makesine.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=makesine.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


