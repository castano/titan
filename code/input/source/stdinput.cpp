/*===========================================================================
	Title: stdinput.cpp
	Author: Ignacio Castaño Aguado
	Description: Standard input device

===========================================================================*/
#ifdef WIN32
#include "input.h"
#include <winuser.h>



/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define NUM_DI_KEYS		222




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern Framework Import;
extern BYTE TranslationTable[NUM_DI_KEYS];





/*---------------------------------------------------------------------------
	stdInput methods
---------------------------------------------------------------------------*/


stdInput::stdInput(void){
};


int stdInput::Init(int flags){

	center_x=(int)(Import.cvars->ValueForKey("v_width")/2.0f);
	center_y=(int)(Import.cvars->ValueForKey("v_height")/2.0f);


	if(flags&ID_KEYBOARD) {
		this->flags|=ID_KEYBOARD;

		BYTE state[256];			// clear all keys
		appMemzero(state, 256);
		SetKeyboardState(state);

	}
	if(flags&ID_MOUSE) this->flags|=ID_MOUSE;

	return flags;
};


void stdInput::Shut(void){

};


void stdInput::Refresh(void){
	if(flags&ID_KEYBOARD) {
	}
	if(flags&ID_MOUSE) {
		POINT p;
		GetCursorPos(&p);
		mx=-(p.x-center_x);
		my=-(p.y-center_y);
		SetCursorPos( center_x, center_y );
	}
};


int stdInput::GetValue(int device, int index){
	static HKL layout=GetKeyboardLayout(0);

	if(device==ID_KEYBOARD)	{
		return GetAsyncKeyState(TranslationTable[index]);
	}
	else if(device==ID_MOUSE) {
		switch(index){
			case MV_XAXIS:
				return mx;
			case MV_YAXIS:
				return my;
			case MV_ZAXIS:
				return 0;
			case MV_BUTTON1:
				return GetAsyncKeyState(VK_LBUTTON);
			case MV_BUTTON2:
				return GetAsyncKeyState(VK_RBUTTON);
			case MV_BUTTON3:
				return GetAsyncKeyState(VK_MBUTTON);
			case MV_BUTTON4:
				return 0;
			case MV_WHEELUP:
				return 0;
			case MV_WHEELDOWN:
				return 0;
		};
		return 0;
	}
	else return 0;
};


const input_caps *stdInput::GetInputCaps(void){
	return NULL;
};







/*---------------------------------------------------------------------------
	Boring translation table
---------------------------------------------------------------------------*/

