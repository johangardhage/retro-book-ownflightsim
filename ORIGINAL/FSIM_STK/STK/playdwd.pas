(******************************************************************************
File:              playdwd.pas
Version:					 2.20
Tab stops:         every 2 columns
Project:           DWD Player
Copyright:         1994-1995 DiamondWare, Ltd.  All rights reserved.
Written:           Keith Weiner & Erik Lorenzen
Pascal Conversion: David A. Johndrow
DPMI Version:      Tom Repstad
Purpose:           Contains simple example code to show how to load/play a
                   .DWD file
History:           94/10/21 KW Started playdwd.c
                   94/11/12 DJ Translated to Pascal
                   95/01/12 EL Finalized
                   95/03/22 EL Finalized for 1.01
                   95/04/11 EL Finalized for 1.02
									 95/06/06 EL Finalized for 1.03, no changes
									 95/06/06 EL Finalized for 2.00, no changes
                   95/09/11 TR Protected Mode Version
									 95/10/16 EL Finalized for 2.10, fixed bug in Exist (typo)
									 95/10/24 EL Changed volumes to 95%, general cleanup
									 95/10/24 EL Finalized for 2.20

Notes
-----
This code isn't really robust when it comes to standard error checking
and particularly recovery, software engineering technique, etc.  A buffer
is statically allocated.  A better technique would be to use fstat() or stat()
to determine the file's size then malloc(size).  The STK will handle songs
larger than 64K (but not digitized sounds).  Obviously, you'd need to fread()
such a file in chunks, or write some sort of hfread() (huge fread).  Also,
exitting and cleanup is not handled robustly in this code.  The code below can
only be validated by extremely careful scrutiny to make sure each case is
handled properly.

But all such code would make this example file less clear; its purpose was
to illustrate how to call the STK, not how to write QA-proof software.
******************************************************************************)



program playdwd;

{$IFDEF DPMI}
	uses crt, err, mem, dws, winapi;
{$ELSE}
	uses crt, err, mem, dws;
{$ENDIF}


var
	ExitSave: 	pointer;

	sound:			dws_ADDRESS;
	soundsize:	longint;

  dov:        dws_DOPTR;
  dres:       dws_DRPTR;
  ideal:      dws_IDPTR;
	dplay:			dws_DPPTR;

  fp:         file;
  result:     word;


function Exist(filename: string): boolean;
var
	fp: file;

begin
	assign(fp, filename);
  {$I- }
	reset(fp);
	close(fp);
  {$I+ }

  Exist := (IOResult = 0);
end;


procedure ExitPlay; far;

label TRYTOKILLAGAIN;

begin
  ExitProc := ExitSave;

TRYTOKILLAGAIN:

  if (dws_Kill <> 1) then
	begin
    (*
     . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
     . or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
     . must remove his tsr, and dws_Kill must be called again.  If it's
     . dws_NOTINITTED, there's nothing to worry about at this point.
    *)
    err_Display;

    if (dws_ErrNo = dws_Kill_CANTUNHOOKISR) then
		begin
      goto TryToKillAgain;
		end;
	end;

	{$IFDEF DPMI}
	if (sound.ptr <> nil) then
	{$ELSE}
	if (sound <> nil) then
	{$ENDIF}
	begin
		mem_FreeDOS(sound, soundsize);
	end;

  dispose(dplay);
  dispose(ideal);
  dispose(dres);
  dispose(dov);
end;



begin
  ExitSave := ExitProc;
  ExitProc := @ExitPlay;

  writeln;
	writeln('PLAYDWD 2.20 is Copyright 1994-95, DiamondWare, Ltd.');
  writeln('All rights reserved.');
  writeln;
  writeln;

  new(dov);
  new(dres);
  new(ideal);
  new(dplay);

	{$IFDEF DPMI}
	sound.ptr := nil;
	{$ELSE}
	sound := nil;
	{$ENDIF}


  if (ParamCount = 0) then
	begin
    writeln('Usage PLAYDWD <dwd-file>');
    halt(65535);
	end;

  if Exist(ParamStr(1)) then
	begin
		assign(fp, ParamStr(1));
		reset(fp,1);
    soundsize := filesize(fp);

    (* Please note we don't check to see if we get the memory we need. *)
		mem_GetDOS(sound, soundsize);

		{$IFDEF DPMI}
		blockread(fp, sound.ptr^, soundsize);
		{$ELSE}
		blockread(fp, sound^, soundsize);
		{$ENDIF}


		close(fp);
  end
  else
	begin
    writeln('Unable to open '+ParamStr(1));
    halt(65535);
	end;

  (*
   . We need to set every field to -1 in dws_DETECTOVERRIDES record; this
   . tells the STK to autodetect everything.  Any other value
   . overrides the autodetect routine, and will be accepted on
   . faith, though the STK will verify it if possible.
  *)
  dov^.baseport := 65535;
  dov^.digdma   := 65535;
  dov^.digirq   := 65535;

  if (dws_DetectHardWare(dov, dres) = 0) then
	begin
    err_Display;
    halt(65535);
	end;

  if ((dres^.capability and dws_capability_DIG) <> dws_capability_DIG) then
	begin
    if ((dres^.baseport <> 904) and (dres^.baseport <> 65535)) then
		begin
      writeln('The sound hardware supports digitized sound playback,');
      writeln('but we could not find the DMA channel and/or IRQ level.');
    end
    else
		begin
      writeln('Support for digitized playback not found.');
		end;

    halt(65535);
	end;


  (*
   . The "ideal" record tells the STK how you'd like it to initialize the
   . sound hardware.  In all cases, if the hardware won't support your
   . request, the STK will go as close as possible.  For example, not all
   . sound boards will support al sampling rates (some only support 5 or
   . 6 discrete rates).
  *)
  ideal^.musictyp   := 0;     (*0=No music, 1=OPL2*)
  ideal^.digtyp     := 8;     (*0=No Dig, 8=8bit*)
  ideal^.dignvoices := 1;     (*number of voices (up to 16)*)
  ideal^.dignchan   := 1;     (*1=mono*)

	(*Set ideal^.digrate, in Hz*)
  if (dws_DGetRateFromDWD(sound, @ideal^.digrate) = 0) then
	begin
    err_Display;
    halt(65535);
	end;

  if (dws_Init(dres, ideal) = 0) then
	begin
    err_Display;
    halt(65535);
	end;

	(* Set master volume to about 95% max *)
	if (dws_XMaster(242) = 0) then
	begin
    err_Display;
	end;

  dplay^.snd      := sound;
  dplay^.count    := 1;       (* 0=infinite loop, 1-N num times to play sound *)
  dplay^.priority := 1000;
  dplay^.presnd   := 0;

  if (dws_DPlay(dplay) = 0) then
	begin
    err_Display;
    halt(65535);
	end;

  repeat
	begin
    if(dws_DSoundStatus(dplay^.soundnum, @result) = 0) then
		begin
      err_Display;
      halt(65535);
		end;
	end;
  until (result = 0) or (keypressed);

	halt(0);
end.
