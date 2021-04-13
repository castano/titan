


class DXInput : public IInputDevice {
	
	HWND					hWnd;
	HINSTANCE				hInstance;

	LIBRARY					di_module;

	LPDIRECTINPUT 			pDirectInput;

	LPDIRECTINPUTDEVICE2	pKeyboardDevice;
	LPDIRECTINPUTDEVICE2	pMouseDevice;

	DIMOUSESTATE			MouseState;
	DIDEVICEOBJECTDATA		BufferedInput[DINPUT_BUFFERSIZE];
	BYTE					KeyState[MAX_KEYBKEYS];		// buffer returned by GetDeviceState


	bool					bActive;

	input_caps				caps;

public:
	DXInput();

	int						Init(int flags);
	void					Shut(void);
	void					Refresh(void);

	void					Activate(bool on_off);

	int						GetValue(int device, int index);

	const input_caps *		GetInputCaps(void);

private:
	bool					InitKeyboard(void);
	bool					InitMouse(void);
	bool					InitOthers(int i);

};



