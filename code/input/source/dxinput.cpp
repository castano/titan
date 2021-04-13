/*===========================================================================
	Title: dxinput.cpp
	Author: Ignacio Castaño Aguado
	Description: DirectX input device
	TODO: 
		- support multiple number of devices, create DInputDevice class that
		stores the com interface, device data and capabilities.
		- support multiple keyboards and multiple mouses.

===========================================================================*/
#ifdef WIN32
#include "input.h"
#include <initguid.h>





/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

// Then I have to define the data formats for device events. I'm not
// entirely sure why the high bit has to be set. Undocumented requirement?
#define DI_KEY_ID(code)	(0x80000000 | DIDFT_BUTTON | DIDFT_MAKEINSTANCE(code))




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern Framework Import;




/*---------------------------------------------------------------------------
	Constants:
---------------------------------------------------------------------------*/

static DIOBJECTDATAFORMAT DIKeyboardFmt[] = {
	{ &GUID_Key, 0,   DI_KEY_ID(0),   0L },	
	{ &GUID_Key, 1,   DI_KEY_ID(1),   0L },
	{ &GUID_Key, 2,   DI_KEY_ID(2),   0L },
	{ &GUID_Key, 3,   DI_KEY_ID(3),   0L },
	{ &GUID_Key, 4,   DI_KEY_ID(4),   0L },
	{ &GUID_Key, 5,   DI_KEY_ID(5),   0L },
	{ &GUID_Key, 6,   DI_KEY_ID(6),   0L },
	{ &GUID_Key, 7,   DI_KEY_ID(7),   0L },
	{ &GUID_Key, 8,   DI_KEY_ID(8),   0L },
	{ &GUID_Key, 9,   DI_KEY_ID(9),   0L },
	{ &GUID_Key, 10,  DI_KEY_ID(10),  0L },
	{ &GUID_Key, 11,  DI_KEY_ID(11),  0L },
	{ &GUID_Key, 12,  DI_KEY_ID(12),  0L },
	{ &GUID_Key, 13,  DI_KEY_ID(13),  0L },
	{ &GUID_Key, 14,  DI_KEY_ID(14),  0L },
	{ &GUID_Key, 15,  DI_KEY_ID(15),  0L },
	{ &GUID_Key, 16,  DI_KEY_ID(16),  0L },
	{ &GUID_Key, 17,  DI_KEY_ID(17),  0L },
	{ &GUID_Key, 18,  DI_KEY_ID(18),  0L },
	{ &GUID_Key, 19,  DI_KEY_ID(19),  0L },
	{ &GUID_Key, 20,  DI_KEY_ID(20),  0L },
	{ &GUID_Key, 21,  DI_KEY_ID(21),  0L },
	{ &GUID_Key, 22,  DI_KEY_ID(22),  0L },
	{ &GUID_Key, 23,  DI_KEY_ID(23),  0L },
	{ &GUID_Key, 24,  DI_KEY_ID(24),  0L },
	{ &GUID_Key, 25,  DI_KEY_ID(25),  0L },
	{ &GUID_Key, 26,  DI_KEY_ID(26),  0L },
	{ &GUID_Key, 27,  DI_KEY_ID(27),  0L },
	{ &GUID_Key, 28,  DI_KEY_ID(28),  0L },
	{ &GUID_Key, 29,  DI_KEY_ID(29),  0L },
	{ &GUID_Key, 30,  DI_KEY_ID(30),  0L },
	{ &GUID_Key, 31,  DI_KEY_ID(31),  0L },
	{ &GUID_Key, 32,  DI_KEY_ID(32),  0L },
	{ &GUID_Key, 33,  DI_KEY_ID(33),  0L },
	{ &GUID_Key, 34,  DI_KEY_ID(34),  0L },
	{ &GUID_Key, 35,  DI_KEY_ID(35),  0L },
	{ &GUID_Key, 36,  DI_KEY_ID(36),  0L },
	{ &GUID_Key, 37,  DI_KEY_ID(37),  0L },
	{ &GUID_Key, 38,  DI_KEY_ID(38),  0L },
	{ &GUID_Key, 39,  DI_KEY_ID(39),  0L },
	{ &GUID_Key, 40,  DI_KEY_ID(40),  0L },
	{ &GUID_Key, 41,  DI_KEY_ID(41),  0L },
	{ &GUID_Key, 42,  DI_KEY_ID(42),  0L },
	{ &GUID_Key, 43,  DI_KEY_ID(43),  0L },
	{ &GUID_Key, 44,  DI_KEY_ID(44),  0L },
	{ &GUID_Key, 45,  DI_KEY_ID(45),  0L },
	{ &GUID_Key, 46,  DI_KEY_ID(46),  0L },
	{ &GUID_Key, 47,  DI_KEY_ID(47),  0L },
	{ &GUID_Key, 48,  DI_KEY_ID(48),  0L },
	{ &GUID_Key, 49,  DI_KEY_ID(49),  0L },
	{ &GUID_Key, 50,  DI_KEY_ID(50),  0L },
	{ &GUID_Key, 51,  DI_KEY_ID(51),  0L },
	{ &GUID_Key, 52,  DI_KEY_ID(52),  0L },
	{ &GUID_Key, 53,  DI_KEY_ID(53),  0L },
	{ &GUID_Key, 54,  DI_KEY_ID(54),  0L },
	{ &GUID_Key, 55,  DI_KEY_ID(55),  0L },
	{ &GUID_Key, 56,  DI_KEY_ID(56),  0L },
	{ &GUID_Key, 57,  DI_KEY_ID(57),  0L },
	{ &GUID_Key, 58,  DI_KEY_ID(58),  0L },
	{ &GUID_Key, 59,  DI_KEY_ID(59),  0L },
	{ &GUID_Key, 60,  DI_KEY_ID(60),  0L },
	{ &GUID_Key, 61,  DI_KEY_ID(61),  0L },
	{ &GUID_Key, 62,  DI_KEY_ID(62),  0L },
	{ &GUID_Key, 63,  DI_KEY_ID(63),  0L },
	{ &GUID_Key, 64,  DI_KEY_ID(64),  0L },
	{ &GUID_Key, 65,  DI_KEY_ID(65),  0L },
	{ &GUID_Key, 66,  DI_KEY_ID(66),  0L },
	{ &GUID_Key, 67,  DI_KEY_ID(67),  0L },
	{ &GUID_Key, 68,  DI_KEY_ID(68),  0L },
	{ &GUID_Key, 69,  DI_KEY_ID(69),  0L },
	{ &GUID_Key, 70,  DI_KEY_ID(70),  0L },
	{ &GUID_Key, 71,  DI_KEY_ID(71),  0L },
	{ &GUID_Key, 72,  DI_KEY_ID(72),  0L },
	{ &GUID_Key, 73,  DI_KEY_ID(73),  0L },
	{ &GUID_Key, 74,  DI_KEY_ID(74),  0L },
	{ &GUID_Key, 75,  DI_KEY_ID(75),  0L },
	{ &GUID_Key, 76,  DI_KEY_ID(76),  0L },
	{ &GUID_Key, 77,  DI_KEY_ID(77),  0L },
	{ &GUID_Key, 78,  DI_KEY_ID(78),  0L },
	{ &GUID_Key, 79,  DI_KEY_ID(79),  0L },
	{ &GUID_Key, 80,  DI_KEY_ID(80),  0L },
	{ &GUID_Key, 81,  DI_KEY_ID(81),  0L },
	{ &GUID_Key, 82,  DI_KEY_ID(82),  0L },
	{ &GUID_Key, 83,  DI_KEY_ID(83),  0L },
	{ &GUID_Key, 84,  DI_KEY_ID(84),  0L },
	{ &GUID_Key, 85,  DI_KEY_ID(85),  0L },
	{ &GUID_Key, 86,  DI_KEY_ID(86),  0L },
	{ &GUID_Key, 87,  DI_KEY_ID(87),  0L },
	{ &GUID_Key, 88,  DI_KEY_ID(88),  0L },
	{ &GUID_Key, 89,  DI_KEY_ID(89),  0L },
	{ &GUID_Key, 90,  DI_KEY_ID(90),  0L },
	{ &GUID_Key, 91,  DI_KEY_ID(91),  0L },
	{ &GUID_Key, 92,  DI_KEY_ID(92),  0L },
	{ &GUID_Key, 93,  DI_KEY_ID(93),  0L },
	{ &GUID_Key, 94,  DI_KEY_ID(94),  0L },
	{ &GUID_Key, 95,  DI_KEY_ID(95),  0L },
	{ &GUID_Key, 96,  DI_KEY_ID(96),  0L },
	{ &GUID_Key, 97,  DI_KEY_ID(97),  0L },
	{ &GUID_Key, 98,  DI_KEY_ID(98),  0L },
	{ &GUID_Key, 99,  DI_KEY_ID(99),  0L },
	{ &GUID_Key, 100, DI_KEY_ID(100), 0L },
	{ &GUID_Key, 101, DI_KEY_ID(101), 0L },
	{ &GUID_Key, 102, DI_KEY_ID(102), 0L },
	{ &GUID_Key, 103, DI_KEY_ID(103), 0L },
	{ &GUID_Key, 104, DI_KEY_ID(104), 0L },
	{ &GUID_Key, 105, DI_KEY_ID(105), 0L },
	{ &GUID_Key, 106, DI_KEY_ID(106), 0L },
	{ &GUID_Key, 107, DI_KEY_ID(107), 0L },
	{ &GUID_Key, 108, DI_KEY_ID(108), 0L },
	{ &GUID_Key, 109, DI_KEY_ID(109), 0L },
	{ &GUID_Key, 110, DI_KEY_ID(110), 0L },
	{ &GUID_Key, 111, DI_KEY_ID(111), 0L },
	{ &GUID_Key, 112, DI_KEY_ID(112), 0L },
	{ &GUID_Key, 113, DI_KEY_ID(113), 0L },
	{ &GUID_Key, 114, DI_KEY_ID(114), 0L },
	{ &GUID_Key, 115, DI_KEY_ID(115), 0L },
	{ &GUID_Key, 116, DI_KEY_ID(116), 0L },
	{ &GUID_Key, 117, DI_KEY_ID(117), 0L },
	{ &GUID_Key, 118, DI_KEY_ID(118), 0L },
	{ &GUID_Key, 119, DI_KEY_ID(119), 0L },
	{ &GUID_Key, 120, DI_KEY_ID(120), 0L },
	{ &GUID_Key, 121, DI_KEY_ID(121), 0L },
	{ &GUID_Key, 122, DI_KEY_ID(122), 0L },
	{ &GUID_Key, 123, DI_KEY_ID(123), 0L },
	{ &GUID_Key, 124, DI_KEY_ID(124), 0L },
	{ &GUID_Key, 125, DI_KEY_ID(125), 0L },
	{ &GUID_Key, 126, DI_KEY_ID(126), 0L },
	{ &GUID_Key, 127, DI_KEY_ID(127), 0L },
	{ &GUID_Key, 128, DI_KEY_ID(128), 0L },
	{ &GUID_Key, 129, DI_KEY_ID(129), 0L },
	{ &GUID_Key, 130, DI_KEY_ID(130), 0L },
	{ &GUID_Key, 131, DI_KEY_ID(131), 0L },
	{ &GUID_Key, 132, DI_KEY_ID(132), 0L },
	{ &GUID_Key, 133, DI_KEY_ID(133), 0L },
	{ &GUID_Key, 134, DI_KEY_ID(134), 0L },
	{ &GUID_Key, 135, DI_KEY_ID(135), 0L },
	{ &GUID_Key, 136, DI_KEY_ID(136), 0L },
	{ &GUID_Key, 137, DI_KEY_ID(137), 0L },
	{ &GUID_Key, 138, DI_KEY_ID(138), 0L },
	{ &GUID_Key, 139, DI_KEY_ID(139), 0L },
	{ &GUID_Key, 140, DI_KEY_ID(140), 0L },
	{ &GUID_Key, 141, DI_KEY_ID(141), 0L },
	{ &GUID_Key, 142, DI_KEY_ID(142), 0L },
	{ &GUID_Key, 143, DI_KEY_ID(143), 0L },
	{ &GUID_Key, 144, DI_KEY_ID(144), 0L },
	{ &GUID_Key, 145, DI_KEY_ID(145), 0L },
	{ &GUID_Key, 146, DI_KEY_ID(146), 0L },
	{ &GUID_Key, 147, DI_KEY_ID(147), 0L },
	{ &GUID_Key, 148, DI_KEY_ID(148), 0L },
	{ &GUID_Key, 149, DI_KEY_ID(149), 0L },
	{ &GUID_Key, 150, DI_KEY_ID(150), 0L },
	{ &GUID_Key, 151, DI_KEY_ID(151), 0L },
	{ &GUID_Key, 152, DI_KEY_ID(152), 0L },
	{ &GUID_Key, 153, DI_KEY_ID(153), 0L },
	{ &GUID_Key, 154, DI_KEY_ID(154), 0L },
	{ &GUID_Key, 155, DI_KEY_ID(155), 0L },
	{ &GUID_Key, 156, DI_KEY_ID(156), 0L },
	{ &GUID_Key, 157, DI_KEY_ID(157), 0L },
	{ &GUID_Key, 158, DI_KEY_ID(158), 0L },
	{ &GUID_Key, 159, DI_KEY_ID(159), 0L },
	{ &GUID_Key, 160, DI_KEY_ID(160), 0L },
	{ &GUID_Key, 161, DI_KEY_ID(161), 0L },
	{ &GUID_Key, 162, DI_KEY_ID(162), 0L },
	{ &GUID_Key, 163, DI_KEY_ID(163), 0L },
	{ &GUID_Key, 164, DI_KEY_ID(164), 0L },
	{ &GUID_Key, 165, DI_KEY_ID(165), 0L },
	{ &GUID_Key, 166, DI_KEY_ID(166), 0L },
	{ &GUID_Key, 167, DI_KEY_ID(167), 0L },
	{ &GUID_Key, 168, DI_KEY_ID(168), 0L },
	{ &GUID_Key, 169, DI_KEY_ID(169), 0L },
	{ &GUID_Key, 170, DI_KEY_ID(170), 0L },
	{ &GUID_Key, 171, DI_KEY_ID(171), 0L },
	{ &GUID_Key, 172, DI_KEY_ID(172), 0L },
	{ &GUID_Key, 173, DI_KEY_ID(173), 0L },
	{ &GUID_Key, 174, DI_KEY_ID(174), 0L },
	{ &GUID_Key, 175, DI_KEY_ID(175), 0L },
	{ &GUID_Key, 176, DI_KEY_ID(176), 0L },
	{ &GUID_Key, 177, DI_KEY_ID(177), 0L },
	{ &GUID_Key, 178, DI_KEY_ID(178), 0L },
	{ &GUID_Key, 179, DI_KEY_ID(179), 0L },
	{ &GUID_Key, 180, DI_KEY_ID(180), 0L },
	{ &GUID_Key, 181, DI_KEY_ID(181), 0L },
	{ &GUID_Key, 182, DI_KEY_ID(182), 0L },
	{ &GUID_Key, 183, DI_KEY_ID(183), 0L },
	{ &GUID_Key, 184, DI_KEY_ID(184), 0L },
	{ &GUID_Key, 185, DI_KEY_ID(185), 0L },
	{ &GUID_Key, 186, DI_KEY_ID(186), 0L },
	{ &GUID_Key, 187, DI_KEY_ID(187), 0L },
	{ &GUID_Key, 188, DI_KEY_ID(188), 0L },
	{ &GUID_Key, 189, DI_KEY_ID(189), 0L },
	{ &GUID_Key, 190, DI_KEY_ID(190), 0L },
	{ &GUID_Key, 191, DI_KEY_ID(191), 0L },
	{ &GUID_Key, 192, DI_KEY_ID(192), 0L },
	{ &GUID_Key, 193, DI_KEY_ID(193), 0L },
	{ &GUID_Key, 194, DI_KEY_ID(194), 0L },
	{ &GUID_Key, 195, DI_KEY_ID(195), 0L },
	{ &GUID_Key, 196, DI_KEY_ID(196), 0L },
	{ &GUID_Key, 197, DI_KEY_ID(197), 0L },
	{ &GUID_Key, 198, DI_KEY_ID(198), 0L },
	{ &GUID_Key, 199, DI_KEY_ID(199), 0L },
	{ &GUID_Key, 200, DI_KEY_ID(200), 0L },
	{ &GUID_Key, 201, DI_KEY_ID(201), 0L },
	{ &GUID_Key, 202, DI_KEY_ID(202), 0L },
	{ &GUID_Key, 203, DI_KEY_ID(203), 0L },
	{ &GUID_Key, 204, DI_KEY_ID(204), 0L },
	{ &GUID_Key, 205, DI_KEY_ID(205), 0L },
	{ &GUID_Key, 206, DI_KEY_ID(206), 0L },
	{ &GUID_Key, 207, DI_KEY_ID(207), 0L },
	{ &GUID_Key, 208, DI_KEY_ID(208), 0L },
	{ &GUID_Key, 209, DI_KEY_ID(209), 0L },
	{ &GUID_Key, 210, DI_KEY_ID(210), 0L },
	{ &GUID_Key, 211, DI_KEY_ID(211), 0L },
	{ &GUID_Key, 212, DI_KEY_ID(212), 0L },
	{ &GUID_Key, 213, DI_KEY_ID(213), 0L },
	{ &GUID_Key, 214, DI_KEY_ID(214), 0L },
	{ &GUID_Key, 215, DI_KEY_ID(215), 0L },
	{ &GUID_Key, 216, DI_KEY_ID(216), 0L },
	{ &GUID_Key, 217, DI_KEY_ID(217), 0L },
	{ &GUID_Key, 218, DI_KEY_ID(218), 0L },
	{ &GUID_Key, 219, DI_KEY_ID(219), 0L },
	{ &GUID_Key, 220, DI_KEY_ID(220), 0L },
	{ &GUID_Key, 221, DI_KEY_ID(221), 0L },
	{ &GUID_Key, 222, DI_KEY_ID(222), 0L },
	{ &GUID_Key, 223, DI_KEY_ID(223), 0L },
	{ &GUID_Key, 224, DI_KEY_ID(224), 0L },
	{ &GUID_Key, 225, DI_KEY_ID(225), 0L },
	{ &GUID_Key, 226, DI_KEY_ID(226), 0L },
	{ &GUID_Key, 227, DI_KEY_ID(227), 0L },
	{ &GUID_Key, 228, DI_KEY_ID(228), 0L },
	{ &GUID_Key, 229, DI_KEY_ID(229), 0L },
	{ &GUID_Key, 230, DI_KEY_ID(230), 0L },
	{ &GUID_Key, 231, DI_KEY_ID(231), 0L },
	{ &GUID_Key, 232, DI_KEY_ID(232), 0L },
	{ &GUID_Key, 233, DI_KEY_ID(233), 0L },
	{ &GUID_Key, 234, DI_KEY_ID(234), 0L },
	{ &GUID_Key, 235, DI_KEY_ID(235), 0L },
	{ &GUID_Key, 236, DI_KEY_ID(236), 0L },
	{ &GUID_Key, 237, DI_KEY_ID(237), 0L },
	{ &GUID_Key, 238, DI_KEY_ID(238), 0L },
	{ &GUID_Key, 239, DI_KEY_ID(239), 0L },
	{ &GUID_Key, 240, DI_KEY_ID(240), 0L },
	{ &GUID_Key, 241, DI_KEY_ID(241), 0L },
	{ &GUID_Key, 242, DI_KEY_ID(242), 0L },
	{ &GUID_Key, 243, DI_KEY_ID(243), 0L },
	{ &GUID_Key, 244, DI_KEY_ID(244), 0L },
	{ &GUID_Key, 245, DI_KEY_ID(245), 0L },
	{ &GUID_Key, 246, DI_KEY_ID(246), 0L },
	{ &GUID_Key, 247, DI_KEY_ID(247), 0L },
	{ &GUID_Key, 248, DI_KEY_ID(248), 0L },
	{ &GUID_Key, 249, DI_KEY_ID(249), 0L },
	{ &GUID_Key, 250, DI_KEY_ID(250), 0L },
	{ &GUID_Key, 251, DI_KEY_ID(251), 0L },
	{ &GUID_Key, 252, DI_KEY_ID(252), 0L },
	{ &GUID_Key, 253, DI_KEY_ID(253), 0L },
	{ &GUID_Key, 254, DI_KEY_ID(254), 0L },
	{ &GUID_Key, 255, DI_KEY_ID(255), 0L },
};

