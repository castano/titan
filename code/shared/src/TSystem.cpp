/*===========================================================================
	Title: TSystem.cpp
	Author: Ignacio Castaño Aguado
	Description: 
	Classes:
		TSystem
===========================================================================*/
#include "shared.h"


/*---------------------------------------------------------------------------
	Globals
---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
	Definitions
---------------------------------------------------------------------------*/

#define BASE_PATH "base/"
#define Q3_PATH "baseq3/"



/*---------------------------------------------------------------------------
	Members
---------------------------------------------------------------------------*/


Error *			TSystem::err = NULL;
IOutputDevice *	TSystem::out = NULL;
vars *			TSystem::cvars = NULL;




/*---------------------------------------------------------------------------
	Methods
---------------------------------------------------------------------------*/


void TSystem::Init(Error *err, IOutputDevice *out, vars *cvars){

	out->Insert("--- Setting up Titan System\n");

	TSystem::err=err;
	TSystem::out=out;
	TSystem::cvars=cvars;

	// init the file environment
	FileEnvironment::Init();
	FileEnvironment::AddPath(BASE_PATH);
	FileEnvironment::AddPath(Q3_PATH);
	FileEnvironment::LoadPacks();


	// Init basic commands and run default.cmd (do that after cvars and console creation)
	InitCommonCommands();
	CmdFile("default.cmd");
	if(TSystem::err->GetLastError()){
		TSystem::err->CriticalError(CRITICAL_ERROR, "Se produjo un error crítico durante la inicialización,\ncompruebe que no haya archivos dañados o corruptos.\n");
	}
}


