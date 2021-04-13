# Microsoft Developer Studio Generated NMAKE File, Based on rendogl.dsp
!IF "$(CFG)" == ""
CFG=rendogl - Win32 Profile
!MESSAGE No configuration specified. Defaulting to rendogl - Win32 Profile.
!ENDIF 

!IF "$(CFG)" != "rendogl - Win32 Release" && "$(CFG)" != "rendogl - Win32 Debug" && "$(CFG)" != "rendogl - Win32 Profile"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rendogl.mak" CFG="rendogl - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rendogl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rendogl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rendogl - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "rendogl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\rend_ogl.dll"

!ELSE 

ALL : "shared - Win32 Release" "..\..\rend_ogl.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"shared - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\gls.obj"
	-@erase "$(INTDIR)\glutils.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\state.obj"
	-@erase "$(INTDIR)\textureMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rend_ogl.exp"
	-@erase "..\..\rend_ogl.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /ML /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDOGL_EXPORTS" /D "ASM" /Fp"$(INTDIR)\rendogl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /QaxiM /Qunroll8 /Qipo /Qip /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rendogl.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=version.lib winmm.lib shared.lib user32.lib advapi32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\rend_ogl.pdb" /machine:I386 /nodefaultlib:"libc.lib msvcrt.lib libcp.lib msvcprt.lib libcd.lib" /def:"rendogl.def" /out:"../../rend_ogl.dll" /implib:"$(OUTDIR)\rend_ogl.lib" 
LINK32_OBJS= \
	"$(INTDIR)\gls.obj" \
	"$(INTDIR)\glutils.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\state.obj" \
	"$(INTDIR)\textureMatrix.obj" \
	"..\shared\lib\shared.lib"

"..\..\rend_ogl.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rendogl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\rend_ogl.dll"

!ELSE 

ALL : "shared - Win32 Debug" "..\..\rend_ogl.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"shared - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\gls.obj"
	-@erase "$(INTDIR)\glutils.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\state.obj"
	-@erase "$(INTDIR)\textureMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rend_ogl.exp"
	-@erase "$(OUTDIR)\rend_ogl.lib"
	-@erase "$(OUTDIR)\rend_ogl.pdb"
	-@erase "..\..\rend_ogl.dll"
	-@erase "..\..\rend_ogl.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDOGL_EXPORTS" /D "ASM" /Fp"$(INTDIR)\rendogl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rendogl.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=version.lib winmm.lib shared.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\rend_ogl.pdb" /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /def:"rendogl.def" /out:"../../rend_ogl.dll" /implib:"$(OUTDIR)\rend_ogl.lib" /pdbtype:sept 
DEF_FILE= \
	".\rendogl.def"
LINK32_OBJS= \
	"$(INTDIR)\gls.obj" \
	"$(INTDIR)\glutils.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\state.obj" \
	"$(INTDIR)\textureMatrix.obj" \
	"..\shared\lib\shared.lib"

"..\..\rend_ogl.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rendogl - Win32 Profile"

OUTDIR=.\Profile
INTDIR=.\Profile

!IF "$(RECURSE)" == "0" 

ALL : "..\..\rend_ogl.dll"

!ELSE 

ALL : "..\..\rend_ogl.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\gls.obj"
	-@erase "$(INTDIR)\glutils.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\state.obj"
	-@erase "$(INTDIR)\textureMatrix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rend_ogl.exp"
	-@erase "$(OUTDIR)\rend_ogl.lib"
	-@erase "..\..\rend_ogl.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDOGL_EXPORTS" /D "ASM" /Fp"$(INTDIR)\rendogl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rendogl.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=version.lib winmm.lib shared.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd" /def:"rendogl.def" /out:"../../rend_ogl.dll" /implib:"$(OUTDIR)\rend_ogl.lib" 
DEF_FILE= \
	".\rendogl.def"
LINK32_OBJS= \
	"$(INTDIR)\gls.obj" \
	"$(INTDIR)\glutils.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\state.obj" \
	"$(INTDIR)\textureMatrix.obj"

"..\..\rend_ogl.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("rendogl.dep")
!INCLUDE "rendogl.dep"
!ELSE 
!MESSAGE Warning: cannot find "rendogl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "rendogl - Win32 Release" || "$(CFG)" == "rendogl - Win32 Debug" || "$(CFG)" == "rendogl - Win32 Profile"
SOURCE=..\glsetup\gls.c

"$(INTDIR)\gls.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "rendogl - Win32 Release"

"shared - Win32 Release" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Release" 
   cd "..\rendogl"

"shared - Win32 ReleaseCLEAN" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Release" RECURSE=1 CLEAN 
   cd "..\rendogl"

!ELSEIF  "$(CFG)" == "rendogl - Win32 Debug"

"shared - Win32 Debug" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Debug" 
   cd "..\rendogl"

"shared - Win32 DebugCLEAN" : 
   cd "\titan\code\shared"
   $(MAKE) /$(MAKEFLAGS) /F .\shared.mak CFG="shared - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\rendogl"

!ELSEIF  "$(CFG)" == "rendogl - Win32 Profile"

!ENDIF 

SOURCE=.\glutils.cpp

"$(INTDIR)\glutils.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\render.cpp

"$(INTDIR)\render.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\state.cpp

"$(INTDIR)\state.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\textureMatrix.cpp

"$(INTDIR)\textureMatrix.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

