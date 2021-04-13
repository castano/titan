/*===========================================================================
	Title: TGrowArray.h
	Author: Ignacio Castaño Aguado
	Description: Growable Array, usefull for storing semistatic small elements
		that need fast direct acces. 
		¡¡¡ It works only with pointers to objects !!!
	Classes:
		GrowArray<T>

===========================================================================*/



/*----------------------------------------------------------------------------
	GrowArray
----------------------------------------------------------------------------*/

//
//	Stores arbitrary entries into an indexed table of lists
//
template <class T> 
class GrowArray {
public:

	//	Creates the table of given size
	GrowArray(int size=1024, int growsize=256);

	//	Destroy it
	~GrowArray(void);

	//	Inserts an element.
	void add(const T);

	//	Clears the array
	void clear(void);

	//	Direct access operators
	T &operator[](int idx);
	T operator[](int idx) const;

	// gets..
	int getSize(void)const{return top;};

private:

	//	Table of linked lists
	T *Array;

	//	Size of the table
	int size;

	//	Size of the allocated blocks
	int blocksize;

	//	top
	int top;
};


//
//	Constructor
//
template <class T>
GrowArray<T>::GrowArray<T>(int size, int growsize){
	blocksize = NextPowerOfTwo(growsize);
	Array = (T *)malloc(sizeof(Entry<T>)*size);
	top=0;
};


//
//	Destructor
//
template <class T>
GrowArray<T>::~GrowArray<T>(void){
	free(Array);
};


//
//	Adds an element to the growable array
//
template <class T>
void GrowArray<T>::add(const T elem){
	if(top>size){
		size+=blocksize;
		Array=(T *)realloc(Array, sizeof(Entry<T>)*size);
	}
	Array[top]=elem;
	top++;
};


//
//	Clears the growable array but doesnt resize it.
//
template <class T>
void GrowArray<T>::clear(void){
	for(int i=0; i<top; i++){
		delete Array[i];
	}
	top=0;	
};


template <class T>
T &GrowArray<T>::operator[](int idx){
	return Array[idx];
};

template <class T>
T GrowArray<T>::operator[](int idx) const{
	return Array[idx];
};

