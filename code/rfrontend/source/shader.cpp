/*===========================================================================
	Title: shader.cpp
	Author: Ignacio Castaño Aguado
	Description: shader database, and shader loading
	Classes:
		ShaderMng				- shader manager
	TODO:
		- do something when we get out of shader hashes!!

===========================================================================*/
#include "rfrontend.h"





/*---------------------------------------------------------------------------
	Globals
---------------------------------------------------------------------------*/

extern IOutputDevice *	out;
extern IRender *		render;
extern Error *			err;
extern IAlloc *			GMalloc;
extern World *			world;




/*---------------------------------------------------------------------------
	ShaderMng
---------------------------------------------------------------------------*/


//
// Constructor: allocate space for the hashes
//
ShaderMng::ShaderMng(const int num){
	guard(ShaderMng::ShaderMng);
	num_refs=0;
	max_refs=num;
	ShaderTable = (ShaderHash *) malloc(sizeof(ShaderHash)*num);
	appMemzero(ShaderTable, sizeof(ShaderHash)*num);
	unguard;
}

ShaderMng::~ShaderMng(void){
	guard(ShaderMng::~ShaderMng);
	free(ShaderTable);
	unguard;
}


//
// Adds a reference to a shader
//
int ShaderMng::AddShader(const char *name, int flags, int type){
	guard(ShaderMng::AddShader);
	int i;

	// search for name
	for(i=0; i<max_refs; i++){
		
		if(!ShaderTable[i].name) continue;		// free slot

		if(!strcmp(ShaderTable[i].name, name)){
			// allready referenced, increase reference counter
			ShaderTable[i].ref_count++;
			return i;
		}
	}

	if(num_refs>=max_refs){
		// TODO: do something better here !!!
		err->CriticalError(UNKNOWN_ERROR, "Out of shader refs slots!");
		return -1;
	}

	// find a free reference and allocate it
	for(i=0; i<max_refs; i++){
		if(ShaderTable[i].ref_count==0 && ShaderTable[i].shader==NULL){
			ShaderTable[i].ref_count	= 1;
			ShaderTable[i].shader= NULL;
			ShaderTable[i].name	= (char *) malloc(strlen(name)+1); // TODO: use namehunk->alloc
			strcpy(ShaderTable[i].name, name);
			ShaderTable[i].flags	= flags;
			ShaderTable[i].type	= type;
			
			num_refs++;
			return i;
		}
	}
	return -1;
	unguard;
}



//
// Deletes a reference, and his shader
//
// FIXME: not tested!!!
// FIXME: why not decrease the ref_count and update later???
void ShaderMng::DeleteShader(const int num){
	guard(ShaderMng::DeleteShader);
	// decrease reference counter
	if(ShaderTable[num].ref_count) {
		ShaderTable[num].ref_count--;

		// delete if we reached 0
		if(!ShaderTable[num].ref_count) {
			delete ShaderTable[num].shader; ShaderTable[num].shader=NULL;
			free(ShaderTable[num].name); ShaderTable[num].name=NULL;
			num_refs--;
		}
	}
	unguard;
}


//
// reset all reference counters
//
void ShaderMng::ResetAll(void){
	guard(ShaderMng::ResetAll);
	for(int i=0; i<max_refs; i++){
		ShaderTable[i].ref_count=0;
	}
	unguard;
}


//
// loads unloaded shaders, deletes unreferenced shaders
//
void ShaderMng::Update(void){
	guard(ShaderMng::Update);
	char *name, *shadername;
	int i;

	
	// start search
	out->Insert("--- Reading Shaders:\n");
	SearchFile search("scripts/*.shader");
	
	// for each .shader file
	for(name=search.FirstFile(); name; name=search.NextFile()){
		
		// opens the file:
		out->Insert("---    %s\n",name);
		ShaderFile file((char *)name);

		// parse each shader
		while(shadername=file.GetShaderName()){
			
			for(i=0; i<max_refs; i++){

				// load it if
				if( !ShaderTable[i].shader &&					// not a free slot
					ShaderTable[i].ref_count &&					// has a reference
					!strcmp(ShaderTable[i].name, shadername)){	// the names are equal
					
					//out->Insert("---      %s\n", shadername);

					// create blank shader
					Shader *s = ShaderTable[i].shader = CreateShader();

					// set the appropiate flags
					s->surface_flags = ShaderTable[i].flags;

					// parse it, and fill shader structure
					file.Parse(s);

					// precalculate multitexture collapsing
					if(s->num_layers>1)
					for(int i=0; i<s->num_layers-1; i++){
						if( s->layer[i]->blendsrc==IntForConst("GL_ONE") && 
							s->layer[i]->blenddst==IntForConst("GL_ZERO")){
							if(
								(s->layer[i+1]->blendsrc==IntForConst("GL_DST_COLOR")  && 
								 s->layer[i+1]->blenddst==IntForConst("GL_ZERO")     ) ||
								(s->layer[i+1]->blendsrc==IntForConst("GL_ZERO")       && 
								 s->layer[i+1]->blenddst==IntForConst("GL_SRC_COLOR"))
							){
								s->layer[i]->flags|=LF_MULTITEX;
								s->flags|=SF_MULTITEX;
								//out->Insert("!!! two layers collapsed!\n");
							}
						}
					}
					break;
				}
			}

			// if not found, skip shader
			if(i==max_refs) file.Skip();
		}
	}

	for(i=0; i<max_refs; i++){
		if(ShaderTable[i].ref_count){
			// load referenced shaders that havent been found
			if(ShaderTable[i].shader==NULL){
				ShaderTable[i].shader = CreateShader();
				ShaderTable[i].shader->surface_flags = ShaderTable[i].flags;
				DefaultShader(i);
			}
		}
		else {
			// delete non-referenced shaders
			DeleteReference(i);
		}
	}
	out->Insert("ShaderMng: %d shaders in memory\n", num_refs);

	// reallocate surface buffers
	for(i=0; i<max_refs; i++){
		ShaderTable[i].num_nodes=0;

		delete [] ShaderTable[i].list;
		ShaderTable[i].list=NULL;

		if(ShaderTable[i].ref_count){
			ShaderTable[i].list = new Node[ShaderTable[i].ref_count];
		}
	}


	unguard;
}


