/*===========================================================================
	Title: alloc.h
	Author: Ignacio Castaño Aguado
	Description: memory allocators
	
		- one of our 

===========================================================================*/
#include "shared.h"
#include <malloc.h>





/*---------------------------------------------------------------------------
	Implementations:
---------------------------------------------------------------------------*/



//
// Simple memory allocator that uses standard ansi memory allocation
//
void TSimpleAlloc::Init(void){
	guard(TSimpleAlloc::Init);
	unguard;
}

void TSimpleAlloc::Shut(void){
	guard(TSimpleAlloc::Shut);
	unguard;
}

void * TSimpleAlloc::Malloc( DWORD count, const char *Tag ){
	guard(TSimpleAlloc::Malloc);
	if(count<=0){
		//if(TSystem::err) TSystem::err->Log(HEAP_ERROR, "TSimpleAlloc: Error, bad count\n");
		return NULL;
	}

	void *ptr = malloc(count);
		
	if(!ptr)
		if(TSystem::err) TSystem::err->CriticalError(ERROR_ALLOCATING_MEMORY, "Error allocating memory\n");
		
	allocations++;
	return ptr;
	unguard;
}

void * TSimpleAlloc::Realloc( void* mem, DWORD count, const char *Tag ){
	guard(TSimpleAlloc::Realloc);
		
	//if(count<0)
	//	if(TSystem::err) TSystem::err->Log(HEAP_ERROR, "TSimpleAlloc: Error, bad count\n");

	void *ptr;

	if( mem && count ) {
		ptr = realloc( mem, count );
	}
	else if( count ) {
		ptr = malloc( count );
	}
	else {
		if( mem )
			free( mem );
		ptr = NULL;
	}
	return ptr;
	unguardf(( TEXT("%08X %i %s"), (INT)mem, count, Tag ));
}

void TSimpleAlloc::Free( void* mem ){
	guard(TSimpleAlloc::Free);
	if(mem) {
		allocations--;
		free(mem);
	}
	unguard;
}

void TSimpleAlloc::DumpAllocs(void){
	guard(TSimpleAlloc::DumpAllocs);
	if(TSystem::out) TSystem::out->Insert("DumpAllocs: No allocation checking!\n%d allocations\n", allocations);
	unguard;
}

void TSimpleAlloc::HeapCheck(void){
	guard(TSimpleAlloc::HeapCheck);
#if _MSC_VER
	INT Result = _heapchk();
	switch(Result){
		case _HEAPBADBEGIN:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: Bad start of heap\n");
			//er->CriticalError(HEAP_ERROR, "Error, bad start of heap\n");
			break;
		case _HEAPBADNODE:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: Bad node in heap\n");
			//er->CriticalError(HEAP_ERROR, "Error, bad node in heap\n");
			break;
		case _HEAPBADPTR:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: Bad pointer in heap\n");
			//er->CriticalError(HEAP_ERROR, "Error, bad pointer in heap\n");
			break;
		case _HEAPEMPTY:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: Heap is empty\n");
			break;
		case _HEAPOK:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: Heap is fine\n");
			break;
		default:
			if(TSystem::out) TSystem::out->Insert("HeapCheck: unknown heap state!?\n");
		break;
	}
#endif
	unguard;
}



