/*===========================================================================
	Title: cmd.h
	Author: Ignacio Castaño Aguado
	Description: command parsing, and execing

===========================================================================*/

#include "shared.h"
#include <string.h>		// for strcmp




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

CmdList Commands;		// main list, more lists can be created, but usually we only need one
//extern vars *cvars;



/*---------------------------------------------------------------------------
	CmdList:
---------------------------------------------------------------------------*/

CmdList::CmdList(void){
	list=NULL;
}

CmdList::~CmdList(void){
	command *c, *cnext=NULL;
	for(c=list; c; c=cnext){
		cnext=c->next;
		delete c;
	}
}


void CmdList::Add(command *c){
	c->next=list;
	list=c;
}


int CmdList::Exec(const char *name){

	for(command *c=list; c; c=c->next){
		if(!strcmp(name, c->name)) {
			c->bind();
			return 1;
		}
	}
	
	if(TSystem::cvars->isKey(name)){
		TSystem::out->Insert("%s \"%s\"\n",name, TSystem::cvars->StringForKey(name));
		return 1;
	}
	
	return 0;

}




/*---------------------------------------------------------------------------
	CmdFile:
---------------------------------------------------------------------------*/

CmdFile::CmdFile(char *name):VFile(name){
	
	Parser::StartParseBuffer(mem, size);
	
	while(Parser::GetToken(true)){
		if(!Commands.Exec(Parser::token)){
			TSystem::err->Log(UNKNOWN_ERROR, "Command \"%s\" not found\n", Parser::token);
			return;
			//while(Parser::GetToken(false)){	}// skip unknown command
		}
		//	FIXME: that should give a warning message!!
	}
}



/*---------------------------------------------------------------------------
	Commands:
---------------------------------------------------------------------------*/


// FIXME: that wont work if a command is called inside anoher command since they use the same vars
static char arg[3][512];	// that looks pretty silly

//
// sets var value
//
static void set_cmd(void){
	if(!Parser::GetToken(false)) goto set_cmd_error;
	strcpy(arg[0], Parser::token);
	if(!Parser::GetToken(false)) goto set_cmd_error;
	TSystem::cvars->CreateVar(arg[0], Parser::token);
	return;

set_cmd_error:
	TSystem::err->Log(UNKNOWN_ERROR, "*** sintax error:\n--- set varname, value\n");
}


//
// displays a string in the console
//
static void echo_cmd(void){
	if(Parser::GetToken(false))
		TSystem::out->Insert("%s\n", Parser::token);
}



/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

//
//	Initializes command list
//
void InitCommonCommands(void){
	command *c;
	c=new command;
	c->bind=&set_cmd;
	c->name="set";
	Commands.Add(c);

	c=new command;
	c->bind=&echo_cmd;
	c->name="echo";
	Commands.Add(c);
}
