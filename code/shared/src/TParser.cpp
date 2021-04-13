/*===========================================================================
	Title: TParser.h
	Author: Ignacio Castaño Aguado
	Description: parsing stuff
	Classes:
		Parser		- static class to parse files and buffers
===========================================================================*/
#include "shared.h"



/*---------------------------------------------------------------------------
	Member declarations:
---------------------------------------------------------------------------*/

char		Parser::token[MAXTOKEN];			// buffer for tokens
int			Parser::scriptline;
VFile *		Parser::file;
const byte *Parser::buffer;
const byte *Parser::buf_end;





/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/


void Parser::StartParseFile(const char *name){
	file = new VFile(name);
	
	buffer = file->mem;
	buf_end = file->mem + file->size;
	scriptline=0;
}

void Parser::StartParseBuffer(const BYTE *buff, const INT size){
	buffer=buff;
	buf_end=buff+size;
	scriptline=0;
}

void Parser::StartParseString(const char *string){
	buffer=(BYTE *)string;
	buf_end=(BYTE *)string+strlen(string);
	scriptline=0;
}

void Parser::StopParseFile(void){
	delete file;
}


// if crossline is false, GetToken returns false when a line break is found
// GetToken(true) returns 0 only if end of file
// GetToken(false) returns 0 if line break or end of file
bool Parser::GetToken(bool crossline){
	char    *token_p;

	if(!buffer) return 0;
	if(buffer==buf_end) return 0;

	*token=0;	// init to zero


// skip space
skipspace:
	while (*buffer <= 32 || *buffer == '\n' || *buffer == ';'){
		// no se si \n, \t y \r son menores que 32, pero supongo que sí
		if(*buffer=='\n')
			scriptline++;
		if (buffer >= buf_end) {
			if (!crossline){
				//Import.insert("Line %i is incomplete",scriptline);
			}
			return 0;
		}
		
		if (!crossline){
			if(*buffer=='\n' || *buffer==';'){
				return 0;
			}
		}
		buffer++;
	}

	if (buffer >= buf_end) return 0;

	// # // comments
	if (*buffer == '#' || ( buffer[0] == '/' && buffer[1] == '/') ){
		while (*buffer++ != '\n'){
			if (buffer >= buf_end) return 0;
		}
		scriptline++;
		if(!crossline) return 0;
		goto skipspace;
	}

	// /* */ comments
	if (buffer[0] == '/' && buffer[1] == '*'){
		buffer+=2;
		while (buffer[0] != '*' && buffer[1] != '/'){
			buffer++;
			if (buffer >= buf_end){
				//Import.insert("*** Error: Premature end of file, '/*' without '*/'");
				return 0;
			}
			if(*buffer=='\n')scriptline++;
		}
		buffer += 2;
		goto skipspace;
	}


	//
	// copy token
	//
	token_p = token;

	if (*buffer == '"'){		// quoted token
		buffer++;
		while (*buffer != '"'){
			*token_p++ = *buffer++;
			if (buffer == buf_end) break;
			if (token_p == &token[MAXTOKEN]){
				//Import.insert("*** Error, Token too large on line %i\n",scriptline);
			}
		}
		buffer++;
	}
	else {	// regular token
		if(*buffer=='{' || *buffer=='}') {
			*token_p++=*buffer++;
			*token_p = 0;
			return 1;
		}
		while ( *buffer > 32 && *buffer != '\n' && *buffer != ';' && *buffer !='{' && *buffer !='}') {
			*token_p++ = *buffer++;
			if (buffer == buf_end) break;
			if (token_p == &token[MAXTOKEN]){
				//Import.insert("*** Error, Token too large on line %i\n",scriptline);
			}
		}
	}

	*token_p = 0;

	return 1;
}

