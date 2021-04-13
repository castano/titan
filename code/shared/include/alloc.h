/*===========================================================================
	Title: alloc.h
	Author: Ignacio Castaño Aguado
	Description: memory allocators
		- one of our 

===========================================================================*/
#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__





/*---------------------------------------------------------------------------
	Interfaces:
---------------------------------------------------------------------------*/

//
// Memory allocator.
//
class IAlloc {
public:
	virtual void Init(void)=0;
	virtual void Shut(void)=0;
	virtual void * Malloc( DWORD count, const char* Tag )=0;
	virtual void * Realloc( void *mem, DWORD count, const char* Tag )=0;
	virtual void Free( void *mem )=0;
	virtual void DumpAllocs(void)=0;
	virtual void HeapCheck(void)=0;
};



/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

inline void *Align( void *ptr, INT alignment ) {
	return (void *)(((DWORD)ptr + alignment - 1) & ~(alignment-1));
}




class TSimpleAlloc : public IAlloc {
private:
	int allocations;

public:
	void Init(void);
	void Shut(void);
	void * Malloc( DWORD count, const char *Tag );
	void * Realloc( void* mem, DWORD count, const char *Tag );
	void Free( void* mem );
	void DumpAllocs(void);
	void HeapCheck(void);
};



#endif