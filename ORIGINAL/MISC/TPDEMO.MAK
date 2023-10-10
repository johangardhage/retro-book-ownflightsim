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
LLATDOS_tpdemodexe =  -c -Tde
RLATDOS_tpdemodexe = 
BLATDOS_tpdemodexe = 
CNIEAT_tpdemodexe = -IC:\BC45\INCLUDE -D
LNIEAT_tpdemodexe = -x
LEAT_tpdemodexe = $(LLATDOS_tpdemodexe)
REAT_tpdemodexe = $(RLATDOS_tpdemodexe)
BEAT_tpdemodexe = $(BLATDOS_tpdemodexe)

#
# Dependency List
#
Dep_tpdemo = \
   tpdemo.exe

tpdemo : BccDos.cfg $(Dep_tpdemo)
  echo MakeNode 

Dep_tpdemodexe = \
   tpdemo.obj

tpdemo.exe : $(Dep_tpdemodexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_tpdemodexe) $(LNIEAT_tpdemodexe) +
C:\BC45\LIB\c0l.obj+
tpdemo.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

tpdemo.obj :  tpdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_tpdemodexe) $(CNIEAT_tpdemodexe) -o$@ tpdemo.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=tpdemo.csm
-ml
-f
-y
| $@


