/*===========================================================================
	Title: input.h
	Author: Ignacio Castaño Aguado
	Description: main header file of the input module
	TODO: 
		- joystick input
		- multiple mouses, keyboards, joysticks...
		- mouse filtering
		- mouse calibration, (sensibility) and inversion must be in client game!!

===========================================================================*/
#ifndef _INPUT__H___
#define _INPUT__H___




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define MAX_DISP				32
#define MAX_KEYBKEYS			256
#define DINPUT_BUFFERSIZE		2048

// devices:
enum {
	ID_KEYBOARD=0x01,
	ID_MOUSE=0x02,
	ID_JOYSTICK1=0x04,
	ID_JOYSTICK2=0x08,
};

// mouse values:
enum {
	MV_XAXIS,
	MV_YAXIS,
	MV_ZAXIS,
	MV_BUTTON1,
	MV_BUTTON2,
	MV_BUTTON3,
	MV_BUTTON4,
	MV_WHEELUP,
	MV_WHEELDOWN
};



/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

#include "shared.h"
#include "keybuffer.h"

#include "shared.h"

#include "keys.h"

#ifdef WIN32
#define DIRECTINPUT_VERSION	0x0500
#include <dinput.h>
#endif



/*---------------------------------------------------------------------------
	Structures:
---------------------------------------------------------------------------*/

typedef struct {
	int num_mouse;			// number of mouses
	int num_keyboard;		// number of keyboards
	int num_joystick;		// number of joysticks
	int mouse_buttons;		// number of mouse buttons
	int mouse_axis;			// number of mouse axis
	int keyboard_keys;		// number of keyboard keys
	int joystick_buttons;	// number of joystick buttons
	int joystick_axis;		// number of joystick axis

	int keybuffer;			// do we have a key buffer?
	// ...
}input_caps;



// input updates all the actions depending on the buttons or axes asociated
/*typedef struct {
	int num_buttons;					// number of buttons/keys/axes associated with this action
	int value;
	int buttons[NUM_KEYS_PER_ACTION];
	int device[NUM_KEYS_PER_ACTION];
}action;
*/



/*---------------------------------------------------------------------------
	Interfaces:
---------------------------------------------------------------------------*/

class IInput {
public:
	virtual int		Init(void)=0;
	virtual void	Shut(void)=0;
	virtual int		Restart(void)=0;
	virtual void	Refresh(void)=0;
	virtual void	Activate(bool on_off)=0;

	// value reading:
	virtual int		GetValue(int device, int index)=0;
	
	virtual char *	GetBuffer(void)=0;
	virtual int		GetCursorPos(void)=0;
	virtual void	EnableBuffer(bool b)=0;
	virtual void	ClearBuffer(void)=0;
};


class IInputDevice {
public:
	virtual int Init(int flags)=0;
	virtual void Shut(void)=0;
	virtual void Refresh(void)=0;

	virtual int	GetValue(int device, int index)=0;

	virtual const input_caps *GetInputCaps(void)=0;

	KeyBuffer *KeyBuff;
};

/*---------------------------------------------------------------------------
	Implementations:
---------------------------------------------------------------------------*/

#ifdef WIN32
#include "dxinput.h"
#include "stdinput.h"
#endif


/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

void CreateInput(IInput **, Framework *);
void DeleteInput(IInput *inp);




#endif	// _INPUT__H___