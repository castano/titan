/*===========================================================================
	Title: Tbitarray.h
	Author: Ignacio Castaño Aguado
	Description: bit array, usefull for storing flags, but dont very fast...
	TODO: 
		- check bounds if debuging ...
===========================================================================*/




/*-----------------------------------------------------------------------------
	Globals:
-----------------------------------------------------------------------------*/

//
// Bit Array
//
class BitArray {
private:
	INT *array;
	int size;
public:
	BitArray(int size){
		this->size=size/32+1;
		array=new INT[this->size];
		clear();
	}
	~BitArray(void){
		delete [] array;
	}

	inline void set(int i){
		array[i>>5] |= (1<<(i&31));
	}
	inline void unset(int i){
		array[i>>5] &= ~(1<<(i&31));
	}
	inline void toggle(int i){
		array[i>>5] ^= (1<<(i&31));
	}
	
	inline void clear(void){
		appMemzero(array, size*sizeof(INT));
	}
	inline void setall(void){
		memset(array, ~0, size*sizeof(INT));
	}

	// this funcion returns zero if bit isnt set, nonzero, if it is set.
	inline int __fastcall get(int i){
		return array[i>>5]&(1<<(i&31));
	}
};
