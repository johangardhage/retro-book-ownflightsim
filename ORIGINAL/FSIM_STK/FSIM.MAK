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
LLATDOS_fsimdexe =  -c -Tde
RLATDOS_fsimdexe = 
BLATDOS_fsimdexe = 
CNIEAT_fsimdexe = -IC:\BC45\INCLUDE -D
LNIEAT_fsimdexe = -x
LEAT_fsimdexe = $(LLATDOS_fsimdexe)
REAT_fsimdexe = $(RLATDOS_fsimdexe)
BEAT_fsimdexe = $(BLATDOS_fsimdexe)

#
# Dependency List
#
Dep_fsim = \
   fsim.exe

fsim : BccDos.cfg $(Dep_fsim)
  echo MakeNode 

Dep_fsimdexe = \
   fsmain.obj\
   aircraft.obj\
   viewcntl.obj\
   view.obj\
   gauges.obj\
   polylist.obj\
   world.obj\
   poly2.obj\
   loadp.obj\
   drawpol2.obj\
   pcx.obj\
   palette.obj\
   screenc.obj\
   input.obj\
   evntmgr2.obj\
   htimer.obj\
   fix.obj\
   sound.obj\
   sndcntl.obj\
   sbfm.obj\
   detect.obj\
   bresnham.obj\
   screen.obj\
   mio2.obj

fsim.exe : $(Dep_fsimdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_fsimdexe) $(LNIEAT_fsimdexe) +
C:\BC45\LIB\c0l.obj+
fsmain.obj+
aircraft.obj+
viewcntl.obj+
view.obj+
gauges.obj+
polylist.obj+
world.obj+
poly2.obj+
loadp.obj+
drawpol2.obj+
pcx.obj+
palette.obj+
screenc.obj+
input.obj+
evntmgr2.obj+
htimer.obj+
fix.obj+
sound.obj+
sndcntl.obj+
sbfm.obj+
detect.obj+
bresnham.obj+
screen.obj+
mio2.obj
$<,$*
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

fsmain.obj :  fsmain.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ fsmain.cpp
|

aircraft.obj :  aircraft.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ aircraft.cpp
|

viewcntl.obj :  viewcntl.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ viewcntl.cpp
|

view.obj :  view.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ view.cpp
|

gauges.obj :  gauges.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ gauges.cpp
|

polylist.obj :  polylist.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ polylist.cpp
|

world.obj :  world.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ world.cpp
|

poly2.obj :  poly2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ poly2.cpp
|

loadp.obj :  loadp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ loadp.cpp
|

drawpol2.obj :  drawpol2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ drawpol2.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ pcx.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ palette.cpp
|

screenc.obj :  screenc.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ screenc.cpp
|

input.obj :  input.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ input.cpp
|

evntmgr2.obj :  evntmgr2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ evntmgr2.cpp
|

htimer.obj :  htimer.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ htimer.cpp
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ fix.cpp
|

sound.obj :  sound.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ sound.cpp
|

sndcntl.obj :  sndcntl.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ sndcntl.cpp
|

sbfm.obj :  sbfm.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimdexe) $(CNIEAT_fsimdexe) -o$@ sbfm.cpp
|

detect.obj :  detect.asm
  $(TASM) @&&|
 /ml detect.asm ,detect.obj
|

bresnham.obj :  bresnham.asm
  $(TASM) @&&|
 /ml bresnham.asm ,bresnham.obj
|

screen.obj :  screen.asm
  $(TASM) @&&|
 /ml screen.asm ,screen.obj
|

mio2.obj :  mio2.asm
  $(TASM) @&&|
 /ml mio2.asm ,mio2.obj
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=fsim.csm
-ml
-f
-3
| $@


