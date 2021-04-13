/*===========================================================================
	Title: TString.cpp
	Author: Ignacio Castaño Aguado
	Description: usefull string functions

===========================================================================*/
#include "shared.h"



//
// case insensitive strcmp (have been substituted by stricmp)
//
int appStrcmp (const char *s1, const char *s2) {
	int		c1, c2;
	
	do {
		c1 = *s1++;
		c2 = *s2++;

		if (appToLower(c1) != appToLower(c2)) {
			return appToLower(c1) < appToLower(c2) ? -1 : 1;
		}
	} while (c1 && c2);
	
	return 0;		// strings are equal
}