//
// converts from windows virtual-keys to our codes
//
static BYTE TranslationTable[NUM_DI_KEYS] = {
	0x00,			//					0x00
	VK_ESCAPE,		// KEY_ESCAPE		0x01
	'1',			// KEY_1			0x02
	'2',			// KEY_2			0x03
	'3',			// KEY_3			0x04
	'4',			// KEY_4			0x05
	'5',			// KEY_5			0x06
	'6',			// KEY_6			0x07
	'7',			// KEY_7			0x08
	'8',			// KEY_8			0x09
	'9',			// KEY_9			0x0A
	'0',			// KEY_0			0x0B
	VK_SUBTRACT,	// KEY_MINUS		0x0C
	0x00,			// KEY_EQUALS		0x0D
	VK_BACK,		// KEY_BACKSPACE	0x0E
	VK_TAB,			// KEY_TAB			0x0F
	'Q',			// KEY_Q			0x10
	'W',			// KEY_W			0x11
	'E',			// KEY_E			0x12
	'R',			// KEY_R			0x13
	'T',			// KEY_T			0x14
	'Y',			// KEY_Y			0x15
	'U',			// KEY_U			0x16
	'I',			// KEY_I			0x17
	'O',			// KEY_O			0x18
	'P',			// KEY_P			0x19
	0x00,			// KEY_LBRACKET		0x1A
	0x00,			// KEY_RBRACKET		0x1B
	VK_RETURN,		// KEY_ENTER		0x1C
	VK_CONTROL,		// KEY_LCONTROL		0x1D
	'A',			// KEY_A			0x1E
	'S',			// KEY_S			0x1F
	'D',			// KEY_D			0x20
	'F',			// KEY_F			0x21
	'G',			// KEY_G			0x22
	'H',			// KEY_H			0x23
	'J',			// KEY_J			0x24
	'K',			// KEY_K			0x25
	'L',			// KEY_L			0x26
	0x00,			// KEY_SEMICOLON	0x27
	0x00,			// KEY_APOSTROPHE	0x28
	0x00,			// KEY_GRAVE		0x29
	VK_SHIFT,		// KEY_LSHIFT		0x2A
	0x00,			// KEY_BACKSLASH	0x2B
	'Z',			// KEY_Z			0x2C
	'X',			// KEY_X			0x2D
	'C',			// KEY_C			0x2E
	'V',			// KEY_V			0x2F
	'B',			// KEY_B			0x30
	'N',			// KEY_N			0x31
	'M',			// KEY_M			0x32
	0x00,			// KEY_COMMA		0x33
	0x00,			// KEY_PERIOD		0x34
	0x00,			// KEY_SLASH		0x35
	VK_SHIFT,		// KEY_RSHIFT		0x36
	VK_MULTIPLY,	// KEY_KP_MULTIPLY	0x37
	VK_MENU,		// KEY_LALT			0x38
	VK_SPACE,		// KEY_SPACE		0x39
	VK_CAPITAL,		// KEY_CAPSLOCK		0x3A
	VK_F1,			// KEY_F1			0x3B
	VK_F2,			// KEY_F2			0x3C
	VK_F3,			// KEY_F3			0x3D
	VK_F4,			// KEY_F4			0x3E
	VK_F5,			// KEY_F5			0x3F
	VK_F6,			// KEY_F6			0x40
	VK_F7,			// KEY_F7			0x41
	VK_F8,			// KEY_F8			0x42
	VK_F9,			// KEY_F9			0x43
	VK_F10,			// KEY_F10			0x44
	VK_NUMLOCK,		// KEY_NUMLOCK		0x45
	VK_SCROLL,		// KEY_SCRLOCK		0x46
	VK_NUMPAD7,		// KEY_KP_7			0x47
	VK_NUMPAD8,		// KEY_KP_8			0x48
	VK_NUMPAD9,		// KEY_KP_9			0x49
	VK_SUBTRACT,	// KEY_KP_MINUS		0x4A
	VK_NUMPAD4,		// KEY_KP_4	        0x4B
	VK_NUMPAD5,		// KEY_KP_5			0x4C
	VK_NUMPAD6,		// KEY_KP_6			0x4D
	VK_ADD,			// KEY_KP_PLUS		0x4E
	VK_NUMPAD1,		// KEY_KP_1			0x4F
	VK_NUMPAD2,		// KEY_KP_2			0x50
	VK_NUMPAD3,		// KEY_KP_3			0x51
	VK_NUMPAD0,		// KEY_KP_0			0x52
	VK_DECIMAL,		// KEY_KP_DECIMAL	0x53
	0x00,			//					0x54
	0x00,			//					0x55
	0x00,			//					0x56
	VK_F11,			// KEY_F11			0x57
	VK_F12,			// KEY_F12			0x58
	0x00,			//					0x59
	0x00,			//					0x5A
	0x00,			//					0x5B
	0x00,			//					0x5C
	0x00,			//					0x5D
	0x00,			//					0x5E
	0x00,			//					0x5F
	0x00,			//					0x60
	0x00,			//					0x61
	0x00,			//					0x62
	0x00,			//					0x63
	VK_F13,			// KEY_F13			0x64
	VK_F14,			// KEY_F14			0x65
	VK_F15,			// KEY_F15			0x66
	0x00,			//					0x67
	0x00,			//					0x68
	0x00,			//					0x69
	0x00,			//					0x6A
	0x00,			//					0x6B
	0x00,			//					0x6C
	0x00,			//					0x6D
	0x00,			//					0x6E
	0x00,			//					0x6F
	0x00,			// KEY_KANA			0x70
	0x00,			//					0x71
	0x00,			//					0x72
	0x00,			//					0x73
	0x00,			//					0x74
	0x00,			//					0x75
	0x00,			//					0x76
	0x00,			//					0x77
	0x00,			//					0x78
	0x00,			// KEY_CONVERT		0x79
	0x00,			//					0x7A
	0x00,			// KEY_NOCONVERT	0x7B
	0x00,			//					0x7C
	0x00,			// KEY_YEN			0x7D
	0x00,			//					0x7E
	0x00,			//					0x7F
	0x00,			//					0x80
	0x00,			//					0x81
	0x00,			//					0x82
	0x00,			//					0x83
	0x00,			//					0x84
	0x00,			//					0x85
	0x00,			//					0x86
	0x00,			//					0x87
	0x00,			//					0x88
	0x00,			//					0x89
	0x00,			//					0x8A
	0x00,			//					0x8B
	0x00,			//					0x8C
	0x00,			// KEY_KP_EQUALS	0x8D
	0x00,			//					0x8E
	0x00,			//					0x8F
	0x00,			// KEY_CIRCUMFLEX	0x90
	0x00,			// KEY_AT			0x91
	0x00,			// KEY_COLON		0x92
	0x00,			// KEY_UNDERLINE	0x93
	0x00,			// KEY_KANJI		0x94
	0x00,			// KEY_STOP			0x95
	0x00,			// KEY_AX			0x96
	0x00,			// KEY_UNLABELED	0x97
	0x00,			//					0x98
	0x00,			//					0x99
	0x00,			//					0x9A
	0x00,			//					0x9B
	VK_RETURN,		// KEY_KP_ENTER		0x9C
	VK_CONTROL,		// KEY_RCONTROL		0x9D
	0x00,			//					0x9E
	0x00,			//					0x9F
	0x00,			//					0xA0
	0x00,			//					0xA1
	0x00,			//					0xA2
	0x00,			//					0xA3
	0x00,			//					0xA4
	0x00,			//					0xA5
	0x00,			//					0xA6
	0x00,			//					0xA7
	0x00,			//					0xA8
	0x00,			//					0xA9
	0x00,			//					0xAA
	0x00,			//					0xAB
	0x00,			//					0xAC
	0x00,			//					0xAD
	0x00,			//					0xAE
	0x00,			//					0xAF
	0x00,			//					0xB0
	0x00,			//					0xB1
	0x00,			//					0xB2
	0x00,			// KEY_KP_COMMA		0xB3
	0x00,			//					0xB4
	VK_DIVIDE,		// KEY_KP_DIVIDE	0xB5
	0x00,			//					0xB6
	0x00,			// KEY_SYSRQ		0xB7
	VK_MENU,		// KEY_RALT			0xB8
	0x00,			//					0xB9
	0x00,			//					0xBA
	0x00,			//					0xBB
	0x00,			//					0xBC
	0x00,			//					0xBD
	0x00,			//					0xBE
	0x00,			//					0xBF
	0x00,			//					0xC0
	0x00,			//					0xC1
	0x00,			//					0xC2
	0x00,			//					0xC3
	0x00,			//					0xC4
	0x00,			//					0xC5
	0x00,			//					0xC6
	VK_HOME,		// KEY_HOME			0xC7
	VK_UP,			// KEY_UP			0xC8
	VK_PRIOR,		// KEY_PAGEUP		0xC9
	0x00,			//					0xCA
	VK_LEFT,		// KEY_LEFT			0xCB
	0x00,			//					0xCC
	VK_RIGHT,		// KEY_RIGHT		0xCD
	0x00,			//					0xCE
	VK_END,			// KEY_END			0xCF
	VK_DOWN,		// KEY_DOWN			0xD0
	VK_NEXT,		// KEY_PAGEDOWN		0xD1
	VK_INSERT,		// KEY_INSERT		0xD2
	VK_DELETE,		// KEY_DELETE		0xD3
	0x00,			//					0xD4
	0x00,			//					0xD5
	0x00,			//					0xD6
	0x00,			//					0xD7
	0x00,			//					0xD8
	0x00,			//					0xD9
	0x00,			//					0xDA
	VK_LWIN,		// KEY_LSYSTEM		0xDB
	VK_RWIN,		// KEY_RSYSTEM		0xDC
	VK_APPS,		// KEY_MENU			0xDD
};





#endif