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
LLATDOS_walkmandexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_walkmandexe = 
BLATDOS_walkmandexe = 
CNIEAT_walkmandexe = -IC:\BC45\INCLUDE -D
LNIEAT_walkmandexe = -x
LEAT_walkmandexe = $(LLATDOS_walkmandexe)
REAT_walkmandexe = $(RLATDOS_walkmandexe)
BEAT_walkmandexe = $(BLATDOS_walkmandexe)

#
# Dependency List
#
Dep_walkman = \
   walkman.exe

walkman : BccDos.cfg $(Dep_walkman)
  echo MakeNode 

Dep_walkmandexe = \
   walkman.obj\
   sprite.obj\
   pcx.obj\
   screen.obj

walkman.exe : $(Dep_walkmandexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_walkmandexe) $(LNIEAT_walkmandexe) +
C:\BC45\LIB\c0l.obj+
walkman.obj+
sprite.obj+
pcx.obj+
screen.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

walkman.obj :  walkman.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkmandexe) $(CNIEAT_walkmandexe) -o$@ walkman.cpp
|

sprite.obj :  sprite.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkmandexe) $(CNIEAT_walkmandexe) -o$@ sprite.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_walkmandexe) $(CNIEAT_walkmandexe) -o$@ pcx.cpp
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
-H=walkman.csm
-R-
-i32
-N
-r-
-vi-
-H-
-ml
-f
| $@


