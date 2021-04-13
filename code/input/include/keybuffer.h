/*===========================================================================
	Title: keybuffer.h
	Author: Ignacio Castaño Aguado
	Description: Buffer of the last key pressed

===========================================================================*/
#ifndef _KEY_BUFF__H__
#define _KEY_BUFF__H__




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define BUFFER_SIZE				2048
#define FIRST_REPEAT_TIME		100
#define NEXT_REPEAT_TIME		20



/*---------------------------------------------------------------------------
	Structures:
---------------------------------------------------------------------------*/

typedef struct {
	int TimeStamp;			// time that have been pressed
	int NextTime;			// time to put this Char again
}Key;




/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

class KeyBuffer {
public:
	KeyBuffer(void);

	void Clear(void);

	void ProcessKey(BYTE *buffer, int key);		// buffer is a buffer with all the keys pressed

	char *GetBuffer(void);

	int GetCursorPos(void){return current;};
	
	void Enabled(bool b);

protected:
	void Insert(char c);

	Key	Keys[MAX_KEYBKEYS];
	char Buffer[BUFFER_SIZE];
	int top;					// top of the stack
	int current;				// current position on the stack
	bool enabled;
};



#endif	// _KEY_BUFF__H__