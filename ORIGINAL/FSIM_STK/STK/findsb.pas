(******************************************************************************
File: 						 findsb.pas
Version:					 2.20
Tab stops:				 every 2 columns
Project:					 FINDSB utility
Copyright:				 1994-1995 DiamondWare, Ltd.	All rights reserved.
Written:					 Keith Weiner & Erik Lorenzen
Pascal Conversion: David A. Johndrow
DPMI Version:      Tom Repstad
Purpose:           Example code to autodetect and print out the sound hardware
History:					 94/10/21 KW Started
									 94/11/11 DJ Converted
									 95/02/02 EL Cleaned up & Finalized
									 95/03/22 EL Finalized for 1.01
									 95/04/11 EL Finalized for 1.02
									 95/06/06 EL Finalized for 1.03, no changes
									 95/06/06 EL Finalized for 2.00, no changes
									 95/10/16 EL Finalized for 2.10, no changes
									 95/09/11 TR Protected Mode Version, new HexStr
									 95/10/18 EL Finalized for 2.20, no changes
******************************************************************************)



program findsb;

uses crt, err, dws;


var
	dov:		dws_DOPTR;
	dres: 	dws_DRPTR;


function HexStr(d: word): string;
const
	 hexchars: array [0..15] of char = '0123456789ABCDEF';
var
	 i,j : integer;
	 str : string;
	 chr : char;

begin
	str := '';

	for i := 3 downto 0 do
	begin
		j := i*4;
		chr := hexchars[ (d shr j) and $F];

		if not ((str = '') and (chr = '0')) then
		begin
			str := str + chr;
		end;
	end;
	HexStr := str;
end;


begin
	new(dov);
	new(dres);

	writeln;
	writeln('FINDSB 2.20 is Copyright 1994-95, DiamondWare, Ltd.');
	writeln('All rights reserved.');
	writeln;
	writeln;

	(*
	 . We need to set every field to -1 (65635) in dws_DETECTOVERRIDES struct;
	 . this tells the STK to autodetect everything.  Any other value
	 . overrides the autodetect routine, and will be accepted on
	 . faith, though the STK will verify it if possible.
	*)
	dov^.baseport := 65535;
	dov^.digdma 	:= 65535;
	dov^.digirq 	:= 65535;

	if (dws_DetectHardWare(dov, dres) = 0) then
	begin
		err_Display;
		halt(65535);
	end;


	if (((dres^.capability and dws_capability_FM) = dws_capability_FM) or
			((dres^.baseport <> 904) and (dres^.baseport <> 65535))) then
	begin
		writeln('Base port is ',HexStr(dres^.baseport),' hex');
		writeln('');

		if (dres^.mixtyp <> 1) then
		begin
			writeln('The sound hardware supports mixing.');
			writeln('');
		end
		else
		begin
			writeln('Mixing will be done in software.');
			writeln('');
		end;

		if ((dres^.capability and dws_capability_FM) = dws_capability_FM) then
		begin
			writeln('The sound hardware supports FM music playback.');
			writeln;
		end
		else
		begin
			writeln('Support for FM music playback not found.');
			writeln('');
		end;

		if ((dres^.capability and dws_capability_DIG) = dws_capability_DIG) then
		begin
			(* If we got here dws_DetectHardWare got PORT, IRQ, & DMA *)
			writeln('The sound hardware supports digitized sound playback.');
			writeln('The sound hardware uses DMA channel ',dres^.digdma,' and IRQ level ',dres^.digirq,'.');
			writeln;
		end
		else if ((dres^.baseport <> 904) and (dres^.baseport <> 65535)) then
		begin
			(*
			 . If dres.baseport isn't either 388hex, or -1, then it's a valid
			 . baseport.	So if we got here, then we didn't find either IRQ
			 . level, and/or DMA channel.  In order to play digitized sounds,
			 . we need these settings as well.	In your application, you should
			 . ask the user.
			*)
			writeln('The sound hardware supports digitized sound playback,');
			writeln('but we could not find the DMA channel and/or IRQ level.');

		end
		else
		begin
			writeln('Support for digitized playback not found.');
			writeln('');
		end;

	end
	else
	begin
		writeln('No sound hardware detected.');
		writeln;
	end;

end.
