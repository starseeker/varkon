# Microsoft Developer Studio Project File - Name="Varkon95" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Varkon95 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Varkon95.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Varkon95.mak" CFG="Varkon95 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Varkon95 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Varkon95 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /W3 /GX /Ot /Oi /Oy /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "V3_OPENGL" /D "VARKON" /D "V3_ODBC" /YX /FD /c
# SUBTRACT CPP /Og
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib ws2_32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib comctl32.lib winmm.lib /nologo /stack:0x2000000 /subsystem:windows /machine:I386 /out:"..\..\Bin\Varkon95.exe"

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "V3_OPENGL" /D "VARKON" /D "V3_ODBC" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib ws2_32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib comctl32.lib winmm.lib /nologo /stack:0x2000000 /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Varkon95 - Win32 Release"
# Name "Varkon95 - Win32 Debug"
# Begin Group "IG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig12.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig13.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig14.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig15.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig16.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig17.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig18.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig19.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig20.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig21.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig22.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig23.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig24.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig25.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig26.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig27.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig28.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig29.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig30.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig31.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig32.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig33.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig34.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig35.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig36.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig37.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig38.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\IG\src\ig39.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig5.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig6.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig7.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig8.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\ig9.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ig\src\varkon.c
# End Source File
# End Group
# Begin Group "DB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Db\src\db1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\db2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\db3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\db4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbarc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbbplane.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbcsys.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbcurve.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbdelete.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbdim.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbfile.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbgroup.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbheader.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbid.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbkey.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbline.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\DB\src\dbmesh.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbpart.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbpoint.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbsurf.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbtext.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbtform.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbtraverse.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Db\src\dbxhatch.c
# End Source File
# End Group
# Begin Group "GE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge101.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge107.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge109.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge110.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge120.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge133.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge135.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge141.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge200.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge201.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge202.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge203.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge204.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge205.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge206.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge207.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge208.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge300.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge301.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge302.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge303.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge304.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge306.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge307.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge308.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge309.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge310.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge311.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge312.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge315.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge320.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge350.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge351.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge352.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge353.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge354.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge400.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge401.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge402.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge403.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge717.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge718.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge807.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge810.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge813.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge815.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\ge817.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gecurint.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gedim.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\geevale.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\GE\src\geevalnc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gehatch.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\geintpos.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\geinvcrv.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\geinvon.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\geinvtan.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gelinint.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gemktf.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\GE\src\gepinc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gesort.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfang.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfents.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfmath.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfpos.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfseg.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\getfvec.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\gevector.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\suinit.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur100.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur102.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur104.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur105.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur106.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur107.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur108.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur120.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur122.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur150.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur160.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur161.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur163.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur164.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur170.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur172.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur174.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur177.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur178.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur179.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur180.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur181.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur182.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur184.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur186.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur190.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur191.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur200.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur201.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur202.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur203.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur204.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\GE\src\sur205.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur209.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur210.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur211.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur212.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur213.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur214.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur215.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur216.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur217.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur219.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur220.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur221.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur222.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur223.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur224.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur225.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur226.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur227.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur228.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur229.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur230.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur232.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur233.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur234.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur235.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur236.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur240.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur242.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur243.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur245.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur250.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur252.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur254.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur256.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur260.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur262.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur270.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur271.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur272.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur273.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur280.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur288.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur289.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur290.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur291.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur292.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur293.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur294.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur295.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur296.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur297.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur298.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur299.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur300.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur301.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur302.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur311.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur360.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur361.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur362.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur363.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur365.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur367.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur400.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur410.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur412.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur414.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur416.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur460.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur461.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur490.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur500.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur501.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur502.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur503.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur504.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur510.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur511.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur512.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur513.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur514.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur520.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur549.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur550.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur551.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur552.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur553.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur554.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur555.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur556.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur557.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur558.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur559.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur600.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur602.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur604.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur606.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur608.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur609.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur610.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur620.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur622.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur624.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur626.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur628.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur629.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur630.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur635.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur640.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur650.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur651.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur660.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur661.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur670.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur672.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur674.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur676.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur680.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur690.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur700.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur701.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur702.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur705.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur709.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur710.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur711.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur712.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur715.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur716.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur717.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur718.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur719.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur720.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur721.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur722.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur723.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur724.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur725.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur726.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur757.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur758.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur759.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur760.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur761.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur762.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur763.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur764.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur765.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur766.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur767.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur768.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur769.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur770.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur771.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur772.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur773.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur774.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur775.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur776.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur777.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur778.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur779.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur780.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur782.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur784.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur785.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur786.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur787.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur790.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur792.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur799.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur800.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur801.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur850.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur851.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur860.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur861.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur863.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur865.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur868.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur869.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur890.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur891.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur892.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur893.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur900.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur901.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur902.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur903.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur904.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur905.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur910.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur911.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur912.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur913.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur914.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur915.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur916.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur918.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur919.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur920.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur921.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur922.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur923.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur924.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur925.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur926.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur928.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur930.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur940.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur950.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur951.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur952.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur960.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur962.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur963.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur970.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur980.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur982.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur983.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\sur984.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ge\src\suvecm.c
# End Source File
# End Group
# Begin Group "EX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex12.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex13.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex15.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex16.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex17.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex18.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex19.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex20.c

