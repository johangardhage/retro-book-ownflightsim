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
LLATDOS_fsimstkdexe =  -c -Tde -LC:\BC45\LIB
RLATDOS_fsimstkdexe = 
BLATDOS_fsimstkdexe = 
CNIEAT_fsimstkdexe = -IC:\BC45\INCLUDE -D
LNIEAT_fsimstkdexe = -x
LEAT_fsimstkdexe = $(LLATDOS_fsimstkdexe)
REAT_fsimstkdexe = $(RLATDOS_fsimstkdexe)
BEAT_fsimstkdexe = $(BLATDOS_fsimstkdexe)
CLATW16_dwsdlib = 
LLATW16_dwsdlib = 
RLATW16_dwsdlib = 
BLATW16_dwsdlib = 
CEAT_dwsdlib = $(CEAT_fsimstkdexe) $(CLATW16_dwsdlib)
CNIEAT_dwsdlib = -IC:\BC45\INCLUDE -D
LNIEAT_dwsdlib = -x
LEAT_dwsdlib = $(LEAT_fsimstkdexe) $(LLATW16_dwsdlib)
REAT_dwsdlib = $(REAT_fsimstkdexe) $(RLATW16_dwsdlib)
BEAT_dwsdlib = $(BEAT_fsimstkdexe) $(BLATW16_dwsdlib)

#
# Dependency List
#
Dep_fsimstk = \
   fsimstk.exe

fsimstk : BccDos.cfg $(Dep_fsimstk)
  echo MakeNode 

Dep_fsimstkdexe = \
   dws.lib\
   soundstk.obj\
   htimestk.obj\
   err.obj\
   dwdsp.obj\
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
   fix.obj\
   sndcntl.obj\
   detect.obj\
   bresnham.obj\
   screen.obj\
   mio2.obj

fsimstk.exe : $(Dep_fsimstkdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_fsimstkdexe) $(LNIEAT_fsimstkdexe) +
C:\BC45\LIB\c0l.obj+
soundstk.obj+
htimestk.obj+
err.obj+
dwdsp.obj+
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
fix.obj+
sndcntl.obj+
detect.obj+
bresnham.obj+
screen.obj+
mio2.obj
$<,$*
dws.lib+
C:\BC45\LIB\emu.lib+
C:\BC45\LIB\mathl.lib+
C:\BC45\LIB\cl.lib

|

soundstk.obj :  soundstk.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ soundstk.cpp
|

htimestk.obj :  htimestk.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ htimestk.cpp
|

err.obj :  err.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ err.c
|

dwdsp.obj :  dwdsp.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ dwdsp.c
|

fsmain.obj :  fsmain.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ fsmain.cpp
|

aircraft.obj :  aircraft.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ aircraft.cpp
|

viewcntl.obj :  viewcntl.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ viewcntl.cpp
|

view.obj :  view.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ view.cpp
|

gauges.obj :  gauges.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ gauges.cpp
|

polylist.obj :  polylist.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ polylist.cpp
|

world.obj :  world.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ world.cpp
|

poly2.obj :  poly2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ poly2.cpp
|

loadp.obj :  loadp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ loadp.cpp
|

drawpol2.obj :  drawpol2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ drawpol2.cpp
|

pcx.obj :  pcx.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ pcx.cpp
|

palette.obj :  palette.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ palette.cpp
|

screenc.obj :  screenc.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ screenc.cpp
|

input.obj :  input.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ input.cpp
|

evntmgr2.obj :  evntmgr2.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ evntmgr2.cpp
|

fix.obj :  fix.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ fix.cpp
|

sndcntl.obj :  sndcntl.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_fsimstkdexe) $(CNIEAT_fsimstkdexe) -o$@ sndcntl.cpp
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
-H=fsimstk.csm
-ml
-f
-3
| $@


