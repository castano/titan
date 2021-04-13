/*===========================================================================
	Title: ShaderFile.h
	Author: Ignacio Castaño Aguado
	Description: .shader file parser
	TODO: 
		- we spend a lot of time in stricmp
		- use a hash table to parse the commands and minimize strcmps
===========================================================================*/

#include "rfrontend.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern World *		world;




/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

char *ShaderFile::GetShaderName(void){
	if(mem){
		if(Parser::GetToken(true)){
			return Parser::token;
		}
	}
	return NULL;
};

void ShaderFile::Skip(void){
	int depth=0;
	
	while(Parser::GetToken(true)){
		if(!strcmp(Parser::token, "{"))depth++;
		else if(!strcmp(Parser::token, "}"))depth--;
		
		if(depth==0)break;
	}
}

void ShaderFile::Parse(Shader *s){

	s->num_layers=0;



	if(Parser::GetToken(true)){
		if(strcmp(Parser::token, "{")) {
			TSystem::err->Log(UNKNOWN_ERROR, "Warning, '{' expected");
			return;
		}
	}

	while(Parser::GetToken(true)){

		if(!strcmp(Parser::token, "{")) {
			
			s->layer[s->num_layers]=world->layers.CreateLayer();
			ParseLayer(s, s->layer[s->num_layers]);
			s->num_layers++;

			continue;
		}

		else if(!strcmp(Parser::token, "}")) break;	// ends shader
		
		else if(!stricmp(Parser::token, "cull")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}

			if(!stricmp(Parser::token, "front")) s->cull=CULL_FRONT;
			else if(!stricmp(Parser::token, "back")) s->cull=CULL_BACK;
			else if(!stricmp(Parser::token, "none")||!stricmp(Parser::token, "disable")||!stricmp(Parser::token, "twosided")) s->cull=CULL_NONE;
			else {
				// unknown parameter !!
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown parameter %s in line %d\n", Parser::token, Parser::scriptline);
			}
		}

		else if(!stricmp(Parser::token, "nopicmip")){
			s->flags|=SF_NOPICMIP;
		}
		
		else if(!stricmp(Parser::token, "nomipmaps")){
			s->flags|=SF_NOMIPMAP;
		}
		
		else if(!stricmp(Parser::token, "polygonoffset")){
			s->flags|=SF_POLYGONOFFSET;
		}
		
		else if(!stricmp(Parser::token, "portal")){
			s->flags|=SF_PORTAL;
			s->sortvalue=SORT_PORTAL;
		}
		
		else if(!stricmp(Parser::token, "sort")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			
			if(!stricmp(Parser::token, "portal")) {
				s->flags|=SF_PORTAL;
				s->sortvalue=SORT_PORTAL;
			}
			else if(!stricmp(Parser::token, "sky")) s->sortvalue=SORT_SKY;
			else if(!stricmp(Parser::token, "opaque")) s->sortvalue=SORT_OPAQUE;
			else if(!stricmp(Parser::token, "banner")) s->sortvalue=SORT_BANNER;
			else if(!stricmp(Parser::token, "underwater")) s->sortvalue=SORT_UNDERWATER;
			else if(!stricmp(Parser::token, "additive")) s->sortvalue=SORT_ADDITIVE;
			else if(!stricmp(Parser::token, "nearest")) s->sortvalue=SORT_NEAREST;
			else {
				s->sortvalue=atoi(Parser::token);
				if(s->sortvalue==0){			// FIXME: does atoi return 0 when no isdigit?
					s->sortvalue=SORT_OPAQUE;
					// unknown parameter !!
					TSystem::err->Log(UNKNOWN_ERROR, "Warning, invalid parameter %s in line %d\n", Parser::token, Parser::scriptline);
				}
			}
		}
		else if(!stricmp(Parser::token, "deformvertexes")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			
			if(!stricmp(Parser::token, "wave")) {
				s->deformVertexes = DEFORMVERTEXES_WAVE;
				s->deformVertexes_params[0]=parseFunc();
				for(int i=1; i<7; i++){
					if(!Parser::GetToken(false)) break;
					s->deformVertexes_params[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "normal")) {
			}
			else if(!stricmp(Parser::token, "bulge")) {
			}
			else if(!stricmp(Parser::token, "move")) {
			}
			else if(!stricmp(Parser::token, "autosprite")) {
			}
			else if(!stricmp(Parser::token, "autosprite2")) {
			}
			else {
				// unknown parameter !!
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown parameter %s in line %d\n", Parser::token, Parser::scriptline);
			}
		}

		

		else if(!stricmp(Parser::token, "surfaceparm")){			// ignore
			while(Parser::GetToken(false)){};
			continue;
		}
		else if(!stricmp(Parser::token, "fogParams")){			// ignore?
			while(Parser::GetToken(false)){};
			continue;
		}



		// skip non-used commands
		else if(!strnicmp(Parser::token, "qer_", 4)){			// ignore
			while(Parser::GetToken(false)){};
			continue;
		}
		else if(!strnicmp(Parser::token, "q3map_", 6)){			// ignore
			while(Parser::GetToken(false)){};
			continue;
		}
		else if(!stricmp(Parser::token, "light")){				// ignore
			while(Parser::GetToken(false)){};
			continue;
		}
		else if(!stricmp(Parser::token, "tessSize")){			// ignore
			while(Parser::GetToken(false)){};
			continue;
		}
		
		
		// unknown command
		else{
			TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown command %s in line %d\n", Parser::token, Parser::scriptline);
		}

		// unknown keyword skip line
		while(Parser::GetToken(false)){TSystem::err->Log(UNKNOWN_ERROR, "Warning, skipping %s in line %d\n", Parser::token, Parser::scriptline);};

	}

};


