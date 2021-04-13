/*===========================================================================
	Title: window.h
	Author: Ignacio Castaño Aguado
	Description: Window creation
		this class supports only one window, to enable more we have to have a 
		static list of hwnd and redirect the messages to the appropiate window.
	Cvars:
		v_fullscreen
		v_width
		v_height
		v_aspect
		v_gamma
		r_overBrightBits
		v_depthBits
		v_stencilBits
		v_mode

===========================================================================*/
#ifndef __WINDOW_H_
#define __WINDOW_H_


/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

#include "display.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern vars *cvars;
extern Console *console;
extern IInput *input;



static Var v_fullscreen("v_fullscreen", 0);
static Var v_width("v_width", 640);
static Var v_height("v_height", 480);
static Var v_aspect("v_aspect", (float)4/3);
static Var v_colorBits("v_colorBits", 16);
static Var v_hz("v_hz", 0);
static Var v_depthBits("v_depthBits", 32);
static Var v_stencilBits("v_stencilBits", 0);
static Var v_gamma("v_gamma", 1.3375f);
static Var v_mode("v_mode", 0);

static Var wnd_handle("wnd_handle", 0);

static Var r_overBrightBits("r_overBrightBits", 0.0f);



/*---------------------------------------------------------------------------
	Win32 Window Class
---------------------------------------------------------------------------*/
#ifdef WIN32
class Window {
public:
	// public window data
//	HWND wnd_handle;
	HDC wnd_dc;

private:
	
	// this pointer
	static Window *s_Wnd;	// to support multiple windows, that must to be a list

	// private window data
	HINSTANCE wnd_Instance;
	HCURSOR	wnd_cursor;
	
	// window properties
	vidmode wnd;			// those are the desired, but not the current!, current're saved in cvars
	
	
	// win32 display manager
	Win32Display display;


	//
	// Registers window class
	//
	void Register(void){
		guard(Window::Register);
		WNDCLASS cls = { CS_VREDRAW | CS_HREDRAW | CS_OWNDC, (WNDPROC)Window::StaticWindowProc,
                     0, 0, wnd_Instance, 0, 0, (HBRUSH)GetStockObject(BLACK_BRUSH),
                     0, _APP_ };
		RegisterClass(&cls);
		unguard;
	};


	//
	// Creates a fullscreen window
	//
	void CreateFullscreen(void){
		guard(Window::Register);
		wnd_handle = (int)CreateWindowEx(
			WS_EX_TOPMOST,					// some drivers could be slower if not topmost
			_APP_, _APP_,
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0,
			display.desk.width,
			display.desk.height,
			0, 0, wnd_Instance, 0
		);
		//cvars->SetKeyValue("wnd_handle", *((float *)((void *)&wnd_handle)));
		// width, height, bpp, and hz depend on the display, so they are set later
		unguard;
	};


	//
	// Creates a windowed window
	//
	void CreateWindowed(void){
		guard(Window::CreateWindowed);
		wnd_handle = (int)CreateWindowEx(
			WS_EX_APPWINDOW,
			_APP_, _APP_,
			WS_BORDER | WS_DLGFRAME | WS_CLIPSIBLINGS,
			0, 0,
			wnd.width,
			wnd.height,
			0, 0, wnd_Instance, 0
		);
		//cvars->SetKeyValue("wnd_handle", *((float *)((void *)&wnd_handle)));
		v_width=wnd.width; 
		v_height=wnd.height; 
		// bpp and hz are the same of the desktop
		v_colorBits=display.desk.bpp; 
		v_hz=display.desk.hz;
		SetWindowPos((HWND)wnd_handle.ivalue, 0, (display.desk.width-wnd.width)/2, 
			(display.desk.height-wnd.height)/2, wnd.width, wnd.height, SWP_SHOWWINDOW);

		unguard;
	};


	//
	// Idles the window, when we are in fullscreen
	//
	// if the app has to be minimized, minimize is set to true, if we are closing it is false
	void FullscreenIdle(int full, int minimize){
		guard(Window::FullscreenIdle);
		if(full && v_fullscreen.ivalue) {
			wnd_cursor=SetCursor(0);
			ShowWindow((HWND)wnd_handle.ivalue, SW_RESTORE);
			SetWindowPos((HWND)wnd_handle.ivalue, 0, 0, 0, display.current.width,
				display.current.height, SWP_SHOWWINDOW);
			display.set(wnd);
			SetWindowPos((HWND)wnd_handle.ivalue, 0, 0, 0, display.current.width,
				display.current.height, SWP_SHOWWINDOW);
		}
		else if(!full && v_fullscreen.ivalue) {
			SetWindowPos((HWND)wnd_handle.ivalue, 0, 0, 0, display.current.width,
				display.current.height, SWP_SHOWWINDOW);
			display.restore();
			SetWindowPos((HWND)wnd_handle.ivalue, 0, 0, 0, display.current.width,
				display.current.height, SWP_SHOWWINDOW);
			if(minimize) {           
				ShowWindow((HWND)wnd_handle.ivalue,SW_MINIMIZE);
			}
		}
		unguard;
	};

public:

	//
	// Constructor to initilize data members
	//
	Window(void){
		guard(Window::WindowProc);
		s_Wnd=this;
		wnd_dc=NULL;
		wnd.set(v_width.ivalue, v_height.ivalue, v_colorBits.ivalue, v_hz.ivalue);
		unguard;
	};


