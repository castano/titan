/*===========================================================================
	Title: keybuffer.cpp
	Author: Ignacio Castaño Aguado
	Description: Buffer of the last key pressed

===========================================================================*/
#include "input.h"


KeyBuffer::KeyBuffer(void){
	enabled=false;
	Clear();
}


void KeyBuffer::Clear(void){
	int i;

	for(i=0; i<BUFFER_SIZE; i++) 
		Buffer[i]=0;

	for(i=0; i<MAX_KEYBKEYS; i++) {
		Keys[i].TimeStamp=0;
		Keys[i].NextTime=0;
	}

	top=0;
	current=0;
};


void KeyBuffer::ProcessKey(BYTE *buffer, int key){
	static HKL layout=GetKeyboardLayout(0);
	static byte State[256];

	if(enabled){
		if(top < BUFFER_SIZE) {
			char c[4];
			int inc;
			if (GetKeyboardState(State)==FALSE) return;
			if (key==DIK_RIGHT){
				current++;
				if(current>top) current=top;
			}
			else if(key==DIK_LEFT){
				current--;
				if(current<0) current=0;
			}
			else if(key==DIK_DELETE){
				if(current<top){
					for(int i=current;i<top;i++) Buffer[i]=Buffer[i+1];
					top--;
				}
			}
			else if(key==DIK_BACKSPACE){
				if(current>0) {
					current--;
					for(int i=current;i<top;i++) Buffer[i]=Buffer[i+1];
					top--;
				}
			}
			else if(key==DIK_HOME) current=0;
			else if(key==DIK_END) current=top;
			else {
				UINT vk = MapVirtualKeyEx(key, 1, layout);
				inc = ToAsciiEx(vk, key, State, (unsigned short *)c, 0, layout);
				if(c[0]>16 && c[0]<127) {
					Insert(c[0]);
				}
			}
		}
	}
};


char *KeyBuffer::GetBuffer(void){
	return Buffer;
};

void KeyBuffer::Enabled(bool b){
	enabled=b;
};


void KeyBuffer::Insert(char c){

	if(top<BUFFER_SIZE){
		for(int i=top;i>=current;i--){
			Buffer[i]=Buffer[i-1];
		}
		top++;
	}
	Buffer[current]=c;
	current++;
};