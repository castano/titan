/*===========================================================================
	Title: console.h
	Author: Ignacio Castaño Aguado
	Description: console output and input device

===========================================================================*/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define LOGFILE		"console.log"
#define MAXMSGS		1024
#define MSGLENGHT	80




/*---------------------------------------------------------------------------
	Console:
---------------------------------------------------------------------------*/

class Console : public DumpFile {
public:

	// Constructor
	Console(void);

	// Draws the console trough overlay object, and execute commands
	void Update(void);

	// Opens or closes the console
	void Toggle(void);

	// sets the height of the console in a virtual screen of 640x480
	void SetHeight(int h);

	// sets the console font
	void SetFont(int f);

	// sets the back image of the console
	void SetBack(int b);

	// inserts a 
	void Insert(const char *str, ...);

protected:
	
	char msg[MAXMSGS][MSGLENGHT];		// message que
	int	first;							// first and last indexes to the que
	int last;

	int font;							// texture handles
	int back;

	// behaviour of the console:
	bool movingConsole;					
	bool isOpen;
	int VisibleMessages;				// (Height%16)

	float Height;						// visible portion of the console
	float MaxHeight;

	float MoveSpeed;
};





#endif // __CONSOLE_H__