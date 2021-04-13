/*===========================================================================
	Title: cmd.h
	Author: Ignacio Castaño Aguado
	Description: command parsing, and execing

===========================================================================*/
#ifndef __CMD__H_
#define __CMD__H_



/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

typedef struct _command {
	char *name;
	void (*bind)(void);
	_command *next;
}command;





/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

//
//	Initializes command list with some trivial commands (set, echo, ...)
//
void InitCommonCommands(void);





/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

//
//	List of commands
//
class CmdList {
public:
	CmdList(void);
	~CmdList(void);
	void Add(command *);
	int Exec(const char *buffer);// returns true if cmd found and exec

private:

	command *list;
};


//
//	Parses a command file automatically
//
class CmdFile : public VFile {
public:
	CmdFile(char *name);
};



#endif //__CMD__H_