/*===========================================================================
	Title: TString.h
	Author: Ignacio Castaño Aguado
	Description: String and Character related utilities

===========================================================================*/


/*-----------------------------------------------------------------------------
	Globals:
-----------------------------------------------------------------------------*/

extern DWORD GCRCTable[];



/*-----------------------------------------------------------------------------
	Character type functions:
-----------------------------------------------------------------------------*/

inline char appToUpper( char c ) {
	return (c<'a' || c>'z') ? (c) : (c+'A'-'a');
}
inline char appToLower( char c ) {
	return (c<'A' || c>'Z') ? (c) : (c+'a'-'A');
}
inline bool appIsAlpha( char c ) {
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}
inline bool appIsDigit( char c ) {
	return c>='0' && c<='9';
}
inline bool appIsAlnum( char c ) {
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
}


//
// Case insensitive string hash function.
//
inline DWORD appStrihash( const char* Data ) {
	DWORD Hash=0;
	while( *Data ) {
		char Ch = appToUpper(*Data++);
		BYTE  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}


// case insensitive strcmp
int appStrcmp (const char *s1, const char *s2);


