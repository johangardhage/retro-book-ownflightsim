(******************************************************************************
File:      err.pas
Version:	 2.20
Tab stops: every 2 columns
Project:   any STK related code
Copyright: 1994-1995 DiamondWare, Ltd.	All rights reserved.*
Written:	 Keith Weiner & Erik Lorenzen
DPMI Ver:  Tom Repstad
Purpose:   Contains a routine to handle any error generated by the STK
History:   94/10/21 KW Started
           95/02/21 EL Finalized for 1.00
           95/03/18 EL Added new error to err_Display(), dws_BUSY
           95/03/22 EL Finalized for 1.01
					 95/04/06 EL Added new error to err_Display(), dws_IRQDISABLED
           95/04/11 EL moved DisplayError to err.c (err_Display).
           95/04/12 EL Finalized for 1.02
					 95/06/06 EL Finalized for 1.03, no changes
					 95/06/06 EL Finalized for 2.00, no changes
           95/09/11 TR Protected Mode Version
					 95/10/16 EL Finalized for 2.10, fixed a commented out error
					 95/10/24 EL Finalized for 2.20, updated err comments (to err.c)

Notes
-----
*Permission is expressely granted to use err_Display or any derivitive made
 from it to registered users of the STK.
******************************************************************************)



unit err;

interface

uses crt, dws;

procedure err_Display;

implementation

