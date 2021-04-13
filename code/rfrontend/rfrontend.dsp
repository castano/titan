# Microsoft Developer Studio Project File - Name="rfrontend" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rfrontend - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rfrontend.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rfrontend.mak" CFG="rfrontend - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rfrontend - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "rfrontend - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "rfrontend - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
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
# ADD LIB32 /nologo /out:"lib\rfrontendr.lib"

!ELSEIF  "$(CFG)" == "rfrontend - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
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
# ADD LIB32 /nologo /out:"lib\rfrontend.lib"

!ENDIF 

# Begin Target

# Name "rfrontend - Win32 Release"
# Name "rfrontend - Win32 Debug"
# Begin Group "include"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\include\BezierPatch.h
# End Source File
# Begin Source File

SOURCE=.\include\bsp.h
# End Source File
# Begin Source File

SOURCE=.\include\camera.h
# End Source File
# Begin Source File

SOURCE=.\include\layer.h
# End Source File
# Begin Source File

SOURCE=.\include\object.h
# End Source File
# Begin Source File

SOURCE=.\include\overlay.h
# End Source File
# Begin Source File

SOURCE=.\include\rfrontend.h
# End Source File
# Begin Source File

SOURCE=.\include\scene.h
# End Source File
# Begin Source File

SOURCE=.\include\shader.h
# End Source File
# Begin Source File

SOURCE=.\include\shaderfile.h
# End Source File
# Begin Source File

SOURCE=.\include\tex.h
# End Source File
# Begin Source File

SOURCE=.\include\TShaderParser.h
# End Source File
# Begin Source File

SOURCE=.\include\world.h
# End Source File
# End Group
# Begin Group "source"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\source\BezierPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\source\bsp.cpp
# End Source File
# Begin Source File

SOURCE=.\source\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\source\frustum.cpp
# End Source File
# Begin Source File

SOURCE=.\source\layer.cpp
# End Source File
# Begin Source File

SOURCE=.\source\overlay.cpp
# End Source File
# Begin Source File

SOURCE=.\source\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\source\shader.cpp
# End Source File
# Begin Source File

SOURCE=.\source\shaderfile.cpp
# End Source File
# Begin Source File

SOURCE=.\source\tex.cpp
# End Source File
# Begin Source File

SOURCE=.\source\TShaderParser.cpp
# End Source File
# Begin Source File

SOURCE=.\source\world.cpp
# End Source File
# End Group
# End Target
# End Project
