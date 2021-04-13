/*===========================================================================
	Title: framework.h
	Author: Ignacio Castaño Aguado
	Description: interfaces of the objects that are exported to the modules

===========================================================================*/
#ifndef _FRAMEWORK__H_
#define _FRAMEWORK__H_



/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

enum {
	_NO_ERROR,
	ERROR_LOADING_FILE,
	ERROR_LOADING_MODULE,
	ERROR_FREEING_MODULE,
	ERROR_ALLOCATING_MEMORY,
	ERROR_READING_FILE,
	ERROR_INVALID_FILENAME,
	ERROR_FORMAT_NOT_SUPPORTED,
	ERROR_CREATING_WINDOW,
	CRITICAL_ERROR,
	HEAP_ERROR,
	PARSE_ERROR,
	UNKNOWN_ERROR,
};



/*---------------------------------------------------------------------------
	Interfaces:
---------------------------------------------------------------------------*/


//
// Generic Output Device
//
class IOutputDevice {
public:
	virtual void Insert(const char *str, ...)=0;
};


//
// Generic Output Devide for errors
//
class IErrorDevice {
public:
	virtual void Log(const int error, const char *, ...)=0;
	virtual void Debug(const int error, const char *, ...)=0;
	virtual void Box(const int error, const char *, ...)=0;
	virtual void CriticalError(const int error, const char *, ...)=0;
};


//
// Variable Sytem Interface
//
class ivars {
public:
	virtual bool SetKeyValue(const char *name, const char *string, int flags=0)=0;
	virtual bool SetKeyValue(const char *name, const float value, int flags=0)=0;
	virtual bool SetKeyValue(const char *name, const int value, int flags=0)=0;

	virtual void RegisterVar(Var &var)=0;
	virtual void UnregisterVar(Var &var)=0;

	// query methods:
	virtual char * StringForKey(const char * name) =0;
	virtual float ValueForKey(const char * name) =0;
	virtual int IntForKey(const char * name) =0;
};


//
// The framework itselfs
//
class Framework {
public:
	class ivars	*			cvars;
	class IOutputDevice *	console;
	class Timer *			timer;	// FIXME: export a timer interface, instead the timer itself
	class IErrorDevice *	error;
};




#endif // _FRAMEWORK__H_