	//
	// Create a window wich properties depend on cvars
	//
	int create(void){
		guard(Window::create);
		console->Insert("--- Creating Window\n");
		
		wnd_Instance=(HINSTANCE) cvars->IntForKey("sys_instance");

		// register cvars
		cvars->RegisterVar(v_fullscreen);
		cvars->RegisterVar(v_width);
		cvars->RegisterVar(v_height);
		cvars->RegisterVar(v_colorBits);
		cvars->RegisterVar(v_hz);
		cvars->RegisterVar(v_depthBits);
		cvars->RegisterVar(v_stencilBits);
		cvars->RegisterVar(v_gamma);
		cvars->RegisterVar(wnd_handle);
		cvars->RegisterVar(r_overBrightBits);

		wnd.set(v_width.ivalue, v_height.ivalue, v_colorBits.ivalue, v_hz.ivalue);

		Register();			// register window class

		display.init();

		if(v_fullscreen.ivalue) {
			CreateFullscreen();	// create window
			SetWindowPos((HWND)wnd_handle.ivalue, 0, 0, 0, display.current.width,
				display.current.height, SWP_SHOWWINDOW);
			display.setGamma(v_gamma.fvalue, 2);
		}
		else {
			CreateWindowed();
		}
		
		if(!wnd_handle.ivalue){
			err->CriticalError(ERROR_CREATING_WINDOW, "Error creating window\n");
		}

		wnd_cursor=SetCursor(0);
		ShowCursor(0);

		ShowWindow((HWND)wnd_handle.ivalue, SW_SHOWNORMAL);
		SetCapture((HWND)wnd_handle.ivalue);

		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		wnd_dc = GetDC((HWND)wnd_handle.ivalue);

		
		return 1;
		unguard;
	};


	//
	// Destroy the window
	//
	void destroy(void){
		guard(Window::destroy);
		console->Insert("--- Destroying window\n");
		if(v_fullscreen.ivalue) {
			// restore display:
			display.restoreGamma();
			FullscreenIdle(false, false);
			//wnd_fullscreen=0;
			v_fullscreen=0;
		}
		
		ReleaseCapture();

		if(wnd_handle.ivalue) {
			DestroyWindow((HWND)wnd_handle.ivalue);
			wnd_handle=NULL;
		}

		UnregisterClass("Titan", wnd_Instance);
		unguard;
	};


	//
	// That will change the window to fullscreen, but currently it doesnt do anything
	//
	void setFullscreen(int full){
		guard(Window::setFullscreen);
		if(!wnd_handle.ivalue){
			v_fullscreen=full;
		}
		else{
			// destroy window
			v_fullscreen=full;
			// create window
		}
		unguard;
	};

	//
	// same as above, but with resolution
	//
	void setRes(int w, int h, int bpp, int hz){
		guard(Window::setRes);
		if(!wnd_handle.ivalue){
			wnd.set(w, h, bpp, hz);
		}
		else{
			// destroy window
			wnd.set(w, h, bpp, hz);
			// create window
		}
		unguard;
	};


	//
	// this is the static window proc that maps the messages to the window
	// now only supports one instance
	static LONG WINAPI StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		guard(Window::StaticWindowProc);
		if(s_Wnd){
			return s_Wnd->WindowProc(hWnd, uMsg, wParam, lParam);
		}
		else if(uMsg==WM_NCCREATE){
			wnd_handle=(int)lParam;
		}
		return 0;
		unguard;
	};


	//
	// this is the real window proc
	//
	// FIXME: catch alt tab?
	LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		guard(Window::StaticWindowProc);
		switch (uMsg){
			case WM_CLOSE:
				return 0;
			break;
			case WM_ACTIVATEAPP:
				if(wParam) {
					ShowCursor(0);
					SetCapture((HWND)wnd_handle.ivalue);
				}
				//if(wnd_fullscreen) FullscreenIdle(wParam,!wParam);
				if(!wParam) {
					SetCursor(wnd_cursor);
					ReleaseCapture();
				}
			break;

			case WM_ACTIVATE:
				if(v_fullscreen.ivalue) FullscreenIdle(wParam, !wParam);
			    if ( LOWORD(wParam)==WA_INACTIVE ) {
					//if(console) console->Insert("!!!  Inactiva\n");
					if(input) input->Activate(false);
					SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
				} else {
					//if(console) console->Insert("!!!  Activa\n");
					if(input) input->Activate(true);
					SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				}
			break;

			case WM_SETFOCUS:
				//if(input) input->Activate(true);
				break;
			case WM_KILLFOCUS:
				//if(input) input->Activate(false);
				break;

			// disable screen saver and monitor power down messages
			case WM_SYSCOMMAND:
				switch (wParam){
					case SC_SCREENSAVE:
					case SC_MONITORPOWER:
						return 0;
					break;
					default:
					break;
				}
			break;
		}
		// default action
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		unguard;
	}
};

// this pointer initialization
Window * Window::s_Wnd=NULL;

#else // WIN32



/*---------------------------------------------------------------------------
	X Window Class
---------------------------------------------------------------------------*/

class Window {
public:
	int create(void){
		guard(Window::create);
		unguard;
	};
	void destroy(void){
		guard(Window::destroy);
		unguard;
	};

	void setFullscreen(int full){
		guard(Window::setFullscreen);
		unguard;
	};

	void setRes(int w, int h, int bpp, int hz){
		guard(Window::setRes);
		unguard;
	};
};


#endif // WIN32



#endif __WINDOW_H_