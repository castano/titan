# Microsoft Developer Studio Generated NMAKE File, Based on titan.dsp
!IF "$(CFG)" == ""
CFG=titan - Win32 Debug
!MESSAGE No configuration specified. Defaulting to titan - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "titan - Win32 Release" && "$(CFG)" != "titan - Win32 Debug" && "$(CFG)" != "titan - Win32 Profile"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "titan.mak" CFG="titan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "titan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "titan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "titan - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "titan - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\titan.exe"

!ELSE 

ALL : "game logic - Win32 Release" "input - Win32 Release" "shared - Win32 Release" "rfrontend - Win32 Release" "..\..\titan.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"rfrontend - Win32 ReleaseCLEAN" "shared - Win32 ReleaseCLEAN" "input - Win32 ReleaseCLEAN" "game logic - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\titan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\titan.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /ML /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ASM" /D "_AFXDLL" /Fp"$(INTDIR)\titan.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Qipo /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\titan.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=dinput.lib dxguid.lib winmm.lib shared.lib rfrontend.lib libjpeg.lib user32.lib gdi32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\titan.pdb" /machine:I386 /nodefaultlib:"libcd.lib" /out:"../../titan.exe" 
LINK32_OBJS= \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\titan.obj" \
	"..\rfrontend\lib\rfrontend.lib" \
	"..\shared\lib\shared.lib" \
	"..\input\lib\input.lib" \
	"..\game logic\lib\game logic.lib"

"..\..\titan.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "titan - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\titan.exe"

!ELSE 

ALL : "game logic - Win32 Debug" "input - Win32 Debug" "shared - Win32 Debug" "rfrontend - Win32 Debug" "..\..\titan.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"rfrontend - Win32 DebugCLEAN" "shared - Win32 DebugCLEAN" "input - Win32 DebugCLEAN" "game logic - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\titan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\titan.pdb"
	-@erase "..\..\titan.exe"
	-@erase "..\..\titan.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ASM" /D "_AFXDLL" /Fp"$(INTDIR)\titan.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\titan.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=dinput.lib dxguid.lib winmm.lib shared.lib rfrontend.lib libjpeg.lib user32.lib gdi32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\titan.pdb" /debug /machine:I386 /nodefaultlib:"libcd" /out:"../../titan.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\titan.obj" \
	"..\rfrontend\lib\rfrontend.lib" \
	"..\shared\lib\shared.lib" \
	"..\input\lib\input.lib" \
	"..\game logic\lib\game logic.lib"

"..\..\titan.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "titan - Win32 Profile"

OUTDIR=.\Profile
INTDIR=.\Profile

!IF "$(RECURSE)" == "0" 

ALL : "..\..\titan.exe"

!ELSE 

ALL : "..\..\titan.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\titan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\titan.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ASM" /D "_AFXDLL" /Fp"$(INTDIR)\titan.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Qipo /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\titan.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=dinput.lib dxguid.lib winmm.lib shared.lib rfrontend.lib libjpeg.lib user32.lib gdi32.lib /nologo /subsystem:windows /profile /machine:I386 /out:"../../titan.exe" /Qipo 
LINK32_OBJS= \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\titan.obj"

"..\..\titan.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("titan.dep")
!INCLUDE "titan.dep"
!ELSE 
!MESSAGE Warning: cannot find "titan.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "titan - Win32 Release" || "$(CFG)" == "titan - Win32 Debug" || "$(CFG)" == "titan - Win32 Profile"

!IF  "$(CFG)" == "titan - Win32 Release"

"rfrontend - Win32 Release" : 
   cd "\titan\code\rfrontend"
   $(MAKE) /$(MAKEFLAGS) /F .\rfrontend.mak CFG="rfrontend - Win32 Release" 
   cd "..\titan"

"rfrontend - Win32 ReleaseCLEAN" : 
   cd "\titan\code\rfrontend"
   $(MAKE) /$(MAKEFLAGS) /F .\rfrontend.mak CFG="rfrontend - Win32 Release" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Debug"

"rfrontend - Win32 Debug" : 
   cd "\titan\code\rfrontend"
   $(MAKE) /$(MAKEFLAGS) /F .\rfrontend.mak CFG="rfrontend - Win32 Debug" 
   cd "..\titan"

"rfrontend - Win32 DebugCLEAN" : 
   cd "\titan\code\rfrontend"
   $(MAKE) /$(MAKEFLAGS) /F .\rfrontend.mak CFG="rfrontend - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Profile"

!ENDIF 

!IF  "$(CFG)" == "titan - Win32 Release"

"shared - Win32 Release" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Release" 
   cd "..\titan"

"shared - Win32 ReleaseCLEAN" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Release" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Debug"

"shared - Win32 Debug" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Debug" 
   cd "..\titan"

"shared - Win32 DebugCLEAN" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Profile"

!ENDIF 

!IF  "$(CFG)" == "titan - Win32 Release"

"input - Win32 Release" : 
   cd "\titan\code\input"
   $(MAKE) /$(MAKEFLAGS) /F .\input.mak CFG="input - Win32 Release" 
   cd "..\titan"

"input - Win32 ReleaseCLEAN" : 
   cd "\titan\code\input"
   $(MAKE) /$(MAKEFLAGS) /F .\input.mak CFG="input - Win32 Release" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Debug"

"input - Win32 Debug" : 
   cd "\titan\code\input"
   $(MAKE) /$(MAKEFLAGS) /F .\input.mak CFG="input - Win32 Debug" 
   cd "..\titan"

"input - Win32 DebugCLEAN" : 
   cd "\titan\code\input"
   $(MAKE) /$(MAKEFLAGS) /F .\input.mak CFG="input - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Profile"

!ENDIF 

!IF  "$(CFG)" == "titan - Win32 Release"

"game logic - Win32 Release" : 
   cd "\titan\code\game logic"
   $(MAKE) /$(MAKEFLAGS) /F ".\game logic.mak" CFG="game logic - Win32 Release" 
   cd "..\titan"

"game logic - Win32 ReleaseCLEAN" : 
   cd "\titan\code\game logic"
   $(MAKE) /$(MAKEFLAGS) /F ".\game logic.mak" CFG="game logic - Win32 Release" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Debug"

"game logic - Win32 Debug" : 
   cd "\titan\code\game logic"
   $(MAKE) /$(MAKEFLAGS) /F ".\game logic.mak" CFG="game logic - Win32 Debug" 
   cd "..\titan"

"game logic - Win32 DebugCLEAN" : 
   cd "\titan\code\game logic"
   $(MAKE) /$(MAKEFLAGS) /F ".\game logic.mak" CFG="game logic - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\titan"

!ELSEIF  "$(CFG)" == "titan - Win32 Profile"

!ENDIF 

SOURCE=.\console.cpp

"$(INTDIR)\console.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\titan.cpp

"$(INTDIR)\titan.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