!IF  "$(CFG)" == "Varkon95 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "Varkon95 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex21.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex22.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex23.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex24.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex25.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex26.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\ex27.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\ex28.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex5.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex6.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex7.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex8.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\ex9.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\excugl.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\excurve.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\exmesh.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\exoru.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\exos.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\expoint.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\exrap.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Ex\src\exsugl.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\extcp.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\EX\src\extime.c
# End Source File
# End Group
# Begin Group "GP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp12.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp13.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp14.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp15.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp16.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp17.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp18.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp19.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp20.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp22.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp23.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp24.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp25.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp26.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp27.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp28.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\GP\src\gp29.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp5.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp6.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp8.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Gp\src\gp9.c
# End Source File
# End Group
# Begin Group "WP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp18.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp19.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp20.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp21.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp22.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp23.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp24.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp25.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp26.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp27.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp28.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp29.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp32.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\Wp\src\wp33.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\WP\src\wp34.c
# End Source File
# End Group
# Begin Group "AN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\An\src\anaexp.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anascan.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anlog.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\annaml.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anpar11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anpar12.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anpar31.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anpar51.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\An\src\anpar52.c
# End Source File
# End Group
# Begin Group "PM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\PM\src\calext.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evact.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\eval1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evarc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evarea.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evbpln.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evcray.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evcsy.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evcsys.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evcur.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evdim.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evdll.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evexit.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evexst.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evfile.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evget.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evgfuncs.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evgprocs.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evgrid.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evgrp.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evlev.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evlin.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evmath.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evmbs.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evmesh.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evodbc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evoru.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evos.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evpart.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evpinc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evplot.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evpoi.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evrap.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evset.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evshade.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evstr.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evsur.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evsymb.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evtcp.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evtext.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evtform.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evtime.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evui.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evvfile.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evview.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evwin.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\evxht.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac13.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac17.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac19.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inac20.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inacc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\infunc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\ingeop.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\ingpar.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\innpar.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inproc.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\inrts.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac5.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmac6.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmallo.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pmlist.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\pretty.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\stac10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\PM\src\stacc.c
# End Source File
# End Group
# Begin Group "MS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sources\MS\src\ms1.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms1.rc
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms10.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms11.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms12.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms13.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms14.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms15.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms16.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms17.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms18.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms2.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms3.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms30.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms31.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms32.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms33.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms34.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms35.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms36.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms37.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms38.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms4.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms5.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms6.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms7.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms8.c
# End Source File
# Begin Source File

SOURCE=..\..\sources\MS\src\ms9.c
# End Source File
# End Group
# End Target
# End Project
