/*===========================================================================
	Title: TDynalink.cpp
	Author: Ignacio Castaño Aguado
	Description: Dinamic Link Libraries.

===========================================================================*/
#include "shared.h"




/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/


//
// Loads library
//
LIBRARY TLoadLibrary(const char *name){
	char path[MAX_FILEPATH];

	sprintf(path, "%s%s%s", LIB_PREFIX, name, LIB_SUFFIX);

	LIBRARY library = LOADLIB(path);

	return library;
}


//
// Unloads library
//
void TUnloadLibrary(LIBRARY library){
	
	if (library != INVALID_LIBRARY){
		UNLOADLIB(library);
	}

}


//
// Binds simbol.
//
bool TBindSymbol(void** ptr, LIBRARY library, const char* symbol){

#ifdef WIN32
	*ptr = GetProcAddress(library, symbol);
#endif

#ifdef LINUX
	*ptr = dlsym(library, symbol);
#endif

	return (*ptr!=NULL);
}