// TODO: generate warning if any stage after the first does not have blendfunc
// FIXME: we should test if the layer has at least one map.
void ShaderFile::ParseLayer(Shader *s, Layer *l){
		
	if(s->flags&SF_NOMIPMAP) l->flags|=LF_NOMIPMAP;
	if(s->flags&SF_NOPICMIP) l->flags|=LF_NOPICMIP;

	while(Parser::GetToken(true)){

		if(!strcmp(Parser::token, "{")){
			// display error, token unexpected!
			break;
		}
		
		else if(!strcmp(Parser::token, "}")) return;	// ends layer!
		
		else if(!stricmp(Parser::token, "map")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "$lightmap")){
				s->flags|=SF_HASLIGHTMAP;
				l->flags=LF_LIGHTMAP;
				l->tcGen=TCGEN_LIGHTMAP;
				l->map[0]=NULL;
				l->rgbGen=RGBGEN_IDENTITY;
			}
			else {
				l->map[0]=world->texmng.AddTexinfo(Parser::token, l->flags);
			}
			continue;
		}
		
		else if(!stricmp(Parser::token, "clampmap")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			l->flags|=LF_CLAMP;
			l->map[0]=world->texmng.AddTexinfo(Parser::token, l->flags);
			
			continue;
		}
		
		else if(!stricmp(Parser::token, "animmap")){
			int num_maps=0;
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			l->anim_speed=(float)atof(Parser::token);
			
			while(Parser::GetToken(false)){
				l->map[num_maps]=world->texmng.AddTexinfo(Parser::token, l->flags);
				num_maps++;
			}
			l->num_maps=num_maps;
			continue;
		}
		
		else if(!stricmp(Parser::token, "blendfunc")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}

			if(!stricmp(Parser::token, "add")){
				l->blendsrc=IntForConst("GL_ONE");
				l->blenddst=IntForConst("GL_ONE");
				l->depthWrite=false;
			}
			else if(!stricmp(Parser::token, "filter")){
				l->blendsrc=IntForConst("GL_ZERO");
				l->blenddst=IntForConst("GL_SRC_COLOR");
				l->depthWrite=false;
			}
			else if(!stricmp(Parser::token, "blend")){
				l->blendsrc=IntForConst("GL_SRC_ALPHA");
				l->blenddst=IntForConst("GL_ONE_MINUS_SRC_ALPHA");
				l->depthWrite=false;
			}
			else {	// TODO: analize better the blendfunc!!
				l->blendsrc=IntForConst(Parser::token);
				if(!Parser::GetToken(false)){
					TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
					continue;
				}	
				l->blenddst=IntForConst(Parser::token);
				if(l->blendsrc==1&&l->blenddst==0&&s->num_layers==0) l->depthWrite=true;
				else l->depthWrite=false;
			}

			if(s->num_layers==0 && s->sortvalue==SORT_OPAQUE/*default*/){
				// FIXME: thats a hack to work around texture sorting problems, id docs say
				//		  that any shader with blendfunc has SORT_ADDITIVE by default.
				if(l->blendsrc==1&&l->blenddst==1&&s->num_layers==0) s->sortvalue=SORT_ADDITIVE;
				else s->sortvalue=SORT_SEETHROUGH;
			}
			//if(l->blendsrc==IntForConst("GL_SRC_ALPHA") && l->blenddst==IntForConst("GL_ONE_MINUS_SRC_ALPHA")) l->rgbGen=RGBGEN_IDENTITY_LIGHTING;
			//else if(l->blendsrc==IntForConst("GL_ONE") && l->blenddst==IntForConst("GL_ONE")) l->rgbGen=RGBGEN_IDENTITY_LIGHTING;
			if(l->blendsrc==IntForConst("GL_ZERO") && l->blenddst==IntForConst("GL_SRC_COLOR")) l->rgbGen=RGBGEN_IDENTITY;
			else if(l->blendsrc==IntForConst("GL_DST_COLOR") && l->blenddst==IntForConst("GL_ZERO")) l->rgbGen=RGBGEN_IDENTITY;


		}
		
		else if(!stricmp(Parser::token, "depthfunc")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "lequal")) l->depthFunc=IntForConst("GL_LEQUAL");
			else if(!stricmp(Parser::token, "equal")) l->depthFunc=IntForConst("GL_EQUAL");
			else {
				// warning!!
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown parameter %s in line %d\n", Parser::token, Parser::scriptline);
			}
		}
		
		else if(!stricmp(Parser::token, "depthwrite")){
			l->depthWrite=true;
		}
		
		else if(!stricmp(Parser::token, "detail")){
			l->flags|=LF_DETAIL;
		}
		
		else if(!stricmp(Parser::token, "alphafunc")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "gt0")){
				l->alphafunc=IntForConst("GL_GREATER");
				l->alphafuncref=0.0f;
				l->depthWrite=false;
			}
			else if(!stricmp(Parser::token, "lt128")){
				l->alphafunc=IntForConst("GL_LOWER");
				l->alphafuncref=0.5f;
				l->depthWrite=false;
			}
			else if(!stricmp(Parser::token, "ge128")){
				l->alphafunc=IntForConst("GL_GEQUAL");
				l->alphafuncref=0.5f;
				l->depthWrite=false;
			}
			else{
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown parameter %s in line %d\n", Parser::token, Parser::scriptline);
			}
		}

		else if(!stricmp(Parser::token, "rgbgen")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "vertex")){
				s->flags|=SF_HASCOLORS;
				l->rgbGen=RGBGEN_VERTEX;
			}
			else if(!stricmp(Parser::token, "lightingdiffuse")){
				s->flags|=SF_HASCOLORS;
				l->rgbGen=RGBGEN_LIGHTING_DIFFUSE;
			}
			else if(!stricmp(Parser::token, "oneminusvertex")){
				s->flags|=SF_HASCOLORS;
				l->rgbGen=RGBGEN_ONE_MINUS_VERTEX;
			}
			else if(!stricmp(Parser::token, "identity")){
				l->rgbGen=RGBGEN_IDENTITY;
			}
			else if(!stricmp(Parser::token, "identitylighting")){
				l->rgbGen=RGBGEN_IDENTITY_LIGHTING;
			}
			else if(!stricmp(Parser::token, "wave")){
				l->rgbGen=RGBGEN_WAVE;
				l->rgbGenParams[0]=parseFunc();
				for(int i=1; i<5; i++){
					if(!Parser::GetToken(false)) break;
					l->rgbGenParams[i]=(float)atof(Parser::token);
				}
			}

			while(Parser::GetToken(false)){TSystem::err->Log(UNKNOWN_ERROR, "Warning, skipping %s in line %d\n", Parser::token, Parser::scriptline);};
			continue;
		}
		else if(!stricmp(Parser::token, "alphagen")){
			// TODO: add distance atenuation !!
			while(Parser::GetToken(false)){};
			continue;
		}

		else if(!stricmp(Parser::token, "tcgen")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "environment")){
				l->tcGen=TCGEN_ENVIRONMENT;
				l->flags|=LF_USENORMALS;
				s->flags|=SF_USENORMALS;
			}

			while(Parser::GetToken(false)){TSystem::err->Log(UNKNOWN_ERROR, "Warning, skipping %s in line %d\n", Parser::token, Parser::scriptline);};
			continue;
		}
		
		else if(!stricmp(Parser::token, "tcmod")){
			if(!Parser::GetToken(false)){
				TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
				continue;
			}
			if(!stricmp(Parser::token, "rotate")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_ROTATE;
				f->parameters=1;
				for(int i=0; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "scale")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_SCALE;
				f->parameters=2;
				for(int i=0; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "scroll")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_SCROLL;
				f->parameters=2;
				for(int i=0; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "stretch")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_STRETCH;
				f->parameters=5;
				f->p[0]=parseFunc();
				for(int i=1; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "transform")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_TRANSFORM;
				f->parameters=6;
				for(int i=0; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}
			else if(!stricmp(Parser::token, "turb")){
				funclist *f;
				l->flags|=LF_HASTCMOD;
				s->flags|=SF_HASTCMOD;
				if(!l->tcMod) f=l->tcMod=new funclist;
				else{
					for(f=l->tcMod; f->next; f=f->next){};
					f=f->next=new funclist;
				}
				f->next=NULL;
				f->func=TCMOD_TURB;
				f->parameters=4;
				for(int i=0; i<MAX_PARAMS; i++){
					if(!Parser::GetToken(false)) break;
					f->p[i]=(float)atof(Parser::token);
				}
			}

			while(Parser::GetToken(false)){TSystem::err->Log(UNKNOWN_ERROR, "Warning, skipping %s in line %d\n", Parser::token, Parser::scriptline);};
			continue;
		}

		else{
			TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown parameter %s in line %d\n", Parser::token, Parser::scriptline);
		}

		// unknown keyword skip line
		while(Parser::GetToken(false)){TSystem::err->Log(UNKNOWN_ERROR, "Warning, skipping %s in line %d\n", Parser::token, Parser::scriptline);};

	}
};


float ShaderFile::parseFunc(void){
	if(!Parser::GetToken(false)){
		TSystem::err->Log(UNKNOWN_ERROR, "Warning, parameter expected in line %d\n", Parser::scriptline);
		return 0;
	}
	
	if(!stricmp(Parser::token, "sin")) return FUNC_SIN;
	else if(!stricmp(Parser::token, "triangle")) return FUNC_TRIANGLE;
	else if(!stricmp(Parser::token, "square")) return FUNC_SQUARE;
	else if(!stricmp(Parser::token, "sawtooth")) return FUNC_SAWTOOTH;
	else if(!stricmp(Parser::token, "inversesawtooth")) return FUNC_INVERSESAWTOOTH;
	else {
		TSystem::err->Log(UNKNOWN_ERROR, "Warning, unknown function in line %d\n", Parser::scriptline);
		return 0;
	}
};