//
// deletes all the shaders
//
void ShaderMng::DeleteAll(void){
	guard(ShaderMng::DeleteAll);
	unguard;
}



//
// loads the default shader and puts it in ShaderTable[i]
//
void ShaderMng::DefaultShader(int i){
	guard(ShaderMng::DefaultShader);
	ShaderHash *r=&ShaderTable[i];
	r->shader->surface_flags=r->flags;

	if(r->type==FACETYPE_MESH){
		r->shader->flags=SF_HASCOLORS;
		r->shader->num_layers=1;
		r->shader->layer[0] = world->layers.CreateLayer();
		r->shader->layer[0]->depthWrite=1;
		r->shader->layer[0]->blendsrc=IntForConst("GL_ONE");
		r->shader->layer[0]->blenddst=IntForConst("GL_ZERO");
		r->shader->layer[0]->rgbGen = RGBGEN_VERTEX;
		r->shader->layer[0]->flags=0;

		// load texture into memory
		r->shader->layer[0]->map[0]=world->texmng.AddTexinfo(r->name, r->shader->layer[0]->flags);
	}
	else {
		r->shader->flags=SF_HASLIGHTMAP|SF_MULTITEX;
		
		r->shader->num_layers=2;
		r->shader->layer[0]=world->layers.CreateLayer();
		r->shader->layer[0]->depthWrite=1;
		r->shader->layer[0]->blendsrc=IntForConst("GL_ONE");
		r->shader->layer[0]->blenddst=IntForConst("GL_ZERO");
		r->shader->layer[0]->flags=LF_LIGHTMAP|LF_NOMIPMAP|LF_NOPICMIP|LF_CLAMP|LF_MULTITEX;
		r->shader->layer[0]->tcGen=TCGEN_LIGHTMAP;
		r->shader->layer[0]->rgbGen = RGBGEN_IDENTITY;
		
		r->shader->layer[1]=world->layers.CreateLayer();
		r->shader->layer[1]->depthWrite=1;
		r->shader->layer[1]->blendsrc=IntForConst("GL_DST_COLOR");
		r->shader->layer[1]->blenddst=IntForConst("GL_ZERO");
		r->shader->layer[1]->flags=0;

		// load texture into memory
		r->shader->layer[1]->map[0]=world->texmng.AddTexinfo(r->name, r->shader->layer[1]->flags);
	}
	unguard;
}


//
// CreateShader allocates a shader and fills it with default values
//
// TODO: we could improve that using a linear allocator to store the shaders
Shader *ShaderMng::CreateShader(void){
	guard(ShaderMng::CreateShader);
	// allocate shader.
	Shader *s= new Shader;

	// fill shader with default values of an empty shader.
	s->surface_flags=0;
	s->contents=0;
	s->cull=CULL_BACK;
	s->sortvalue=SORT_NORMAL;
	s->flags=0;
	s->num_layers=0;
	for(int i=0; i<MAX_LAYERS; i++) 
		s->layer[i]=NULL;

	s->deformVertexes=DEFORMVERTEXES_NONE;

	return s;
	unguard;
}



void ShaderMng::DeleteReference(int ref){
	guard(ShaderMng::DeleteReference);

	if(ShaderTable[ref].shader){
		for(int i=0; i<MAX_LAYERS; i++){
		
			if(!ShaderTable[ref].shader->layer[i]) break;
			
			world->layers.DeleteLayer(ShaderTable[ref].shader->layer[i]);
			ShaderTable[ref].shader->layer[i]=NULL;

		}
		delete ShaderTable[ref].shader;
	}
	if(ShaderTable[ref].name){				// por similitud
		delete ShaderTable[ref].name;
		num_refs--;
	}

	unguard;
}

