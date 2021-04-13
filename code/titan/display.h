/*===========================================================================
	Title: display.h
	Author: Ignacio Castaño Aguado
	Description: display management
	Classes:
		vidmode			: internal utility class
		IDisplay		: display interface
		Win32Display	: win32 display implementation
		XDisplay		: X Windows display implementation
	Cvars:
		r_overBrightBits
		v_with
		v_height
		v_bpp
		v_hz

===========================================================================*/



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern Console *console;
extern vars	*cvars;
extern Error *err;



/*---------------------------------------------------------------------------
	q3 gamma ramp:
---------------------------------------------------------------------------*/

// FIXME:	that was placed here for verification, the current gamma func is not the
//			same as q3 gamma func, but they are very similar
static const WORD GammaTable[]={
 0,
 2048,
 3584,
 4608,
 5632,
 6656,
 7680,
 8704,
 9728,
 10752,
 11776,
 12288,
 13312,
 14336,
 14848,
 15872,
 16384,
 17408,
 17920,
 18944,
 19456,
 19968,
 20992,
 21504,
 22528,
 23040,
 23552,
 24576,
 25088,
 25600,
 26112,
 27136,
 27648,
 28160,
 29184,
 29696,
 30208,
 30720,
 31232,
 32256,
 32768,
 33280,
 33792,
 34304,
 35328,
 35840,
 36352,
 36864,
 37376,
 37888,
 38400,
 39424,
 39936,
 40448,
 40960,
 41472,
 41984,
 42496,
 43008,
 43520,
 44032,
 45056,
 45568,
 46080,
 46592,
 47104,
 47616,
 48128,
 48640,
 49152,
 49664,
 50176,
 50688,
 51200,
 51712,
 52224,
 52736,
 53248,
 53760,
 54272,
 54784,
 55296,
 55808,
 56320,
 56832,
 57344,
 57856,
 58368,
 58880,
 59392,
 59904,
 60416,
 60928,
 61440,
 61952,
 62464,
 62976,
 63488,
 64000,
 64512,
 65024,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
 65280,
};




/*---------------------------------------------------------------------------
	vidmode:
---------------------------------------------------------------------------*/

// little class to save and set vidmode info easily
class vidmode {
public:
	inline vidmode(int w=640, int h=480, int b=16, int z=0){
		set(w, h, b, z);
	};
	
	inline void set(const int w, const int h, const int b, const int z){
		guard(vidmode::set);
		width=w;
		height=h;
		bpp=b;
		hz=z;
		unguard;
	};

	inline void set(const vidmode &vid){
		guard(vidmode::vidmode);
		width=vid.width;
		height=vid.height;
		bpp=vid.bpp;
		hz=vid.hz;
		unguard;
	}
	
	int width;
	int height;
	int bpp;
	int hz;
};


/*---------------------------------------------------------------------------
	Display Interface:
---------------------------------------------------------------------------*/

class __declspec(novtable) IDisplay {
public:
	virtual void init(void)=0;
	virtual void shut(void)=0;

	virtual void set(const vidmode &)=0;
	virtual void restore(void)=0;
	virtual int setGamma(float Gamma, float Overbright)=0;

	vidmode desk;
	vidmode last;
	vidmode current;
	vidmode def;
};




/*---------------------------------------------------------------------------
	Win32 Display:
---------------------------------------------------------------------------*/

#ifdef WIN32
class Win32Display : public IDisplay {
protected:

	// Saves the original gamma ramp
	WORD OldRamp[256*3];


public:

	//
	// Initializes the display
	//
	void init(void){
		guard(vidmode::init);
		// get info:
		HWND hDesktop = GetDesktopWindow();
		HDC desk_dc = GetDC(hDesktop);
		desk.set(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), GetDeviceCaps(desk_dc, PLANES) * GetDeviceCaps(desk_dc, BITSPIXEL), 0);
		last.set(desk);
		current.set(desk);
		GetDeviceGammaRamp(desk_dc, OldRamp);
		ReleaseDC(hDesktop, desk_dc);
		unguard;
	};


	//
	// Shutdown display
	//
	void shut(void){
		guard(vidmode::shut);
		restore();
		unguard;
	};
	

	//
	// Set a given video mode
	//
	void set(const vidmode &vid){
		guard(vidmode::set);
		LONG Error;//=DISP_CHANGE_FAILED;
		DEVMODE DevMode;
		DevMode.dmSize = sizeof(DevMode);
		DevMode.dmBitsPerPel = vid.bpp;
		DevMode.dmPelsWidth = vid.width;
		DevMode.dmPelsHeight = vid.height;
		DevMode.dmDisplayFrequency = vid.hz;

		console->Insert("--- Setting display\n");

		if(vid.hz!=0) DevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		else DevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		Error = ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN); 

		
		if(Error!=DISP_CHANGE_SUCCESSFUL && vid.hz!=0) {
			// Might be running in Windows95, let's try without the hertz change 
			//console->Insert("--- Trying without the hertz change\n");
			DevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
			Error = ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN); 
		}
	
		if(Error!=DISP_CHANGE_SUCCESSFUL) {
			// try with separate changes to bitdepth and resolution
			console->Insert("--- Trying with separate changes to bitdepth and resolution\n");
			DevMode.dmFields = DM_BITSPERPEL;
			Error = ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN);
			if(Error==DISP_CHANGE_SUCCESSFUL) {
				DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; 
				Error = ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN); 
			}
		}
		
		if(Error == DISP_CHANGE_SUCCESSFUL){
			last.set(current);
			current.set(vid);
			cvars->SetKeyValue("v_width", current.width);
			cvars->SetKeyValue("v_height", current.height);
			cvars->SetKeyValue("v_bpp", current.bpp);
			
			HWND hWnd=(HWND)cvars->IntForKey("wnd_handle");
			HDC dc = GetDC(hWnd);
			if(!dc) err->CriticalError(UNKNOWN_ERROR, "Error getting dc\n");
			current.hz = GetDeviceCaps( dc, VREFRESH );
			
			cvars->SetKeyValue("v_hz", current.hz);

			console->Insert("--- video mode %dx%dx%d at %d Hz set\n", current.width, current.height, current.bpp, current.hz);
			return;
		}
		else if(Error == DISP_CHANGE_RESTART) { 
			err->Log(UNKNOWN_ERROR, "You must change your video mode to %ldx%ldx%ld-bit color and then restart\n", vid.width, vid.height, vid.bpp);
		} else if(Error == DISP_CHANGE_BADMODE) {
			err->Log(UNKNOWN_ERROR, "The video mode %ldx%ldx%ld-bit color is not supported\n", vid.width, vid.height, vid.bpp);
		} else if(Error == DISP_CHANGE_FAILED) {
			err->Log(UNKNOWN_ERROR, "Hardware failed to change to %ldx%ldx%ld-bit color\n", vid.width, vid.height, vid.bpp);
		} else {
			err->Log(UNKNOWN_ERROR, "Failed to change to %ldx%ldx%ld-bit color - unknown reason\n", vid.width, vid.height, vid.bpp);
		}
		unguard;
	};
	
	
	//
	// Restore the original videomode
	//
	void restore(void){
		guard(vidmode::restore);
		ChangeDisplaySettings(0,0);
		current.set(desk);
		console->Insert("--- display restored\n");
		unguard;
	};

	
	//
	// Set a new gamma ramp given the gamma and the overbright
	//
	int setGamma(float Gamma, float Overbright){
		guard(vidmode::setGamma);

		HWND hWnd=(HWND)cvars->IntForKey("wnd_handle");
		HDC dc = GetDC(hWnd);
		if(!dc){
			err->CriticalError(UNKNOWN_ERROR, "Error getting dc\n");
		}
		
		WORD Ramp[256*3];

		float Brightness=0;

		for (int i=0 ; i<256 ; i++){
			float f = (float) (255 * pow ( (float)i/255.0 , 1/Gamma ));
			f = (f+Brightness) * Overbright;
			if (f < 0)
				f = 0;
			if (f > 255)
				f = 255;
			Ramp[i+0] = Ramp[i+256] = Ramp[i+512] = (WORD) (appRound(f)<<8);
		//	Ramp[i+0] = Ramp[i+256] = Ramp[i+512] = GammaTable[i];
		//	console->Insert("!!! %d, %d\n", Ramp[i]/256, GammaTable[i]/256);
		}

		if(SetDeviceGammaRamp(dc, Ramp)){
			cvars->SetKeyValue("r_overBrightBits", 1.0f);
			return 1;
		}
		else{
			cvars->SetKeyValue("r_overBrightBits", 0.0f);
			return 0;
		}
		unguard;
 	};


	//
	// Restore the initial gamma ramp
	//
	void restoreGamma(void){
		guard(vidmode::restoreGamma);
		
		HWND hWnd=(HWND)cvars->IntForKey("wnd_handle");
		HDC dc = GetDC(hWnd);
		if(!dc){
			err->CriticalError(UNKNOWN_ERROR, "Error getting dc");
		}
		SetDeviceGammaRamp(dc, OldRamp);
		unguard;
	};
};

#else // WIN32


/*---------------------------------------------------------------------------
	X Window Display:
---------------------------------------------------------------------------*/
class XDisplay : public IDisplay {
public:
	void init(void){};
	void shut(void){};

	void set(vidmode *){};
	int setGamma(float Gamma, float Overbright){};
};


#endif // WIN32