procedure err_Display;
begin
  case dws_ErrNo of

    dws_EZERO:
    begin
      (*
       . This should not have happened, considering how we got here!
      *)
			writeln('DWS: I am confused!  There is no error number!');
    end;

    dws_NOTINITTED:
    begin
      (*
       . If we get here, it means you haven't called dws_Init().
       . The STK needs to initialize itself and the hardware before
       . it can do anything.
      *)
			writeln('DWS: The STK was not initialized');
    end;

    dws_ALREADYINITTED:
    begin
      (*
			 . If we get here, it means you've called dws_Init() already.
			 . Calling dws_DetectHardWare at this point would cause zillions
			 . of problems if we let the call through.
      *)
			writeln('DWS: The STK was already initialized');
    end;

    dws_NOTSUPPORTED:
    begin
      (*
       . If we get here, it means that either the user's machine does not
       . support the function you just called, or the STK was told not to
       . support it in dws_Init.
      *)
			writeln('DWS: Function not supported');
    end;

    dws_DetectHardware_UNSTABLESYSTEM:
    begin
      (*
       . Please report it to DiamondWare if you get here!
       .
       . Ideally, you would disable control-C here, so that the user can't
       . hit control-alt-delete, causing SmartDrive to flush its (possibly
       . currupt) buffers.
      *)
			writeln('DWS: The system (or DOS box) is unstable!');
			writeln('     If not in a multitasking system, please power down!');

      while (1 = 1) do
      begin
      end;
    end;

    (*
     . The following three errors are USER/PROGRAMMER errors.  You forgot
     . to fill the cardtyp struct full of -1's (except in those fields
     . you intended to override, or the user (upon the unlikly event that
     . the STK was unable to find a card) gave you a bad overide value.
    *)
    dws_DetectHardware_BADBASEPORT:
    begin
      (*
       . You set dov.baseport to a bad value, or
       . didn't fill it with a -1.
      *)
			writeln('DWS: Bad port address');
    end;

    dws_DetectHardware_BADDMA:
    begin
      (*
       . You set dov.digdma to a bad value, or
       . didn't fill it with a -1.
      *)
			writeln('DWS: Bad DMA channel');
    end;

    dws_DetectHardware_BADIRQ:
    begin
      (*
       . You set dov.digirq to a bad value, or
       . didn't fill it with a -1.
      *)
			writeln('DWS: Bad IRQ level');
    end;

    dws_Kill_CANTUNHOOKISR:
    begin
      (*
       . The STK points the interrupt vector for the sound card's IRQ
       . to its own code in dws_Init.
       .
       . dws_Kill was unable to restore the vector to its original
       . value because other code has hooked it after the STK
       . initialized(!)  This is really bad.  Make the user get rid
       . of it and call dws_Kill again.
      *)
			writeln('DWS: Get rid of your TSR, pal!');
			writeln('     (Press any key)');
      repeat
      until (keypressed);
    end;

    dws_X_BADINPUT:
    begin
      (*
       . The mixer funtion's can only accept volumes between 0 & 255,
       . the volume will remain unchanged.
      *)
			writeln('DWS: Bad mixer level');
    end;

    dws_D_NOTADWD:
    begin
      (* You passed the STK a pointer to something which is not a .DWD file! *)
			writeln('DWS: Pointer does not point to a .DWD');
    end;

    dws_D_NOTSUPPORTEDVER:
    begin
      (*
       . The STK can't play a .DWD converted using a version of VOC2DWD.EXE
       . newer than itself.  And, although we'll try to maintain backwards
       . compatibility, we may not be able to guarantee that newer versions
       . of the code will be able to play older .DWD files.  In any event,
       . it's a good idea to always convert .VOC files with the utility
       . which comes with the library you're linking into your application.
      *)
			writeln('DWS: Please reconvert this file using the VOC2DWD.EXE');
			writeln('     which came with this library');
    end;

    dws_D_INTERNALERROR:
    begin
      (*
       . This error should never occur and probably will not affect sound
       . play(?).  If it happens please contact DiamondWare.
      *)
			writeln('DWS: An internal error has occured');
			writeln('     Please contact DiamondWare');
    end;

    dws_DPlay_NOSPACEFORSOUND:
    begin
      (*
       . This error is more like a warning, though it may happen on a
       . regular basis, depending on how many sounds you told the STK
       . to allow in dws_Init, how you chose to prioritize sounds and
       . how many sounds are currently being played.
      *)
			writeln('DWS: No more room for new digitized sounds right now');
    end;

    dws_DSetRate_FREQTOLOW:
    begin
      (*
       . The STK will set rate as close as possible to the indicated rate
       . but cannot set a rate that low.
      *)
			writeln('DWS: Playback frequency too low');
    end;

    dws_DSetRate_FREQTOHIGH:
    begin
      (*
       . The STK will set rate as close as possible to the indicated rate
       . but cannot set a rate that high.
      *)
			writeln('DWS: Playback frequency too high');
    end;

    dws_MPlay_NOTADWM:
    begin
      (* You passed the STK a pointer to something which is not a .DWM file! *)
			writeln('DWS: Pointer does not point to a .DWM');
    end;

    dws_MPlay_NOTSUPPORTEDVER:
    begin
      (*
       . The STK can't play a .DWM converted using a version of VOC2DWM.EXE
       . newer than itself.  And, although we'll try to maintain backwards
       . compatibility, we may not be able to guarantee that newer versions
       . of the code will be able to play older .DWM files.  In any event,
       . it's a good idea to always convert .MID files with the utility
       . which comes with the library you're linking into your application.
      *)
			writeln('DWS: Please reconvert this file using the MID2DWM.EXE');
			writeln('     which came with this library');
    end;

    dws_MPlay_INTERNALERROR:
    begin
      (*
       . This error should never occur and probably will not affect sound
       . play(?). If it happens please contact DiamondWare.
      *)
			writeln('DWS: An internal error has occured.');
			writeln('     Please contact DiamondWare');
    end;

    dws_BUSY:
    begin
      (*
       . Can only occur when DWS is being called from the background
       . (within an ISR--possibly timer or keyboard handler.)  If you get
       . this error, do your IRET, and call again from your next interrupt.
       . Repeat until successful.
      *)
      writeln('DWS is busy now, please call again later.');
    end;

    dws_IRQDISABLED:
    begin
			(*
			 . The following error may be triggered by dws_Init, dws_Kill,
			 . or dws_DetectHardWare.  It occurs if interrupts are disabled.
			 . Enable interrupts (execute the STI instruction) and call again.
			*)
			writeln('DWS: Interrupts are disabled.');
		end;

{$IFDEF DPMI}
    dws_NOTRESIDENT:
    begin
			(*
			 . The real-mode STK component has not been installed, or
			 . you may have mixed shareware & registerd versions.  The
			 . solution is simple. Either STKRUN.EXE or run the correct
			 . STKRUN.EXE.
			*)
			writeln('DWS: The STK driver is not resident, or you may be');
			writeln('     mixing shareware and registered versions.');
			writeln('     Registered libs are not compatible with the');
			writeln('     shareware STKRUN.EXE, and vice versa.');
    end;
      
    dws_NOMEM:
    begin
      (*
			. The following error will occur if the STK cannot allocate
			. enough DOS (real-mode) memory.
      *)
			writeln('DWS: Insufficient DOS memory.');
    end;
{$ENDIF}

    else
    begin
      (*
       . If this function was clipped intact from err.c, this case
       . should never occur and probably will not affect sound play(?)
       . If it happens please contact DiamondWare.
      *)
			writeln('DWS: I am confused!  Where am I?  HOW DID I GET HERE????');
			writeln('     The ERROR number is:',dws_ErrNo);
    end;
  end;
end;

end.
