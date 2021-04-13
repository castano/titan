/*===========================================================================
	Title: var.h
	Author: Ignacio Castaño Aguado
	Description: var implementation
	Classes:
		var	

===========================================================================*/
#include "shared.h"



/*---------------------------------------------------------------------------
	Var Implementation:
---------------------------------------------------------------------------*/


//
// Integer var
//
Var::Var(const char *name, int ivalue, int flags){
	this->name = new char[strlen(name)+1];
	strcpy(this->name, name);

	this->svalue=NULL;
	SetInteger(ivalue);
	this->flags=flags;
}


//
// Float var
//
Var::Var(const char *name, float fvalue, int flags){
	this->name = new char[strlen(name)+1];
	strcpy(this->name, name);

	this->svalue=NULL;
	SetFloat(fvalue);
	this->flags=flags;
}


//
// String var
//
Var::Var(const char *name, const char *svalue, int flags){
	this->name = new char[strlen(name)+1];
	strcpy(this->name, name);

	this->svalue=NULL;
	SetString(svalue);
	this->flags=flags;
}



//
// Destructor
//
Var::~Var(void){
	delete [] name; name=NULL;
	delete [] svalue; svalue=NULL;
}


void Var::SetString(const char *svalue){
	delete [] this->svalue;
	this->svalue = new char[strlen(svalue)+1];
	strcpy(this->svalue, svalue);

	this->fvalue=atof(svalue);
	this->ivalue=atoi(svalue);
}

void Var::SetInteger(int ivalue){
	char str[64];
	delete [] this->svalue;
	sprintf(str, "%i", ivalue);
	svalue = new char[strlen(str)+1];
	strcpy(svalue, str);
		
	this->fvalue=appRound(ivalue);
	this->ivalue=ivalue;
}

void Var::SetFloat(float fvalue){
	char str[64];
	delete [] this->svalue;
	if(ceil(fvalue)!=fvalue) sprintf(str, "%f", fvalue);
	else sprintf(str, "%i", (int)fvalue);
	svalue = new char[strlen(str)+1];
	strcpy(svalue, str);
		
	this->fvalue=fvalue;
	this->ivalue=(int)fvalue;
}

int Var::operator= ( const int ivalue ){
	SetInteger(ivalue);
	return ivalue;
}
float Var::operator= ( const float fvalue ){
	SetFloat(fvalue);
	return fvalue;
}
void Var::operator= ( const char *svalue ){
	SetString(svalue);
}
