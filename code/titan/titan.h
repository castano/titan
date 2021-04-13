/*===========================================================================
	Title: titan.h
	Author: Ignacio Castaño Aguado
	Description: main application header

===========================================================================*/
#ifndef _TITAN_APP_
#define _TITAN_APP_



#define _APP_		"Titan Engine"
#define _VERSION_	"v0.1 (private build)"




/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

// staticaly linked modules:
#include "shared.h"
#include "rfrontend.h"
#include "game logic.h"
#include "input.h"

// interfaces:
#include "../interfaces/render.h"
#include "../interfaces/framework.h"

// files in this module:
#include "console.h"




/*---------------------------------------------------------------------------
	Memory:
---------------------------------------------------------------------------*/

extern TSimpleAlloc GMalloc;

//
// C style memory allocation stubs.
//
#define appMalloc     GMalloc.Malloc
#define appFree       GMalloc.Free
#define appRealloc    GMalloc.Realloc



//
// C++ style memory allocation.
//
inline void* operator new( unsigned int size ){
	return appMalloc( size, TEXT("new"));
}
inline void operator delete( void* ptr ){
	appFree( ptr );
}

inline void* operator new[]( unsigned int size ){
	return appMalloc( size, TEXT("new[]") );
}
inline void operator delete[]( void* ptr ){
	appFree( ptr );
}




#endif	// _TITAN_APP_