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
LLATDOS_translatdexe =  -c -Tde
RLATDOS_translatdexe = 
BLATDOS_translatdexe = 
CNIEAT_translatdexe = -IC:\BC45\INCLUDE -D
LNIEAT_translatdexe = -x
LEAT_translatdexe = $(LLATDOS_translatdexe)
REAT_translatdexe = $(RLATDOS_translatdexe)
BEAT_translatdexe = $(BLATDOS_translatdexe)

#
# Dependency List
#
Dep_translat = \
   translat.exe

translat : BccDos.cfg $(Dep_translat)
  echo MakeNode 

Dep_translatdexe = \
   translat.obj\
   screen.obj\
   bresnham.obj

translat.exe : $(Dep_translatdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_translatdexe) $(LNIEAT_translatdexe) +
C:\BC45\LIB\c0l.obj+
translat.obj+
screen.obj+
bresnham.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

translat.obj :  translat.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_translatdexe) $(CNIEAT_translatdexe) -o$@ translat.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\screen.asm ,screen.obj
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\WIREFRAM\bresnham.asm ,bresnham.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=shape.csm
-ml
-f
| $@


