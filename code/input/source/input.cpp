/*===========================================================================
	Title: input.cpp
	Author: Ignacio Castaño Aguado
	Description: main source file of the input module

===========================================================================*/
#include "input.h"




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

Framework Import;





/*---------------------------------------------------------------------------
	Input:
---------------------------------------------------------------------------*/


class Input : public IInput {
private:
	DXInput DXDevice;
	stdInput DefDevice;

	int		DXFlag;
	int		stdFlag;
	
	bool	bActive;

public:
	
	//
	// Initialization
	//
	int Init(void){
		Import.console->Insert("--- Creating input device\n");

		if(Import.cvars->ValueForKey("i_useDirectInput")==1) DXFlag=ID_KEYBOARD|ID_MOUSE;
		else DXFlag=0;

		stdFlag=ID_KEYBOARD|ID_MOUSE;
		
		DXFlag=DXDevice.Init(DXFlag);

		stdFlag^=DXFlag;

		stdFlag=DefDevice.Init(stdFlag);

		int flags = ~(stdFlag|DXFlag);

		if(flags&ID_KEYBOARD){
			Import.console->Insert("*** No keyboard device was found\n");
			return 0;
		}
		if(flags&ID_MOUSE){
			Import.console->Insert("*** No mouse device was found\n");
			return 0;
		}

		bActive=true;

		return 1;
	};
	

	//
	// Shutdown
	//
	void Shut(void){
		Import.console->Insert("--- Shuting input device\n");
		DXDevice.Shut();
		DefDevice.Shut();
	};
	
	
	//
	// Restart
	//
	int Restart(void){
		return 0;
	};
	
	
	//
	// Refresh
	//
	void Refresh(void){
		if(bActive){
			DXDevice.Refresh();
			DefDevice.Refresh();
		}
	};
	

	//
	// Activate
	//
	void Activate(bool on_off){
		DXDevice.Activate(on_off);
		bActive=on_off;
	};


	//
	// Get the value of the 
	//
	int GetValue(int device, int index){
		if(!bActive) return 0;

		if(device&DXFlag){
			return DXDevice.GetValue(device, index);
		}
		else if(device&stdFlag){
			return DefDevice.GetValue(device, index);
		}
		else return 0;
	};

	
	//
	// Retrieve char buffer
	//
	char *GetBuffer(void){
		if(DXFlag&ID_KEYBOARD){
			return DXDevice.KeyBuff->GetBuffer();
		}
		if(stdFlag&ID_KEYBOARD){
			return DefDevice.KeyBuff->GetBuffer();
		}
		return NULL;
	};
	

	//
	// Set the cursor on the buffer
	//
	int GetCursorPos(void){
		if(DXFlag&ID_KEYBOARD) return DXDevice.KeyBuff->GetCursorPos();
		else if(stdFlag&ID_KEYBOARD) return DefDevice.KeyBuff->GetCursorPos();
		else return 0;		// shut up vc
	};

	void EnableBuffer(bool b){
		if(DXFlag&ID_KEYBOARD) DXDevice.KeyBuff->Enabled(b);
		else if(stdFlag&ID_KEYBOARD) DefDevice.KeyBuff->Enabled(b);
	};
	
	void ClearBuffer(void){
		if(DXFlag&ID_KEYBOARD) DXDevice.KeyBuff->Clear();
		else if(stdFlag&ID_KEYBOARD) DefDevice.KeyBuff->Clear();
	};

};





/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

void CreateInput(IInput **inp, Framework *f){
	*inp=(IInput *) new Input;
	memcpy(&Import, f, sizeof(Framework));
}

void DeleteInput(IInput *inp){
	delete inp;
}
