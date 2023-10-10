@echo off

rem Some files change from the regular fsim build to the fsimstk build.
rem
rem Replacements:
rem 	htimestk.cpp replaces htimer.cpp
rem 	soundstk.cpp replaces sound.cpp
rem
rem New:
rem 	dwdsp.c
rem 	dwdsp.h
rem 	err.c
rem 	err.h
rem 	dws.h
rem 	dwt.h
rem 	dws.lib


call dev bc45 tasm32



del *.obj


bcc -wx -v -ml -f -3 @asmobjs @cpps htimer.cpp sbfm.cpp sound.cpp

copy bresnham.exe fsim.exe
del bresnham.exe


bcc -wx -v -ml -f -3 @asmobjs @cpps htimestk.cpp soundstk.cpp err.c dwdsp.c dws.lib

copy bresnham.exe fsimstk.exe
del bresnham.exe


del *.obj
