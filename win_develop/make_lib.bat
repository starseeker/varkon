:: ----------------------------------------------------
::
:: Compile the MBS source modules and install the
:: MBO-files in %VARKON_ROOT%/lib
::
:: 2004-10-15, Sören Larsson, Örebro university
::
:: ----------------------------------------------------

@echo off
cd..

:: Get Current Directory into %VARKON_ROOT%
echo.exit|%comspec%/Kprompt set VARKON_ROOT=$P$_|FIND " " > %temp%.\t1.bat
for %%? in (call del) do %%? %temp%.\t1.bat

set MBSC=%VARKON_ROOT%\bin\mbsc.exe
set LIB=%VARKON_ROOT%\lib
set VLIB=%VARKON_ROOT%\sources\vlib

:: Create lib directory 
::------------------------
if not exist lib mkdir lib

:: Delete old files in lib if any
::--------------------------------
cd %LIB%
if exist *.MBO del *.MBO

:: make analysis
::---------------
cd %VLIB%\analysis
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make dxf
::---------------
cd %VLIB%\dxf
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make edit
::---------------
cd %VLIB%\edit
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make fonts\fedit
::---------------
cd %VLIB%\fonts\fedit
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make iges
::---------------
cd %VLIB%\iges
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make macro
::---------------
cd %VLIB%\macro
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make plott
::---------------
cd %VLIB%\plott
FOR %%f IN (*.MBS) DO %MBSC% %%f
echo Moving MBO-files to lib.
move *.MBO %LIB%

:: make stl
::---------------
cd %VLIB%\stl
FOR %%f IN (*.MBS) DO %MBSC% %%f 
echo Moving MBO-files to lib.
move *.MBO %LIB%

Echo.
Echo.
Echo Finished!
Echo.
pause
echo on