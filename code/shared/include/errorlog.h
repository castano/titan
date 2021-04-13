/*===========================================================================
	Title: Errorlog.h
	Author: Ignacio Castaño Aguado
	Description: Error logging...
	Classes:
		Error

	TODO:
		- current error codes have no meaning!!
		- linux port...

===========================================================================*/
#ifndef _ERRORLOG_H__
#define _ERRORLOG_H__




/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

class Error : public IErrorDevice, IOutputDevice {
public:
	Error(IOutputDevice *);
	void Insert(const char *, ...);
	void Log(const int error, const char *, ...);
	void Debug(const int error, const char *, ...);
	void Box(const int error, const char *, ...);
	void CriticalError(const int error, const char *, ...);

	int GetLastError(void){return LastError;};

	char ErrorString[1024];

private:
#ifndef WIN32
	FILE * ErrorStream = stdout;
#endif
	int LastError;

	IOutputDevice *out;
};



#endif //_ERRORLOG_H_