(******************************************************************************
File:              playdwm.pas
Version:					 2.20
Tab stops:         every 2 columns
Project:           DWM Player
Copyright:         1994-1995 DiamondWare, Ltd.  All rights reserved.
Written:           Keith Weiner & Erik Lorenzen
Pascal Conversion: David A. Johndrow
DPMI Version:      Tom Repstad
Purpose:           Contains simple example code to show how to load/play a
                   .DWM file
History:           94/10/21 KW Started playdwm.c
                   94/11/12 DJ Translated to Pascal
                   95/01/12 EL Finalized
                   95/03/22 EL Finalized for 1.01
                   95/04/11 EL Finalized for 1.02
									 95/06/06 EL Finalized for 1.03, no changes
									 95/06/06 EL Finalized for 2.00, no changes
									 95/10/16 EL Finalized for 2.10, cleaned up
                   95/09/11 TR Protected Mode Version
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



program playdwm;

{$IFDEF DPMI}
	uses crt, err, mem, dws, winapi;
{$ELSE}
	uses crt, err, mem, dws;
{$ENDIF}


var
	ExitSave: 	 pointer;

	song: 			 dws_ADDRESS;
  songsize:    longint;
  songplaying: word;

  dov:         dws_DOPTR;
  dres:        dws_DRPTR;
  ideal:       dws_IDPTR;
	mplay:			 dws_MPPTR;

  fp:          file;
  ch:          char;
  musvol:      word;


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

  (* If dwt is not inited calling dwt_Kill will have no effect *)
  dwt_Kill;

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
      goto TRYTOKILLAGAIN;
    end;
  end;

	{$IFDEF DPMI}
	if (song.ptr <> nil) then
	{$ELSE}
	if (song <> nil) then
	{$ENDIF}
  begin
		mem_FreeDOS(song, songsize);
  end;

  dispose(mplay);
  dispose(ideal);
  dispose(dres);
  dispose(dov);
end;


begin
  ExitSave := ExitProc;
  ExitProc := @ExitPlay;

  writeln;
	writeln('PLAYDWM 2.20 is Copyright 1994-95, DiamondWare, Ltd.');
  writeln('All rights reserved.');
  writeln;
  writeln;

  new(dov);
  new(dres);
  new(ideal);
  new(mplay);


	{$IFDEF DPMI}
	song.ptr := nil;
	{$ELSE}
	song		 := nil;
	{$ENDIF}

	musvol	 := 255; (* Default mxr volume at startup is max *)
	ch			 := '0';

  if (ParamCount = 0) then
  begin
    writeln('Usage PLAYDWM <dwm-file>');
    halt(65535);
  end;

  if Exist(ParamStr(1)) then
  begin
		assign(fp, ParamStr(1));
		reset(fp,1);
    songsize := filesize(fp);

    (* Please note we don't check to see if we get the memory we need. *)
		mem_GetDOS(song, songsize);

		{$IFDEF DPMI}
		blockread(fp, song.ptr^, songsize);
		{$ELSE}
		blockread(fp, song^, songsize);
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

  (*
   . The "ideal" record tells the STK how you'd like it to initialize the
   . sound hardware.  In all cases, if the hardware won't support your
   . request, the STK will go as close as possible.  For example, not all
   . sound boards will support al sampling rates (some only support 5 or
   . 6 discrete rates).
  *)
  ideal^.musictyp   := 1;     (*for now, it's OPL2 music*)
  ideal^.digtyp     := 0;     (*0=No Dig, 8=8bit, 16=16bit*)
  ideal^.digrate    := 0;     (*sampling rate, in Hz*)
  ideal^.dignvoices := 0;     (*number of voices (up to 16)*)
  ideal^.dignchan   := 0;     (*1=mono, 2=stereo*)

  if (dws_Init(dres, ideal) = 0) then
  begin
    err_Display;
    halt(65535);
  end;

  (*
   .  72.8Hz is a decent compromise.  It will work in a Windows DOS box
   .  without any problems, and yet it allows music to sound pretty good.
   .  In my opinion, there's no reason to go lower than 72.8 (unless you
   .  don't want the hardware timer reprogrammed)--music sounds kinda chunky
   .  at lower rates.  You can go to 145.6 Hz, and get smoother (very
   .  subtly) sounding music, at the cost that it will NOT run at the correct
   .  (or constant) speed in a Windows DOS box.}
  *)
  dwt_Init(dwt_72_8HZ);

	(* Set music volume to about 95% max *)
	musvol := 242;

  if (dws_XMusic(musvol) = 0) then
  begin
    err_Display;
  end;

  mplay^.track := song;
  mplay^.count := 1;

  if (dws_MPlay(mplay) = 0) then
  begin
    err_Display;
    halt(65535);
  end;

  (*
   . We're playing.  Let's exit when the song is over, and allow the user
   . to fiddle with the volume level (mixer) in the meantime
  *)
  writeln('Press + or - to change playback volume ');

  repeat
  begin
    if(dws_MSongStatus(@songplaying) = 0) then
    begin
      err_Display;
      halt(65535);
    end;

    if Keypressed then begin
      ch := readkey;
      case ord(ch) of
        43:
        begin
          if (musvol < 255) Then
            inc(musvol);
          writeln('Music Volume is ', musvol);

          if (dws_XMusic(musvol) = 0) then
          begin
            err_Display;
          end;
        end;
        45:
        begin
          if (musvol) > 0 then
            dec(musvol);
          writeln('Music Volume is ', musvol);

          if (dws_XMusic(musvol) = 0) then
          begin
            err_Display;
          end;
        end;
      end;
    end;
  end;
  until (songplaying = 0) or (ch = 'q') or (ch = 'Q') or (ch = chr(27));

	halt(0);
end.
