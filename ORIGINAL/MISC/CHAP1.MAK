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
LLATDOS_chap1dexe =  -c -Tde
RLATDOS_chap1dexe = 
BLATDOS_chap1dexe = 
CNIEAT_chap1dexe = -IC:\BC45\INCLUDE -D
LNIEAT_chap1dexe = -x
LEAT_chap1dexe = $(LLATDOS_chap1dexe)
REAT_chap1dexe = $(RLATDOS_chap1dexe)
BEAT_chap1dexe = $(BLATDOS_chap1dexe)

#
# Dependency List
#
Dep_chap1 = \
   chap1.exe

chap1 : BccDos.cfg $(Dep_chap1)
  echo MakeNode 

Dep_chap1dexe = \
   chap1.obj

chap1.exe : $(Dep_chap1dexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_chap1dexe) $(LNIEAT_chap1dexe) +
C:\BC45\LIB\c0l.obj+
chap1.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

chap1.obj :  chap1.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_chap1dexe) $(CNIEAT_chap1dexe) -o$@ chap1.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=chap1.csm
-ml
-f
| $@


