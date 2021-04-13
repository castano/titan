/*===========================================================================
	Title: errorlog.cpp
	Author: Ignacio Castaño Aguado
	Description: error logging

===========================================================================*/
#include "shared.h"



/*-----------------------------------------------------------------------------
	Methods:
-----------------------------------------------------------------------------*/

//
// Constructor
//
Error::Error(IOutputDevice *out){
	ErrorString[0]=0;
	LastError=_NO_ERROR;
	this->out=out;
}


//
// Insert message
//
void Error::Insert(const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(ErrorString, msg, arg);

	va_end(arg);

	out->Insert("*** %s", ErrorString);
	LastError=UNKNOWN_ERROR;
}


//
// Log message
//
void Error::Log(const int error, const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(ErrorString, msg, arg);

	va_end(arg);

	out->Insert("*** %s", ErrorString);
	LastError=error;
}


//
// Debug message
//
void Error::Debug(const int error, const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(ErrorString, msg, arg);

	va_end(arg);

	out->Insert("*** %s", ErrorString);
#ifdef WIN32
	OutputDebugString(ErrorString);			// FIXME: should i put #ifdef DEBUG ?
#else
#endif
	LastError=error;
}


//
// Message box
//
void Error::Box(const int error, const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(ErrorString, msg, arg);

	va_end(arg);

	out->Insert("*** %s", ErrorString);
	OutputDebugString(ErrorString);
	MessageBox(NULL, ErrorString, "Error" , MB_OK|MB_ICONWARNING);
	LastError=error;
}


//
// Message Box and exit error
//
void Error::CriticalError(const int error, const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(ErrorString, msg, arg);

	va_end(arg);

	out->Insert("*** %s", ErrorString);
	OutputDebugString(ErrorString);
	MessageBox(NULL, ErrorString, "Critical Error" , MB_OK|MB_ICONWARNING);
	LastError=error;
	exit(-1);
}