static DIOBJECTDATAFORMAT DIMouseFmt[] = {
	{ &GUID_XAxis, 0,  0x00FFFF03, 0L },
	{ &GUID_YAxis, 4,  0x00FFFF03, 0L },
	{ &GUID_ZAxis, 8,  0x80FFFF03, 0L },
	{ NULL,        12, 0x00FFFF0C, 0L },
	{ NULL,        13, 0x00FFFF0C, 0L },
	{ NULL,        14, 0x80FFFF0C, 0L },
	{ NULL,        15, 0x80FFFF0C, 0L },
};

const DIDATAFORMAT c_dfDIKeyboard = { 24, 16, 2, 256, 256, DIKeyboardFmt };
const DIDATAFORMAT c_dfDIMouse = { 24, 16, 2, 16, 7, DIMouseFmt };





/*---------------------------------------------------------------------------
	DXInput
---------------------------------------------------------------------------*/

DXInput::DXInput(void){
	pDirectInput=NULL;
	bActive=false;

	pKeyboardDevice=NULL;
	pMouseDevice=NULL;

	//MouseState=?;

	for(int i=0; i<MAX_KEYBKEYS; i++) KeyState[i]=0;

	caps.joystick_axis=0;
	caps.joystick_buttons=0;
	caps.keyboard_keys=0;
	caps.keybuffer=0;
	caps.mouse_axis=0;
	caps.mouse_buttons=0;
	caps.num_joystick=0;
	caps.num_keyboard=0;
	caps.num_mouse=0;
};

