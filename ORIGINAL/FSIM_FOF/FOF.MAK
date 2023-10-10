.AUTODEPEND

#		*Translator Definitions*
USER_ROOT = C:\BYOFS
CC = bcc +FOF.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 aircraft.obj \
 gauges.obj \
 input.obj \
 htimer.obj \
 screenc.obj \
 bresnham.obj \
 screen.obj \
 sbfm.obj \
 sbfmasm.obj \
 sndcntl.obj \
 viewcntl.obj \
 drawpc.obj \
 fix.obj \
 loadpoly.obj \
 pcx.obj \
 poly.obj \
 view.obj \
 matmult.obj \
 detect.obj \
 fsmain.obj

#		*Explicit Rules*
fof.exe: fof.cfg $(EXE_dependencies)
  $(TLINK) /x/n/c/d/P-/L$(LIBPATH) @&&|
c0l.obj+
aircraft.obj+
gauges.obj+
input.obj+
htimer.obj+
screenc.obj+
bresnham.obj+
screen.obj+
sbfm.obj+
sbfmasm.obj+
sndcntl.obj+
viewcntl.obj+
drawpc.obj+
fix.obj+
loadpoly.obj+
pcx.obj+
poly.obj+
view.obj+
matmult.obj+
detect.obj+
fsmain.obj
fof
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
aircraft.obj: fof.cfg aircraft.cpp 

gauges.obj: fof.cfg gauges.cpp 

input.obj: fof.cfg input.cpp 

htimer.obj: fof.cfg htimer.cpp 

screenc.obj: fof.cfg screenc.cpp 

bresnham.obj: fof.cfg bresnham.asm 
	$(TASM) /MX /ZI /O BRESNHAM.ASM,BRESNHAM.OBJ

screen.obj: fof.cfg screen.asm 
	$(TASM) /MX /ZI /O SCREEN.ASM,SCREEN.OBJ

sbfm.obj: fof.cfg sbfm.cpp 

sbfmasm.obj: fof.cfg sbfmasm.asm 
	$(TASM) /MX /ZI /O SBFMASM.ASM,SBFMASM.OBJ

sndcntl.obj: fof.cfg sndcntl.cpp 

viewcntl.obj: fof.cfg viewcntl.cpp 

drawpc.obj: fof.cfg drawpc.cpp 

fix.obj: fof.cfg fix.cpp 

loadpoly.obj: fof.cfg loadpoly.cpp 

pcx.obj: fof.cfg pcx.cpp 

poly.obj: fof.cfg poly.cpp 

view.obj: fof.cfg view.cpp 

matmult.obj: fof.cfg matmult.asm 
	$(TASM) /MX /ZI /O MATMULT.ASM,MATMULT.OBJ

detect.obj: fof.cfg detect.asm 
	$(TASM) /MX /ZI /O DETECT.ASM,DETECT.OBJ

fsmain.obj: fof.cfg fsmain.cpp 

#		*Compiler Configuration File*
fof.cfg: fof.mak
  copy &&|
-ml
-2
-a
-k-
-H=FOF.SYM
-wbbf
-wpin
-wamb
-wamp
-wasm
-wpro
-wcln
-wdef
-wsig
-wnod
-wstv
-wucp
-wuse
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
| fof.cfg


