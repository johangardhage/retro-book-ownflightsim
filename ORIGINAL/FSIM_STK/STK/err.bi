'******************************************************************************
'File:      err.bi
'Version:   2.20
'Tab stops: every 2 columns
'Project:   any STK related code
'Copyright: 1994-1995 DiamondWare, Ltd.  All rights reserved.*
'Written:   Keith Weiner & Erik Lorenzen
'Purpose:   Declares an error handler for STK-generated errors
'History:   94/10/21 KW Started
'           95/02/21 EL Finalized for 1.00
'           95/03/18 EL Added new error to err_Display(), dws_BUSY
'           95/03/22 EL Finalized for 1.01
'           95/04/06 EL Added new error to err_Display(), dws_MUSTBECLI
'           95/04/11 EL moved DisplayError to err.c (err_Display).
'           95/04/11 EL Finalized for 1.02
'           95/06/06 EL Finalized for 1.03, no changes
'           95/08/01 EL Finalized for 2.00, no changes
'           95/10/07 EL Finalized for 2.10, no changes
'           95/10/18 EL Finalized for 2.20, no changes
'
'Notes
'-----
'*Permission is expressely granted to use err_Display or any derivitive made
' from it to registered users of the STK.
'******************************************************************************



DECLARE SUB errDisplay ()