int DXInput::Init(int flags){

	// Obtain app instance and window handle
//	float fTemp=Import.cvars->ValueForKey("hInstance");
//	hInstance=*((HINSTANCE *)((void*)&fTemp));
//	fTemp=Import.cvars->ValueForKey("wnd_handle");
//	hWnd=*((HWND *)((void*)&fTemp));

	hInstance=(HINSTANCE)Import.cvars->IntForKey("sys_instance");
	hWnd=(HWND)Import.cvars->IntForKey("wnd_handle");

	// Try to load DINPUT.DLL, which may or may not exist on this system
	di_module = TLoadLibrary("DINPUT");
	if (di_module == NULL) {
		Import.console->Insert("*** Coudn't load DINPUT.DLL\n");
		return 0;
	}

	// Try to locate the COM factory function for the interface
	HRESULT (CALLBACK* Factory)(HINSTANCE, DWORD, LPDIRECTINPUT*, LPUNKNOWN);
	*((void**)&Factory) = GetProcAddress(di_module, "DirectInputCreateA");
	if (Factory == NULL) {
		Import.console->Insert("*** Coudn't locate COM factory for directInput\n");
		return 0;
	}

	// Call the factory function to create the interface
	HRESULT hr;
	hr = Factory(hInstance, DIRECTINPUT_VERSION, &pDirectInput, NULL);
	if (FAILED(hr)) {
		Import.console->Insert("*** Error creating directInput instance\n");
		return 0;
	}


	// Create direct input object
	/*if(FAILED(DirectInputCreate( hInstance, DIRECTINPUT_VERSION, (LPDIRECTINPUT *)&pDirectInput, NULL ))){
		Import.console->Insert("*** Error creating direct input object\n");
		return 0;
	}*/


	if(flags&ID_KEYBOARD){
		if(!InitKeyboard()){
			Import.console->Insert("*** Error creating keyboard input device\n");
			flags^=ID_KEYBOARD;		// toggle keyboard flag
		}
	}

	if(flags&ID_MOUSE){
		if(!InitMouse()){
			Import.console->Insert("*** Error creating mouse input device\n");
			flags^=ID_MOUSE;		// toggle mouse flag			
		}
	}
		
/*	if(flags&ID_OTHER){
		if(!InitOthers(0)){
			Import.console->Insert("--- No additional input devices found\n");
		} 
		else flags^=ID_OTHER;	// toggle other flag
	}
*/

	Activate(true);

	return flags;
};

