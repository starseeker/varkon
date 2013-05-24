@echo off
cls

::=======================================================================
::
:: Startscript for english version of Varkon for windows
:: 
:: (c) Sören Larsson, Örebro university 2004-10-13
::
::-----------------------------------------------------------------------

echo.
echo Starting varkon ...


:: Root directory for VARKON installation. You may change this
:: to point to the right location. These next 5 lines
:: will set VARKON_ROOT to the main directory of the same tree
:: as this batch file is located in i.e. X:\varkon_1.18A

cd ..
cd ..
echo.exit|%comspec%/Kprompt set VARKON_ROOT=$P$_|FIND " " > %temp%.\t1.bat
for %%? in (call del) do %%? %temp%.\t1.bat
cd com\english

:: Alternative, set VARKON_ROOT explicit (remove 5 lines above, uncomment line below)
:: This has to be done if this batch-file is moved somewere else
:: set VARKON_ROOT=c:\varkon_1.18A



:: Default root for VARKON applications
:: This directory should normally be X:\varkon\APP
:: or somewhere else outside the VARKON_ROOT tree
:: directory so that a new version of VARKON will
:: not interfere with old jobs.
:: All users need write access to this directory

set VARKON_PRD=%VARKON_ROOT%\APP



:: Project files  (*.PID)
:: You may change this location to some other place
:: like X:\varkon\pid if you want PID files from different
:: versions of VARKON to persist in a common place.
:: All users need write access to this directory

set VARKON_PID=%VARKON_ROOT%\PID



:: Standard resources 

set VARKON_INI=%VARKON_ROOT%\CNF\INI\ENGLISH



:: The VARKON documentation (*.htm)

set VARKON_DOC=%VARKON_ROOT%\MAN



:: Errormessages   (*.ERM)

set VARKON_ERM=%VARKON_ROOT%\ERM



:: Fonts for graphical text

set VARKON_FNT=%VARKON_ROOT%\CNF\FNT



:: System module library   (*.MBO)

set VARKON_LIB=%VARKON_ROOT%\LIB



:: Standard menufiles  (*.MDF, *.INC)

set VARKON_MDF=%VARKON_ROOT%\MDF\ENGLISH



:: Plotter configuration
:: You may change this location to some other place
:: like $HOME/cnf/plt if you want to reuse an old
:: configuration when installing a new version of VARKON.
:: All users need write access to this directory

set VARKON_PLT=%VARKON_ROOT%\CNF\PLT



:: Sound files

set VARKON_SND=%VARKON_ROOT%\CNF\SND



:: Temporary files
:: This line may be changed to /tmp or wherever you
:: like to keep temporary files. On a network this
:: directory should usually reside on the local machine
:: so you may set it up differently for different users.
:: All users need write access to this directory

set VARKON_TMP=C:\TEMP



:: Tolerances

set VARKON_TOL=%VARKON_ROOT%\CNF\TOL



:: Start Varkon95.exe

start %VARKON_ROOT%\bin\Varkon95.exe %1 %2 -pm400000 -rts200000 -gm50000

@echo on
::=======================================================================