/*===========================================================================
	Title: var.h
	Author: Ignacio Castaño Aguado
	Description: global variable class, mainly used to store vars through
		different games, and to share them between different modules
	Classes:
		var	
		vars implements ivars
	TODO: 
		- use flags for something: system vars, readonly vars, persisten vars...
		- use a hash table and make Var a child of Entry<info> of be a little
		  decent and use a template linked list

===========================================================================*/





/*-----------------------------------------------------------------------------
	Variable list:
-----------------------------------------------------------------------------*/
// TODO: use a hash table!

class vars : public ivars {
private:
	Var *list;

public:
	vars (void){
		list=NULL;
	};
	
	~vars (void){	
		
		Var *v, *vnext;
		for(v=list; v; v=vnext){
			vnext=v->next;
			if(v->flags&VF_TEMP) 
				delete v;	// NOTE: the coherency of the list is broken, but that doesn't mind in the destructor
		}
		
	};

	void Dump(IOutputDevice *out){
		int num=0;
		for(Var *v=list; v; v=v->next){
			num++;
		}
		out->Insert("--- %d varibles:\n", num);

	}

	//
	// Create a temporal variable.
	//
	void CreateVar(const char *name, const char *string, int flags=0){
		if(!SetKeyValue(name, string, flags)){
			flags|=VF_TEMP;
			Var *v=new Var(name, string, flags);
			RegisterVar(*v);
		}
	}
	void CreateVar(const char *name, float fvalue, int flags=0){
		if(!SetKeyValue(name, fvalue, flags)){
			flags|=VF_TEMP;
			Var *v=new Var(name, fvalue, flags);
			RegisterVar(*v);
		}
	}
	void CreateVar(const char *name, int ivalue, int flags=0){
		if(!SetKeyValue(name, ivalue, flags)){
			flags|=VF_TEMP;
			Var *v=new Var(name, ivalue, flags);
			RegisterVar(*v);
		}
	}


	//
	// Set key value
	//
	bool SetKeyValue(const char *name, const char *string, int flags=0){
		// search key, if found setstring
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				v->SetString(string);
				return true;
			}
		}
		return false;
	};
	bool SetKeyValue(const char *name, const float value, int flags=0){
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				v->SetFloat(value);
				return true;
			}
		}
		return false;
	};
	bool SetKeyValue(const char *name, const int value, int flags=0){
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				v->SetInteger(value);
				return true;
			}
		}
		return false;
	};


	//
	// Register a variable if it doesnt exists. If it exists and is temporal replace it.
	//
	void RegisterVar(Var &var){
		Var *v, *lastv=NULL;
		
		for(v=list; v; v=v->next){				// search for name
			
			if(!strcmp(var.name, v->name)){
				if(v->flags&VF_TEMP){

					var.next=list;
					list=&var;
					
					if(!(var.flags&VF_SYSTEM)){		
						var.svalue=new char[strlen(v->svalue)+1];
						strcpy(var.svalue, v->svalue);
						var.fvalue=v->fvalue;
						var.ivalue=v->ivalue;
						var.flags=v->flags^VF_TEMP;
					}
					
					lastv->next=v->next;		// delete temporal var from the list
					delete v;
					return;
				}
				break;
			}
			lastv=v;
		}
		
		if(!v){									// if not found insert at the beginning
			var.next=list;
			list=&var;
		}

	}

	void UnregisterVar(Var &var){
		if(list==&var) {
			//delete [] list->name; list->name=NULL;
			//delete [] list->svalue; list->svalue=NULL;
			list=var.next;
			return;
		}
		
		for(Var *v=list; v; v=v->next){
			if(v->next==&var){
				v->next=var.next;
				return;
			}
		}
	}




	// query methods:
	char * StringForKey(const char * name) {
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				return v->svalue;
			}
		}
		return "";
	};

	float ValueForKey(const char * name) {
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				return v->fvalue;
			}
		}
		return 0.0f;
	};

	int IntForKey(const char * name) {
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				return v->ivalue;
			}
		}
		return 0;
	};

	bool isKey(const char *name){
		for(Var *v=list; v; v=v->next){
			if(!strcmp(name, v->name)){
				return true;
			}
		}
		return false;
	}
};