void DXInput::Shut(void){

	if( pKeyboardDevice ){
		delete KeyBuff; KeyBuff=NULL;
		pKeyboardDevice->Unacquire();
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
	}

	if( pMouseDevice ){
		pMouseDevice->Unacquire();
		pMouseDevice->Release();
		pMouseDevice=NULL;
	}
	
	if( pDirectInput ){
		pDirectInput->Release();
		pDirectInput = NULL;
	}

	if (di_module != NULL){
		TUnloadLibrary(di_module);
		di_module = NULL;
	}


}


const input_caps *DXInput::GetInputCaps(void){
	return &caps;
};


bool DXInput::InitKeyboard(void){
	HRESULT hr;

	// Create device for the keyboard
	hr=pDirectInput->CreateDevice(GUID_SysKeyboard,(struct IDirectInputDevice **)&(pKeyboardDevice),NULL);
	if(FAILED(hr)){
		Import.error->Debug(-1, "CreateDevice for GUID_SysKeyboard failed");
		return false;
	} 


	// set the data format for this device
	hr=pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr)){
		Import.error->Debug(-1, "SetDataFormat for the keyboard failed");
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
		return false;
	} 


	// set cooperative level
	hr=pKeyboardDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
	if(FAILED(hr)){
		Import.error->Debug(-1, "SetCooperativeLevel failed");
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
		return false;
	} 


    DIPROPDWORD dipdw =
		{
			{
                sizeof(DIPROPDWORD),        // diph.dwSize
                sizeof(DIPROPHEADER),       // diph.dwHeaderSize
                0,                          // diph.dwObj
                DIPH_DEVICE,                // diph.dwHow
            },
			DINPUT_BUFFERSIZE,              // dwData
        };

    hr = pKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
    if (FAILED(hr)) {
		Import.error->Debug(-1, "SetProperty failed");
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
        return false;
    }

	KeyBuff=new KeyBuffer();
	KeyBuff->Clear();

	caps.num_keyboard++;

	return true;
}


