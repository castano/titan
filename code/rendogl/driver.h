/*===========================================================================
	Title: Driver.h
	Author: Ignacio Castaño Aguado
	Description: Dinamically loads the opengl driver using the 
	Classes:
		Driver
	TODO:
		- clear up the initialization
		- document the cvars used
		- two step shutdown
		- solve problems with voodoo2 drivers
===========================================================================*/

#include "render.h"




/*---------------------------------------------------------------------------
	Globals:
--------------------------------------------------------------------------*/
extern Framework Import;

extern PFNGLLOCKARRAYSEXTPROC				glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC				glUnlockArraysEXT;

extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;

extern PFNWGLSWAPINTERVALPROC				wglSwapIntervalEXT;
extern PFNWGLGETSWAPINTERVALPROC			wglGetSwapIntervalEXT;

extern Var									gl_maxTextureSize;
extern Var									gl_maxTextureUnits;
extern Var									gl_acceleration;


/*---------------------------------------------------------------------------
	Driver:
---------------------------------------------------------------------------*/

class Driver {
public:

	bool	ext_compiledVertexArray;		// BitFields ?
	bool	ext_multitexture;
	bool	ext_swapControl;


private:
	
	//
	//	gls related members
	//
	int		glsNumDrivers;
	int		glsDriverIndex;
	DWORD	glsBehavior;
	gls_driver_info *DriverInfos;
	
	//
	//	pixel format related members
	//
	int		pdf_rgbbits;
	int		pdf_zbits;
	int		pdf_flags;
	
	//
	//	application data
	//
	HWND	app_wnd; // the only way to update app_wnd is to create the driver again
	HDC		app_dc;
	HGLRC	app_rc;
	
	static HGLRC global_rc;
	
	
	//
	//	Setup PixelFormat
	//
	void SetupPixelFormat(void){
		PIXELFORMATDESCRIPTOR FormatDescriptor = {
			sizeof (PIXELFORMATDESCRIPTOR), 1,
			pdf_flags, PFD_TYPE_RGBA, pdf_rgbbits,
			0,0, 0,0, 0,0, 0,0,     // rgb & shifts
			0, 0,0,0,0,             // accumulation buffer
			pdf_zbits,
			0, 0,                   // stencil & aux buffers
			PFD_MAIN_PLANE,
			0,                      // reserved
			0,0,0                   // masks
		};
		int FormatIndex = glsChoosePixelFormat(app_dc, &FormatDescriptor);
		glsSetPixelFormat(app_dc, FormatIndex, &FormatDescriptor);
		glsDescribePixelFormat(app_dc, FormatIndex, sizeof(FormatDescriptor), &FormatDescriptor);
		app_rc = glsCreateContext(app_dc);
		if(!global_rc){
			global_rc=app_rc;
		}

		int unsigned const AccelerationFlags =
			((!!(FormatDescriptor.dwFlags & PFD_GENERIC_ACCELERATED))<<1) |
			(!!(FormatDescriptor.dwFlags & PFD_GENERIC_FORMAT));
		char const *apAcceleration[4] =
		{//   00      01         10     11
			"ICD", "None", "Unknown", "MCD"
		};
		
		//Import.cvars->SetKeyValue("gl_acceleration", apAcceleration[AccelerationFlags]);
		gl_acceleration=apAcceleration[AccelerationFlags];
	};


	/* Determine if an extension is supported, must be called after a valid
	 * context has been created */
	bool CheckExtension(char *extName) {
		char *extensions = (char *)glGetString(GL_EXTENSIONS);
		if (strstr(extensions, extName)) return true;
		return false;
	};


public:

	//
	//	Initialization
	//
	int init(void){
		int temp, i;
		char *bestdriver;
		gls_error glsError;

		DriverInfos=0;
		glsDriverIndex=-1;


		Import.console->Insert("--- Initializing driver\n");
		// initialize values:
		// TODO: initialize from cvars
		pdf_rgbbits=16;
		pdf_zbits=32;
		pdf_flags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

		// set behavior
		glsBehavior=glsGetBehavior();
		if(Import.cvars->IntForKey("gl_load_glu")==0){
			glsSetBehavior(glsBehavior|=GLS_BEHAVIOR_NEVER_LOAD_GLU);
		}

		// get driver infos
		glsNumDrivers = glsGetNumberOfDrivers();
		if(DriverInfos) {
			delete [] DriverInfos;
			DriverInfos=NULL;
		}
		DriverInfos = new gls_driver_info[glsNumDrivers];

		for(i=0; i<glsNumDrivers; i++){
			glsGetDriverInfo( i, &DriverInfos[i] );
		}


		app_wnd=(HWND)Import.cvars->IntForKey("wnd_handle");
		//Import.console->Insert("--- wnd_handle: %i\n", app_wnd);

		// setup pixel format
		app_dc = GetDC(app_wnd);
		if(!app_dc){
			// critical error!
		}
		
	

		// if an specific driver was requested, load it directly
		bestdriver=Import.cvars->StringForKey("gl_driver");
		if(bestdriver && *bestdriver){
			for(i=0; i<glsNumDrivers; i++){
				if(!pathcmp(f_Name(DriverInfos[i].GLDriver.aDriverFilePath), bestdriver)){
					glsDriverIndex=i;
					goto load_driver;
				}
			}
		}

		// else load the first accelerated driver found
		for(i=0; i<glsNumDrivers; i++){
			if(!(DriverInfos[i].DriverFlags&GLS_FLAGS_SOFTWARE_ONLY) ||
				pathcmp(f_Name(DriverInfos[i].GLDriver.aDriverFilePath), "opengl.dll")) {
				glsDriverIndex=i;
				goto load_driver;
			}
		}

		// else load any other driver
		if(Import.cvars->ValueForKey("gl_allow_software")){
			// try to load the sgi driver
			for(i=0; i<glsNumDrivers; i++){
				if(!pathcmp(f_Name(DriverInfos[i].GLDriver.aDriverFilePath), "opengl.dll")){
					glsDriverIndex=i;
					goto load_driver;
				}
			}
			
			// or any other driver distinct to microsoft driver
			/*for(i=0; i<glsNumDrivers; i++){
				if(!(DriverInfos[i].DriverFlags&GLS_FLAGS_SOFTWARE_ONLY)) {
					glsDriverIndex=i;
					goto load_driver;
				}
			}*/
			
			// dummy:
			for(i=0; i<glsNumDrivers; i++){
				glsDriverIndex=i;
				goto load_driver;
			}
		}
		
		if(glsDriverIndex==-1){
			Import.console->Insert("*** No OpenGL driver found\n");
			return 0;
		}

load_driver:
		glsError = glsLoadDriver(glsDriverIndex);

		if(glsError!=GLS_ERROR_OK){
			Import.console->Insert("*** No valid OpenGL driver found\n");
			shut();
			return 0;
		}

		SetupPixelFormat();
		Lock();

		if( Import.cvars->ValueForKey("gl_allow_software")==0.0f &&
			(!strcmp(gl_acceleration.svalue,"None") ||
			 !strcmp((char *)glGetString(GL_RENDERER),"GDI Generic")) ){
				
			glsMakeCurrent(0,0);
			// TODO: critical error !!!
			Import.console->Insert("*** No accelerated OpenGL driver found\n");
			shut();
			return 0;
		}



		Import.console->Insert("--- Using %s %s\n", DriverInfos[glsDriverIndex].aDriverDescription, DriverInfos[glsDriverIndex].GLDriver.aDriverFilePath);

		// driver seems to be valid, but there could be problems with fullscreen only drivers,
		// and with drivers that need a 16 bit color display.

		Import.cvars->SetKeyValue("gl_vendor", (char *)glGetString(GL_VENDOR));
		Import.cvars->SetKeyValue("gl_renderer", (char *)glGetString(GL_RENDERER));
		Import.cvars->SetKeyValue("gl_version", (char *)glGetString(GL_VERSION));
		Import.cvars->SetKeyValue("gl_extensions", (char *)glGetString(GL_EXTENSIONS));
		
			
		// Initialize extensions
		if(CheckExtension("GL_EXT_compiled_vertex_array")){
			glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) glsGetProcAddress("glLockArraysEXT");
			glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) glsGetProcAddress("glUnlockArraysEXT");
			ext_compiledVertexArray=true;
			Import.cvars->SetKeyValue("r_ext_compiled_vertex_array", 1);
		}
		else ext_compiledVertexArray=0;

		if(CheckExtension("WGL_EXT_swap_control")){
			wglSwapIntervalEXT    = (PFNWGLSWAPINTERVALPROC) glsGetProcAddress("wglSwapIntervalEXT");
			wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALPROC) glsGetProcAddress("wglGetSwapIntervalEXT");
			ext_swapControl=1;
			if(Import.cvars->ValueForKey("r_swapInterval")==0){
				wglSwapIntervalEXT(0);
			}
			else wglSwapIntervalEXT(1);
		}
		else ext_swapControl=0;

		if(CheckExtension("GL_ARB_multitexture")){
			ext_multitexture=1;

			glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC) glsGetProcAddress("glActiveTextureARB");
			glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) glsGetProcAddress("glClientActiveTextureARB");
			glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &temp);
			gl_maxTextureUnits=temp;
		}
		else {
			ext_multitexture=0;
			gl_maxTextureUnits=1;
		}

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &temp);
		gl_maxTextureSize=temp;




		return 1;	// SUCCEED
	};


	//
	//	Unload the driver from memory
	//
	void shut(void){
		Import.console->Insert("--- shuting driver\n");
		Unlock();	    
		// FIXME: order?
		glsMakeCurrent(0,0);
		glsDeleteContext(app_rc); 
		app_rc=0;
		ReleaseDC(app_wnd, app_dc);
		app_dc = 0;
		delete [] DriverInfos;
		DriverInfos=NULL;
		glsUnloadDriver();
	};


	//
	//	Lock the driver to make some opengl calls
	//
	void Lock(void){
		glsMakeCurrent(app_dc, app_rc);

		if(app_rc!=global_rc){
			if(!glsShareLists(app_rc, global_rc)){
				// TODO: what kind of error ??
			}
		}
	};


	//
	//	Unlock the driver
	//
	void Unlock(void){
		#ifdef DEBUG
			glsMakeCurrent(NULL, NULL);
		#endif // DEBUG
	};


	//
	//	Swaps Back and Front Buffers
	//
	void SwapBuffers(void){
		glsSwapBuffers(app_dc);
	}
};