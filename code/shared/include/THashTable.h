/*===========================================================================
	Title: THashTable.h
	Author: Ignacio Castaño Aguado
	Description: Hash Table
	Structs:
		Entry
	Classes:
		HashTable<T>
	TODO: 
		use a stack in each hash, for faster and easier pushes and pops

===========================================================================*/


/*----------------------------------------------------------------------------
	Definitions.
----------------------------------------------------------------------------*/

// Maximum size of name.
enum {NAME_SIZE	= 64};



/*----------------------------------------------------------------------------
	Entry.
----------------------------------------------------------------------------*/

//
//	An entry in the hash table. 
//
template <class T> struct Entry {

	Entry*	next;				// Pointer to the next entry in this hash bin's linked list.

	char	name[NAME_SIZE];	// The name string.
	T		info;

};


/*----------------------------------------------------------------------------
	HashTable.
----------------------------------------------------------------------------*/

//
//	Stores arbitrary entries into an indexed table of lists
//
template <class T> 
class HashTable {
public:

	//
	//	Creates the table of given size
	//
	HashTable(int size=4096);


	//
	//  Frees the hash
	//
	~HashTable(void);

	//
	//	Inserts an element.
	//
	void put(const char *, const T &);

	//
	//	Gets an element.
	//
	T* get(const char *);

	//
	//	Delete an element
	//
	void erase(const char *);

	//
	//	Clears the table.
	//
	void clear(void);

private:

	//	Table of linked lists
	Entry<T> ** Hash;

	//	Size of the table
	int size;
};




/*----------------------------------------------------------------------------
	HashTable Implementation.
----------------------------------------------------------------------------*/

//
//	Creates the table of given size
//
template <class T>
HashTable<T>::HashTable<T>(int size){
	this->size=size;
	Hash = (Entry<T> **)malloc(sizeof(Entry<T> *)*size);
	appMemzero(Hash, size*sizeof(Entry<T> *));

};


//
// Frees the hash
//
template <class T>
HashTable<T>::~HashTable<T>(void){
	clear();
	free(Hash);
};



//
//	Inserts an element.
//
template <class T>
void HashTable<T>::put(const char *name, const T &info){
	DWORD index = appStrihash( name ) % size;

	for(Entry<T> *e=Hash[index]; e; e=e->next){
		if(!strcmp(e->name, name)) break;
	}
	if(!e) {
		e=new Entry<T>;
		e->next=Hash[index];
		Hash[index]=e;
	}

	strcpy(e->name, name);
	e->info=info;

};



//
//	Gets an element.
//
template <class T>
T* HashTable<T>::get(const char *name){
	DWORD index = appStrihash( name ) % size;

	for(Entry<T> *e=Hash[index]; e; e=e->next){
		if(e)
		if(!strcmp(e->name, name)) {
			return &e->info;
		}
	}

	return NULL;
};

//
// Free the hash
//
template <class T>
void HashTable<T>::clear(void){
	Entry<T> *enext, *e;

	// go through each hash freeing the linked list
	for(int i=0; i<size; i++){
		for(e=Hash[i]; e; e=enext){
			enext=e->next;
			//delete e->info;	// FIXME: ¡¡¡HACK!!! that is done, because in this hashtable all 
							// infos are pointers to previously allocated classes
			delete e;
		}
	}
}