bool DXInput::InitMouse(void){
	HRESULT hr;


	// Create device for the keyboard
	hr=pDirectInput->CreateDevice(GUID_SysMouse, (struct IDirectInputDevice **)&(pMouseDevice),NULL);
	if(FAILED(hr)){
		Import.error->Debug(-1, "CreateDevice for GUID_SysMouse failed\n");
		return false;
	} 


	// set the data format for this device
	hr=pMouseDevice->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr)){
		Import.error->Debug(-1, "SetDataFormat for the mouse failed\n");
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
		return false;
	} 

	// set cooperative level
	if(Import.cvars->ValueForKey("v_fullscreen")==1)
		hr=pMouseDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND|DISCL_EXCLUSIVE);
	else 
		hr=pMouseDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
	if(FAILED(hr)){
		Import.error->Debug(-1, "SetCooperativeLevel failed");
		pKeyboardDevice->Release();
		pKeyboardDevice=NULL;
		return false;
	} 

	caps.num_mouse++;
	
	return true;
}

bool DXInput::InitOthers(int i){
	return false;
}


void DXInput::Refresh(void){
	HRESULT hr;
	DWORD size;
	INT	succeed;

	if(!bActive) return;

	if( pKeyboardDevice ) {

		succeed=false;
		do {
			hr = pKeyboardDevice->GetDeviceState(MAX_KEYBKEYS, KeyState);
			if(FAILED( hr )){
				hr = pKeyboardDevice->Acquire();
				if(FAILED( hr )){
					pKeyboardDevice->Unacquire();
					pKeyboardDevice->Release();
					pKeyboardDevice=NULL;

					delete KeyBuff;

					if(!InitKeyboard()) 
						Import.error->CriticalError(UNKNOWN_ERROR, "Critical DirectInput Error");
					
					pKeyboardDevice->Acquire();
				}
			}
			else 
				succeed=true;

		}while(!succeed);


		succeed=false;
		do {
			size = DINPUT_BUFFERSIZE;
			hr = pKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), BufferedInput, &size, 0);
			if(FAILED( hr )){
				hr = pKeyboardDevice->Acquire();
				if(FAILED( hr )){
					pKeyboardDevice->Unacquire();
					pKeyboardDevice->Release();
					pKeyboardDevice=NULL;

					delete KeyBuff;

					if(!InitKeyboard()) 
						Import.error->CriticalError(UNKNOWN_ERROR, "Critical DirectInput Error");
						
					pKeyboardDevice->Acquire();
				}
			}
			else 
				succeed=true;

		}while(!succeed);

		if ( size > 0) {
			for (DWORD i = 0; i < size; i++) {
				if(BufferedInput[i].dwData & 0x80)				// pressing
					KeyBuff->ProcessKey(KeyState, BufferedInput[i].dwOfs);
			}
		}

	}


	if( pMouseDevice ) {

		succeed=false;
		do {
			hr = pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState);
			if(FAILED( hr )){
				hr = pMouseDevice->Acquire();
				if(FAILED( hr )){
					pMouseDevice->Unacquire();
					pMouseDevice->Release();
					pMouseDevice=NULL;

					delete KeyBuff;

					if(!InitMouse()) 
						Import.error->CriticalError(UNKNOWN_ERROR, "Critical DirectInput Error");
					
					pMouseDevice->Acquire();
				}
			}
			else 
				succeed=true;

		}while(!succeed);
		
	}

}


