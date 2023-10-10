if %1x==x goto noarg
md %1:\byofs
xcopy /s *.* %1:\byofs
goto ender
:noarg
echo need the drive to install to
echo such as
echo INSTALL C
echo or 
echo INSTALL F
echo try again with the drive letter, okay?
:ender
