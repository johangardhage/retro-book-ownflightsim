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
CLATDOS_optdemodexe =  -R- -i32 -N -r- -vi- -H- -ml -f
LLATDOS_optdemodexe =  -c -LC:\BC45\LIB -Tde
RLATDOS_optdemodexe = 
BLATDOS_optdemodexe = 
CEAT_optdemodexe = $(CLATDOS_optdemodexe)
CNIEAT_optdemodexe = -IC:\BC45\INCLUDE; -D
LNIEAT_optdemodexe = -x
LEAT_optdemodexe = $(LLATDOS_optdemodexe)
REAT_optdemodexe = $(RLATDOS_optdemodexe)
BEAT_optdemodexe = $(BLATDOS_optdemodexe)
CLATDOS_makesinedexe =  -ml -f
LLATDOS_makesinedexe =  -c -Tde
RLATDOS_makesinedexe = 
BLATDOS_makesinedexe = 
CEAT_makesinedexe = $(CLATDOS_makesinedexe)
CNIEAT_makesinedexe = -IC:\BC45\INCLUDE -D
LNIEAT_makesinedexe = -x
LEAT_makesinedexe = $(LLATDOS_makesinedexe)
REAT_makesinedexe = $(RLATDOS_makesinedexe)
BEAT_makesinedexe = $(BLATDOS_makesinedexe)

#
# Dependency List
#
Dep_optdemo = \
   optdemo.exe\
   makesine.exe

optdemo : BccDos.cfg $(Dep_optdemo)
  echo MakeNode 

Dep_optdemodexe = \
   optdemo.obj\
   loadpoly.obj\
   screen.obj\
   optpoly.obj\
   drawpol2.obj\
   pcx.obj\
   fix.obj

optdemo.exe : $(Dep_optdemodexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_optdemodexe) $(LNIEAT_optdemodexe) +
C:\BC45\LIB\c0l.obj+
optdemo.obj+
loadpoly.obj+
screen.obj+
optpoly.obj+
drawpol2.obj+
pcx.obj+
fix.obj
$<,$*
C:\BC45\LIB\graphics.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

optdemo.obj :  optdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ optdemo.cpp
|

loadpoly.obj :  loadpoly.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ loadpoly.cpp
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml $(USER_ROOT)\OPTIMIZE\screen.asm ,screen.obj
|

optpoly.obj :  optpoly.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ optpoly.cpp
|

drawpol2.obj :  drawpol2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ drawpol2.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ pcx.cpp
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_optdemodexe) $(CNIEAT_optdemodexe) -o$@ fix.cpp
|

Dep_makesinedexe = \
   makesine.obj

makesine.exe : $(Dep_makesinedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_makesinedexe) $(LNIEAT_makesinedexe) +
C:\BC45\LIB\c0l.obj+
makesine.obj
$<,$*
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
-H=optdemo.csm
| $@


