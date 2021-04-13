/*===========================================================================
	Title: console.cpp
	Author: Ignacio Castaño Aguado
	Description: console

===========================================================================*/
#include "titan.h"




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern vars	*cvars;
extern Error *err;
extern IRender *render;
extern Timer *timer;
extern IInput *input;
extern CmdList Commands;

extern World *world;




/*---------------------------------------------------------------------------
	Members:
---------------------------------------------------------------------------*/


//
// Constructor
//
Console::Console(void):DumpFile(LOGFILE){
	first=0;
	last=0;
	movingConsole=false;
	isOpen=false;

	Height=0;			// current height
	MaxHeight=240;		// expected height when opened

	MoveSpeed=1000;		// 1000 virtual pixels per second
}


//
// Insert a message on the console
//
void Console::Insert(const char *str, ...){
	int i;
	va_list arg;

	va_start(arg,str);
	vsprintf(InsertCad,str,arg);

	va_end(arg);

	int len=(int)strlen(InsertCad);
	int lastspace=0;
	int formated=0;
	int lines=1;
	if(len>MSGLENGHT){
		for(i=0;i<len;i++){
			if(InsertCad[i]==' ')lastspace=i;
			if(i-formated>MSGLENGHT){
				// si la palabra es demasiado larga, la rompe: aquí se debería insertar un espacio
				// para despues sustituirlo por un salto. sin embargo, se sustituye un caracter
				// para no liarnos demasiao.
				if(lastspace==formated)	lastspace=i;
				formated=lastspace;
				lines++;
				InsertCad[lastspace]='\n';
			}
		}
	}

	// insert messages in the console breaking multiline strings
	len=i=0;
	while(InsertCad[i]){
		if(InsertCad[i]=='\n'||InsertCad[i]=='\r'){
			msg[first][i-len]=0;
			i++;
			len=i;
			first++;
			if(first>MAXMSGS-1) first=0;
			if(first==last) last++;
			if(last>MAXMSGS-1) last=0;
		}
		else {
			msg[first][i-len]=InsertCad[i];
			i++;
		}
	}

	fputs(InsertCad,fp);
	fflush(fp);
}


//
// Opens or closes the console
//
void Console::Toggle(void){
	if(!movingConsole) {
		isOpen=!isOpen;
		movingConsole=true;
		input->EnableBuffer(isOpen);	// enable buffer writting
		input->ClearBuffer();			// clears buffer
	}
}


//
// Set the console font
//
void Console::SetFont(int f){
	font=f;
}


//
// Set the console font
//
void Console::SetBack(int b){
	back=b;
}


//
// Draws the console on the screen, and execute commands
//
void Console::Update(void){

	if(input->GetValue(ID_KEYBOARD, KEY_GRAVE)) Toggle();

	if(isOpen){
		// console opening, calculate visible portion
		if(movingConsole){
			if(Height<MaxHeight){
				movingConsole=true;
				Height+=MoveSpeed*timer->frametime;
				if(Height>MaxHeight)Height=MaxHeight;
			}
			else {
				movingConsole=false;
			}
			VisibleMessages=(int)ceil(Height/16.0f)-1;
		}
		else {
			VisibleMessages=14;
		}
		
		// command execution:
		if( input->GetValue(ID_KEYBOARD, KEY_KP_ENTER ) ||
			input->GetValue(ID_KEYBOARD, KEY_ENTER )){
			
			// start parsing before execing any command
			Parser::StartParseString(input->GetBuffer());
			if(Parser::GetToken(false)){
				if(!Commands.Exec(Parser::token))
					err->Log(UNKNOWN_ERROR, "Command %s not found!\n", Parser::token);
			}
			input->ClearBuffer();
		}

	}
	else {
		// console closing, calculate visible portion
		if(movingConsole){
			if(Height>0){
				movingConsole=true;
				Height-=MoveSpeed*timer->frametime;
				if(Height<0)Height=0;
			}
			else {
				movingConsole=false;
			}
			VisibleMessages=(int)ceil(Height/16.0f)-1;
		}
		else {
			VisibleMessages=0;
		}
	}



	if(Height!=0){
		world->over.Quad(back, 0, 0, 640, Height, 1, .75);
		world->over.String("]", font, 8, Height-32, 1);
		world->over.String(input->GetBuffer(), font, 16, Height-32, 1);
		world->over.String("_", font, input->GetCursorPos()*8+16, Height-32);
		world->over.String("Q3CLONE", font, 561, Height-48+1, 1, 1);
		world->over.String("Q3CLONE", font, 560, Height-48, 1, 5);
	}

	int num_msg=first-1;
	for(int i=0; i<VisibleMessages; i++){
		world->over.String(msg[num_msg], font, 8, Height-i*16-48, 1);
		num_msg--;
		if(num_msg<0) num_msg=MAXMSGS-1;
		if(num_msg==last-1) break;
	}

}



