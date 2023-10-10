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
LLATDOS_linetestdexe =  -c -Tde
RLATDOS_linetestdexe = 
BLATDOS_linetestdexe = 
CNIEAT_linetestdexe = -IC:\BC45\INCLUDE -D
LNIEAT_linetestdexe = -x
LEAT_linetestdexe = $(LLATDOS_linetestdexe)
REAT_linetestdexe = $(RLATDOS_linetestdexe)
BEAT_linetestdexe = $(BLATDOS_linetestdexe)

#
# Dependency List
#
Dep_linetest = \
   linetest.exe

linetest : BccDos.cfg $(Dep_linetest)
  echo MakeNode 

Dep_linetestdexe = \
   linetest.obj\
   bresn.obj\
   screen.obj

linetest.exe : $(Dep_linetestdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_linetestdexe) $(LNIEAT_linetestdexe) +
C:\BC45\LIB\c0l.obj+
linetest.obj+
bresn.obj+
screen.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

linetest.obj :  linetest.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_linetestdexe) $(CNIEAT_linetestdexe) -o$@ linetest.cpp
|

bresn.obj :  bresn.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_linetestdexe) $(CNIEAT_linetestdexe) -o$@ bresn.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\screen.asm ,screen.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=linetest.csm
-ml
-f
| $@


