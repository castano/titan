/*===========================================================================
	Title: File.h
	Author: Ignacio Castaño Aguado
	Description: Entrypoint for the application
	Globals:
		SAlloc		- simple memory allocator
		GAlloc		- global memory allocator, points to SAlloc
	
		cvars		- Variable lookup shared by all the modules
		render		- Render Back End
		input		- input devices
		timer		- timer
		err			- error logging
		console		- log dumpfile
		out			- standard output device, points to console

		world		- objects...

	Functions:
		WinMain		- Entrypoint for win32 apps
===========================================================================*/
#include "titan.h"
#include "window.h"		// this file has to be included only by one file since it contains static member declarations





/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

TSimpleAlloc	GMalloc;		// the memory allocator must exist allways

vars *			cvars = NULL;	// global vars
IRender *		render = NULL;	// render
IInput *		input = NULL;	// input
Timer *			timer = NULL;	// timer
Error *			err = NULL;		// error logging
Console *		console = NULL;	// console
IOutputDevice *	out = NULL;		// output device (reference to the console)


World *			world = NULL;


Var				sys_instance("sys_instance", 0, VF_SYSTEM);



/*---------------------------------------------------------------------------
	WinMain:
---------------------------------------------------------------------------*/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	guard(WinMain);
	LIBRARY renderModule;										// module to be loaded
	void  (__cdecl * CreateRender)(IRender **, Framework *);	// functions to map
	void  (__cdecl * DeleteRender)(IRender *);
	Framework export;											// info to export
	char *mapname=NULL;											// name of the map

	MSG msg;													// messages
	int cons=0, font=0, logo=0;									// shader references


	// create standard input and output devices:
	out = console = new Console;
	err = new Error(console);
	cvars = new vars();

	// start writing on the output device
	out->Insert("%s %s - %s - %s - %s\n", _APP_, _VERSION_, _OS_, __DATE__, __TIME__);

	// init the framework:
	TSystem::Init(err, out, cvars);


	// client and window declaration:
	Client client;
	Window window;


	// save hInstance in cvars because other modules need it
	sys_instance=(int)hInstance;
	cvars->RegisterVar(sys_instance);


	// Load libraries:
	console->Insert("--- Loading Libraries\n");
	// try to load library:
	renderModule=TLoadLibrary("rend_ogl");
	if (renderModule==NULL) {
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL);
		err->Box(UNKNOWN_ERROR, "Error, %sError loading rend_ogl.dll\n",(LPCTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
		goto lib_failed;
	}
	// Map the functions:
	TBindSymbol((void **)&CreateRender, renderModule, "CreateRender");
	TBindSymbol((void **)&DeleteRender, renderModule, "DeleteRender");
	if (!CreateRender || !DeleteRender) {
		TUnloadLibrary(renderModule);
		err->Box(UNKNOWN_ERROR, "Coudn't get proc address in rend_ogl.dll");
		goto lib_failed;
	}


	// create window
	if(!window.create()){
		err->CriticalError(UNKNOWN_ERROR, "Error Creating Window");	// TODO: GetLastError ...
		goto window_failed;
	}


	// create timer:
	timer=new Timer();	// that cant fail


	// init framework: (this info is send to the other modules)
	export.cvars=cvars;
	export.console=console;
	export.timer=timer;
	export.error=err;


	// create input: (input is currently statically linked)
	CreateInput( &input, &export);
	if(!input) err->CriticalError(UNKNOWN_ERROR, "Error creating the input device!\n");
	if(!input->Init()){
		err->CriticalError(UNKNOWN_ERROR, "One or more input devices coudn't be initialized. See log for details.\n");
		goto input_failed;
	}


	// initialize the render
	console->Insert("--- Initializing Render\n");
	CreateRender( &render, &export);
	if(!render) {
		err->CriticalError(UNKNOWN_ERROR, "Error creating the render!\n");
		goto render_failed;
	}
	if(!render->Init((WindowHandle)wnd_handle.ivalue)){
		DeleteRender(render);
		TUnloadLibrary(renderModule);
		err->Box(UNKNOWN_ERROR, "The render failed at initialization\n");
		goto render_failed;
	}


	
	// initialize the client (and the camera, and the viewport)
	client.Init();


	// create the world
	console->Insert("--- Creating World\n");
	world=new World();




	// load a map, if one was specified:
	// (objects must be created before shader loading because here we add the shader reference)
	// TODO: objects should be loaded throug the world, the world should be an object factory
	mapname=cvars->StringForKey("mapname");
	if(mapname && stricmp(mapname, "noname") && stricmp(mapname, "")){
		if(!world->AddMap(mapname)){
			render->Shut();
			DeleteRender(render);
			goto render_failed;
		}
	}


	// initialize overlay graphics 
	// TODO: we could also abstract this
	cons=world->shaders.AddShader("console", 0, FACETYPE_MESH);
	font=world->shaders.AddShader("gfx/2d/bigchars", 0, FACETYPE_MESH);
	logo=world->shaders.AddShader("gfx/misc/asidesoft", 0, FACETYPE_MESH);
	console->SetFont(font);
	console->SetBack(cons);

	// initialize world, and all of his objects
	world->Init();

	GMalloc.HeapCheck();

	// bucle de mensajes:
	console->Insert("--- Entering game loop...\n\n");
	while(true){
		// standard message que
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
			if (msg.message == WM_QUIT)	break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {

			// INPUT:
			timer->Refresh();
			input->Refresh();


			// GAME:
			if(!client.Update()) break;		// something goes wrong, or the game has ended
			// FIXME: that should be in client game:
			console->Update();
			char c[1024];
			sprintf(c, "%3.2f fps", 1/timer->frametime);
			world->over.String(c, font, 561, 1, 1, 7);
			world->over.String(c, font, 560, 0, 1, 0);
			//sprintf(c, "find: %d, nodes: %d, faces: %d, pushing: %d", world->find_leaf, world->mark_nodes, world->mark_faces, world->push_faces);
			//world->over.String(c, font, 0, 0, .75);
			
			world->over.Quad(logo, 512, 448, 128, 32, 1, 1);



			// OUTPUT:
			world->Render(&client.cam);




			// TODO: stop game when applicaton isn't active
			/*if(fAppActive){
				HDC AppWindowDC = GetDC(AppWindow);
				AppPaint(AppWindow,AppWindowDC);
				ReleaseDC(AppWindow,AppWindowDC);
			}
			else {
				WaitMessage();
			}*/
		}
	}

	// free the world (textures, objects, shaders, and the world itself)
	console->Insert("--- Freeing world\n");

	world->Shut();
	delete world; world=NULL;

	// delete the render
	console->Insert("--- Shuting render\n");
	render->Shut();
	DeleteRender(render); render=NULL;

render_failed:
	// delete the input device
	input->Shut();
	DeleteInput(input); input=NULL;

input_failed:
	// delete the timer
	delete timer; timer=NULL;

	// destroy the window
	window.destroy();
	
lib_failed:
	// free the libraries
	TUnloadLibrary(renderModule);


window_failed:
	// delete the rest of the vars:
	cvars->Dump(console);
	delete cvars; cvars=NULL;
	GMalloc.HeapCheck();
	GMalloc.DumpAllocs();
	GMalloc.Shut();
	delete err; err=NULL;
	delete console; console=NULL; out=NULL;

	FileEnvironment::Shut();

	return 0;
	unguard;
}



