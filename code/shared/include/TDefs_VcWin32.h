/*============================================================================
	Title: File.h
	Author: Ignacio Castaño Aguado
	Description: This file contains specific win32 definitions for 
		Visual C++ 6.0 compiler.

============================================================================*/




/*----------------------------------------------------------------------------
	Platform specifics types and defines.
----------------------------------------------------------------------------*/

typedef HWND WindowHandle;

// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT
#undef CDECL
#undef BOOL

// Unsigned base types.
typedef unsigned char		BYTE;		// 8-bit  unsigned.
typedef unsigned short		WORD;		// 16-bit unsigned.
typedef unsigned long		DWORD;		// 32-bit unsigned.
typedef unsigned __int64	QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	signed char			SBYTE;		// 8-bit  signed.
typedef signed short		SWORD;		// 16-bit signed.
typedef signed int  		INT;		// 32-bit signed.
typedef signed __int64		SQWORD;		// 64-bit signed.

// small letter version:
typedef unsigned char		byte;
typedef unsigned short		word;
typedef unsigned int		dword;

// other types.
typedef float				FLOAT;
typedef double				DOUBLE;		// 64-bit IEEE double.
typedef INT					BOOL;

// Unwanted VC++ level 4 warnings to disable.
#pragma warning(disable : 4244) /* conversion to float, possible loss of data							*/
//#pragma warning(disable : 4699) /* creating precompiled header											*/
#pragma warning(disable : 4200) /* Zero-length array item at end of structure, a VC-specific extension	*/
#pragma warning(disable : 4100) /* unreferenced formal parameter										*/
//#pragma warning(disable : 4514) /* unreferenced inline function has been removed						*/
//#pragma warning(disable : 4201) /* nonstandard extension used : nameless struct/union					*/
#pragma warning(disable : 4710) /* inline function not expanded											*/
//#pragma warning(disable : 4702) /* unreachable code in inline expanded function							*/
//#pragma warning(disable : 4711) /* function selected for autmatic inlining								*/
//#pragma warning(disable : 4725) /* Pentium fdiv bug														*/
#pragma warning(disable : 4127) /* Conditional expression is constant									*/
//#pragma warning(disable : 4512) /* assignment operator could not be generated                           */
//#pragma warning(disable : 4530) /* C++ exception handler used, but unwind semantics are not enabled     */
//#pragma warning(disable : 4245) /* conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch */
#pragma warning(disable : 4305) /* truncation from 'const double' to 'float'                            */
//#pragma warning(disable : 4238) /* nonstandard extension used : class rvalue used as lvalue             */
//#pragma warning(disable : 4251) /* needs to have dll-interface to be used by clients of class 'ULinker' */
//#pragma warning(disable : 4275) /* non dll-interface class used as base for dll-interface class         */
//#pragma warning(disable : 4511) /* copy constructor could not be generated                              */
//#pragma warning(disable : 4284) /* return type is not a UDT or reference to a UDT                       */
//#pragma warning(disable : 4355) /* this used in base initializer list                                   */
//#pragma warning(disable : 4097) /* typedef-name '' used as synonym for class-name ''                    */
//#pragma warning(disable : 4291) /* typedef-name '' used as synonym for class-name ''                    */

// Strings.
#define LINE_TERMINATOR TEXT("\r\n")
#define PATH_SEPARATOR TEXT("\\")

// DLL file extension.
#define DLLEXT TEXT(".dll")

// No asm if not compiling for x86.
#ifndef _M_IX86
	#undef ASM
	#define ASM 0
#else 
	#define _OS_ "x86-win32"
	#define ASM 1
#endif

// dinamic link libraries.
#define LIBRARY HMODULE
#define LIB_PREFIX		""
#define LIB_SUFFIX		".dll"
#define LOADLIB(file)	LoadLibrary(file)
#define UNLOADLIB(lib)	FreeLibrary((HMODULE)lib)
#define INVALID_LIBRARY	NULL

// path comparator.
#define pathcmp stricmp





/*-----------------------------------------------------------------------------
	Guard macros for call stack display. (taken from unreal)
-----------------------------------------------------------------------------*/

//
// guard/unguardf/unguard macros.
// For showing calling stack when errors occur in major functions.
// Meant to be enabled in release builds.
//

#define guard(func)			{static const char __FUNC_NAME__[]=TEXT(#func);
#define unguard				}
#define unguardf(msg)		}

/*
	currently we use those simple definitions, because all functions arent guarded
	once we guard every function we could throw and catch errors and propagate them
	till recursion ends showing at the end all the calling stack.
*/






/*----------------------------------------------------------------------------
	OS dependant functions:
----------------------------------------------------------------------------*/

//
// Round a floating point number to an integer.
// Note that (int+.5) is rounded to (int+1).
//
#if ASM
__forceinline INT appRound( FLOAT F )
{
	INT I;
	__asm fld [F]
	__asm fistp [I]
	return I;
}
#else
__forceinline INT appRound( FLOAT F )
{
	return (int) F;
}
#endif


//
// Converts to integer equal to or less than.
//
#if ASM
__forceinline INT appFloor( FLOAT F )
{
	static FLOAT Half=0.5;
	INT I;
	__asm fld [F]
	__asm fsub [Half]
	__asm fistp [I]
	return I;
}
#else
__forceinline INT appFloor( FLOAT F )
{
	return (INT)floor(F);
}
#endif

//
// CPU cycles, related to GSecondsPerCycle.
//
#if ASM
#define DEFINED_appCycles 1
#pragma warning (push)
#pragma warning (disable : 4035)
#pragma warning (disable : 4715)
inline DWORD appCycles()
{
	__asm {
		xor   eax,eax	          // Required so that VC++ realizes EAX is modified.
		_emit 0x0F		          // RDTSC  -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		          // Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		xor   edx,edx	          // Required so that VC++ realizes EDX is modified.
	}
}
#pragma warning (pop)
#else
inline DWORD appCycles(){
	return 0;
}
#endif

//
// Memory copy.
//
#if ASM
inline void appMemcpy( void* Dest, const void* Src, INT Count )
{	
	__asm
	{
		mov		ecx, Count
		mov		esi, Src
		mov		edi, Dest
		mov     ebx, ecx
		shr     ecx, 2
		and     ebx, 3
		rep     movsd
		mov     ecx, ebx
		rep     movsb
	}
}
#else
inline void appMemcpy( void* Dest, const void* Src, INT Count )
{	
	memcpy(Dest, Src, Count);
}
#endif

//
// Memory zero.
//
#if ASM
#define DEFINED_appMemzero
inline void appMemzero( void* Dest, INT Count )
{	
	// FIXME: if dest is unaligned, that wont be very fast
	__asm
	{
		mov		ecx, [Count]
		mov		edi, [Dest]
		xor     eax, eax
		mov		ebx, ecx
		shr		ecx, 2
		and		ebx, 3
		rep     stosd
		mov     ecx, ebx
		rep     stosb
	}
}
#else
inline void appMemzero( void* Dest, INT Count )
{	
	memset(Dest, 0, Count);
}
#endif

