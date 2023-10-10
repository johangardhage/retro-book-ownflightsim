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
LLATDOS_loopdexe =  -c -Tde
RLATDOS_loopdexe = 
BLATDOS_loopdexe = 
CNIEAT_loopdexe = -IC:\BC45\INCLUDE -D
LNIEAT_loopdexe = -x
LEAT_loopdexe = $(LLATDOS_loopdexe)
REAT_loopdexe = $(RLATDOS_loopdexe)
BEAT_loopdexe = $(BLATDOS_loopdexe)

#
# Dependency List
#
Dep_loop = \
   loop.exe

loop : BccDos.cfg $(Dep_loop)
  echo MakeNode 

Dep_loopdexe = \
   loop.obj

loop.exe : $(Dep_loopdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_loopdexe) $(LNIEAT_loopdexe) +
C:\BC45\LIB\c0l.obj+
loop.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

loop.obj :  loop.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_loopdexe) $(CNIEAT_loopdexe) -o$@ loop.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=loop.csm
-ml
-f
| $@