void DXInput::Activate(bool on_off){
	bActive=on_off;

	if(on_off){
		if( pKeyboardDevice ) pKeyboardDevice->Acquire();
		if( pMouseDevice )	pMouseDevice->Acquire();
	}
	else {
		if( pKeyboardDevice ) pKeyboardDevice->Unacquire();
		if( pMouseDevice ) pMouseDevice->Unacquire();
	}

}


int DXInput::GetValue(int device, int slot){
	if(device==ID_KEYBOARD && pKeyboardDevice)	return KeyState[slot];
	else if(device==ID_MOUSE && pMouseDevice) {
		switch(slot) {
			case MV_XAXIS:
				return -MouseState.lX;
			case MV_YAXIS:
				return -MouseState.lY;
			case MV_ZAXIS:
				return -MouseState.lZ;
			case MV_BUTTON1:
				return (MouseState.rgbButtons[slot]&0x80)?1:0;
			case MV_BUTTON2:
				return (MouseState.rgbButtons[slot]&0x80)?1:0;
			case MV_BUTTON3:
				return (MouseState.rgbButtons[slot]&0x80)?1:0;
			case MV_BUTTON4:
				return (MouseState.rgbButtons[slot]&0x80)?1:0;
			case MV_WHEELUP:
				return MouseState.lZ>0;
			case MV_WHEELDOWN:
				return MouseState.lZ<0;
			default:
				return 0;
		}
	}
	else return 0;
}


#endif