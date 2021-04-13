/*===========================================================================
	Title: TParser.h
	Author: Ignacio Castaño Aguado
	Description: parsing stuff
	Classes:
		Parser		- static class to parse files and buffers
===========================================================================*/
#ifndef _PARSER_INCLUDE
#define _PARSER_INCLUDE




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define MAXTOKEN			1024





/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/


class Parser {
public:
	static void StartParseFile(const char *name);
	static void StartParseBuffer(const BYTE *buffer, const INT size);
	static void StartParseString(const char *string);

	static void StopParseFile(void);

	static bool GetToken(bool crossline);
	static char token[MAXTOKEN];				// buffer for tokens
	static int scriptline;

protected:
	static VFile *file;

	static const BYTE *buffer;
	static const BYTE *buf_end;
};





#endif // _PARSER_INCLUDE

