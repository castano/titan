/*===========================================================================
	Title: TSystem.h
	Author: Ignacio Castaño Aguado
	Description: 
	Classes:
		TSystem
===========================================================================*/
#ifndef _SYSTEM_INCLUDE
#define _SYSTEM_INCLUDE





/*---------------------------------------------------------------------------
	Framework
---------------------------------------------------------------------------*/
/*
	The framework consist of a memory allocator, an output device, an error manager,
	a timer, and variable system.
*/

class TSystem {
public:
	static void Init(Error *, IOutputDevice *, vars *);

	static Error *			err;
	static IOutputDevice *	out;
	static vars *			cvars;
};





#endif // _FRAMEWORK_INCLUDE