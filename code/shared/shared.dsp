# Microsoft Developer Studio Project File - Name="shared" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=shared - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shared.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shared.mak" CFG="shared - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shared - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "shared - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "shared - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "shared___Win32_Release"
# PROP BASE Intermediate_Dir "shared___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ASM" /YX /FD /c
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\sharedr.lib"

!ELSEIF  "$(CFG)" == "shared - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "shared___Win32_Debug"
# PROP BASE Intermediate_Dir "shared___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\shared.lib"

!ENDIF 

# Begin Target

# Name "shared - Win32 Release"
# Name "shared - Win32 Debug"
# Begin Group "interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\interfaces\framework.h
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\include\alloc.h
# End Source File
# Begin Source File

SOURCE=.\include\cmd.h
# End Source File
# Begin Source File

SOURCE=.\include\consts.h
# End Source File
# Begin Source File

SOURCE=.\Zip\Crc32.h
# End Source File
# Begin Source File

SOURCE=.\include\errorlog.h
# End Source File
# Begin Source File

SOURCE=.\include\file.h
# End Source File
# Begin Source File

SOURCE=.\include\geom.h
# End Source File
# Begin Source File

SOURCE=.\Zip\Globals.h
# End Source File
# Begin Source File

SOURCE=.\include\shared.h
# End Source File
# Begin Source File

SOURCE=.\include\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=.\include\Tbitarray.h
# End Source File
# Begin Source File

SOURCE=.\include\TDefs_VcWin32.h
# End Source File
# Begin Source File

SOURCE=.\include\TGrowArray.h
# End Source File
# Begin Source File

SOURCE=.\include\THashTable.h
# End Source File
# Begin Source File

SOURCE=.\include\timer.h
# End Source File
# Begin Source File

SOURCE=.\include\TParser.h
# End Source File
# Begin Source File

SOURCE=.\include\TString.h
# End Source File
# Begin Source File

SOURCE=.\include\TSystem.h
# End Source File
# Begin Source File

SOURCE=.\include\TWinAlloc.h
# End Source File
# Begin Source File

SOURCE=.\Zip\Unzip.h
# End Source File
# Begin Source File

SOURCE=.\include\vars.h
# End Source File
# End Group
# Begin Group "source"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\src\cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\src\consts.cpp
# End Source File
# Begin Source File

SOURCE=.\src\errorlog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\file.cpp
# End Source File
# Begin Source File

SOURCE=.\src\math.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TAlloc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TCRC32.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TDynalink.cpp
# End Source File
# Begin Source File

SOURCE=.\src\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TString.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip\Unzip.cpp
# End Source File
# Begin Source File

SOURCE=.\src\vars.cpp
# End Source File
# End Group
# End Target
# End Project
