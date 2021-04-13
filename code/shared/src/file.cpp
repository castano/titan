/*===========================================================================
	Title: File.cpp
	Author: Ignacio Castaño Aguado
	Description: Simple but usefull file access classes
	Classes:
		FileEnvironment			- Global file acces functions
		VFile:FileEnvironment	- Read only file loader
		ParseFile:VFile			- File loader with parsing support
		LumpFile:ParseFile		- File loader with lump header and parsing support
		SearchFile				- Searchs file names by given pattern
		DumpFile:IDumpFile		- Inserts strings into a file
		StringList:DumpFile		- Linked list of strings that are inserted into a file
	Functions:
		f_DosToUnixName			- toggles \ to /
		f_UnixToDosName			- toggles / to \
		f_StripName				- strips the filename of a path
		f_Extension				- returns the extension of a filename
		f_StripExtension		- strips the exension of a filename
	TODO: 
		- support for searches on unix filesystems
		- support for bigendian
		- dont add folders to the search paths if they do not exist!!
===========================================================================*/


#include "shared.h"
#include <io.h>
#include <direct.h>
#include <string.h>
#include <stdarg.h> 
//#include <varargs.h> // not requiered



/*---------------------------------------------------------------------------
	Types
---------------------------------------------------------------------------*/

typedef struct PackedFile_s{
	char			name[1024];			// FIXME: static allocation
	int				offset;
	int				size;
} PackedFile;

typedef struct Pack_s {
	char *		name;
	int			nFiles;
	PackedFile *files;
	Pack_s *	next;
}Pack;




/*---------------------------------------------------------------------------
	Globals
---------------------------------------------------------------------------*/

static Pack *packets=NULL;
int	nPackedFiles	= 0;




/*---------------------------------------------------------------------------
	Members
---------------------------------------------------------------------------*/



//
//	FileEnvironment
//

search *FileEnvironment::Search=NULL;

char FileEnvironment::main_path[];

void FileEnvironment::Init(void){
	char pth[MAX_FILEPATH];

#ifdef WIN32
	_getcwd(pth, MAX_FILEPATH);	// get current path
	strcat(pth, "\\");
#endif

	/*
	in unix getcwd returns the current path, but not the path where the executable lies
	that can be solved changing the current path first, querying the current path and
	restoring it.
	*/
#ifdef LINUX
	//strcpy(pth, argv0);
	//f_StripName(pth);
	//chdir(pth);

	getcwd(pth, MAX_FILEPATH);
	strcat(pth, "/");
#endif

	strncpy(FileEnvironment::main_path, pth, MAX_FILEPATH);
}

void FileEnvironment::AddPath(const char *path){
	search *tmp;
	tmp=new search;
	strncpy(tmp->path, main_path, MAX_FILEPATH);
	strcat(tmp->path, path);
	tmp->next=FileEnvironment::Search;
	FileEnvironment::Search=tmp;
}

void FileEnvironment::LoadPacks(void){
	char *			name=NULL;
	FILE *			fp;
	ZIPEnd			End;
	ZIPCtrlHeader	File;
	Pack			*packet;

	char fname[512];					// FIXME: static allocation

	TSystem::out->Insert("--- Reading Packs:\n");
	SearchFile packs("*.pk3");
	
	// for each .pk3 file open it and add it files to the index.
	for(name=packs.FirstFile(); name; name=packs.NextFile()){
	    
		for(search *p=FileEnvironment::Search; p; p=p->next) {
			sprintf(fname,"%s%s",p->path,name);
			#ifdef WIN32
				f_UnixToDosName(fname);
			#else
				f_DosToUnixName(fname);
			#endif
			if((fp=fopen(fname,"rb"))) break;
		}
		
		if (!fp) continue;		// that will never happen
		TSystem::out->Insert("---	%s\n", fname);

		// Read the final header
		fseek( fp, -22, SEEK_END );
		fread( &End,  sizeof( End ), 1,  fp );
		if(End.Signature != ZIPEndSig ){
			TSystem::err->CriticalError(CRITICAL_ERROR, "Corrupted file %s\n", name );
			fclose(fp);
			continue;
		}

		packet = new Pack;
		packet->next = packets;
		packets = packet;
		packet->name = new char[strlen(fname)+1];
		strcpy(packet->name, fname);
		packet->nFiles = End.FilesOnDisk;
		packet->files = new PackedFile[End.FilesOnDisk];

		
		// Read the header of each file
		fseek( fp, End.Offset, SEEK_SET );
		for(int n=0; n < End.FilesOnDisk; n++ ) {
			fread(&File, sizeof(File), 1, fp);
			if(File.Signature != ZIPCtrlHeaderSig){
				TSystem::err->CriticalError(CRITICAL_ERROR, "Corrupted file %s\n", name);
				fclose(fp);
				break;			// continue with next pak
			}

			packet->files[n].size=File.UnCompressedSize;
			packet->files[n].offset=File.Offset;
			appMemzero(packet->files[n].name, 1024);	// FIXME: hack???
			fread(packet->files[n].name, 1, File.FileNameLength, fp);
			nPackedFiles++;			

			fseek(fp, File.ExtraLength + File.CommentLength, SEEK_CUR);
		}

		fclose(fp);
	}

	TSystem::out->Insert("--- %d packed files\n", nPackedFiles);
}


void FileEnvironment::Shut(void){
	search *s, *snext=NULL;

	for(s=FileEnvironment::Search; s; s=snext){
		snext=s->next;
		delete s;
	}

	Pack *p, *pnext=NULL;
	for (p=packets; p; p=pnext){
		delete [] p->name;
		delete [] p->files;
		pnext=p->next;
		delete p;
	}

}



//
//	VFile
//
VFile::VFile(const char *name, bool pak_search){
	FILE *	fp;
    mem=NULL;
	error=0;

	for(search *p=FileEnvironment::Search; p; p=p->next){

		sprintf(fname,"%s%s",p->path,name);
#ifdef WIN32
		f_UnixToDosName(fname);
#else
		f_DosToUnixName(fname);
#endif
		if((fp=fopen(fname,"rb"))==NULL) continue;

		// see the size
		fseek(fp, 0L, SEEK_END);
		size=ftell(fp);
		rewind(fp);

		// allocate memory
		mem=new BYTE[size];
		if(mem==NULL) {
			fclose(fp);
			error=ERROR_ALLOCATING;
			// FIXME: critical error!
			return;
		}

		// copy archive to memory
		fread(mem, size, sizeof(BYTE), fp);
		fclose(fp);
	
		return;
	}

	// Si no lo encontramos buscamos en los paks
	if (pak_search) {
		for (Pack *p=packets; p; p=p->next){
			for (int i=0; i<p->nFiles; i++) {
				if (stricmp(name, p->files[i].name)==0) {
					if((fp=fopen(p->name, "rb")) == NULL) 
						TSystem::err->Log(ERROR_LOADING_FILE, "Error abriendo el fichero %s ", p->name);
					fseek( fp, p->files[i].offset, SEEK_SET );

					mem = (BYTE *) UnZip( fp );
					size=p->files[i].size;
					fclose( fp );
				}
			}
		}
	}



	error=ERROR_LOADING;
	// display error!! : file not found!
	return;
}



VFile::~VFile(){
	// delete memory archive
	delete [] mem;
#ifdef DEBUG
	mem=NULL;
#endif

}






//
//	LumpFile
//

LumpFile::LumpFile(const char *name, const int id, const int ver, const int num_lumps):VFile(name){
	head = (header *)mem;

    if (head->id != id){
		//Error::Log("Bad file id in %s", name);
		return;
	}
    if (head->ver != ver){
		//Error::Log("Bad file version in %s", name);
		return;
	}
}

LumpFile::~LumpFile(void){
	// FIXME: como hacer que éste destructor no llame al destructor padre ?
	//		  ¿lo llama?
	// en todo caso se debería eliminar mem si es que aún no ha sido eliminada.
}

// TODO: test errors...
int LumpFile::ReadLump(int lump, void** dest, size_t elem){
	if(!mem) return NULL;
	int len = head->lump[lump].filelen;
	int num = len / elem;
	
	*dest = malloc(len);

	memcpy(*dest, mem + head->lump[lump].fileofs, num * elem);
	return num;
}



//
//	DumpFile
//

DumpFile::DumpFile(char *name){
	// FIXME: dumpfile must be created in the main_path path, 
	if((fp = fopen(name, "w+"))==NULL){
		// TODO: critical error
	}
}

DumpFile::~DumpFile(void){
	fclose(fp);
}

void DumpFile::Insert(const char *msg, ...){
	va_list arg;

	va_start(arg,msg);
	vsprintf(InsertCad,msg,arg);

	va_end(arg);
	
	int len=(int)strlen(InsertCad);
	int lastspace=0;
	int formated=0;
	int lines=1;
	if(len>95){
		for(int i=0;i<len;i++){
			if(InsertCad[i]==' ')lastspace=i;
			if(i-formated>95){
				// si la palabra es demasiado larga, la rompe: aquí se debería insertar un espacio
				// para despues sustituirlo por un salto. sin embargo, se sustituye un caracter
				// para no liarnos demasiao.
				if(lastspace==formated)	lastspace=i;
				formated=lastspace;
				lines++;
				InsertCad[lastspace]='\n';
			}
		}
	}

	fputs(InsertCad,fp);
	fflush(fp);
}




//
//	Search File
//

SearchFile::SearchFile(char *pattern){
	this->pattern=pattern;
	last=FileEnvironment::Search;
	temp_pat=NULL;
}

SearchFile::~SearchFile(void){
	delete temp_pat;
}

char *SearchFile::FirstFile(void){
	struct _finddata_t fileinfo;

	if(last==NULL) {		// end of the list, reset and return
		last=FileEnvironment::Search;
		return NULL;
	}

	delete [] temp_pat;
	temp_pat = new char[strlen(last->path) + strlen(pattern)+1];
	strcpy(temp_pat,last->path);
	strcat(temp_pat,pattern);

	f_UnixToDosName(temp_pat);

	handle = _findfirst (temp_pat, &fileinfo);
	if (handle == -1){
		_findclose(handle);
		delete [] temp_pat; temp_pat=NULL;
		last=last->next;
		return FirstFile();
	}

	// skip non-valid filenames
	while ((fileinfo.attrib&_A_SUBDIR) || fileinfo.name[0]=='.'){
		if(_findnext(handle,&fileinfo)==-1){
			_findclose(handle);
			return NULL;
		}
	}
	
	strcpy(lastpath, pattern);
	f_StripName(lastpath);
	strcat(lastpath, fileinfo.name);
	return lastpath;
}

char *SearchFile::NextFile(void){
	struct _finddata_t fileinfo;

	while(_findnext(handle, &fileinfo)!=-1) {
		if ((fileinfo.attrib & _A_SUBDIR))	continue;
		if (fileinfo.name[0] == '.') continue;
		
		strcpy(lastpath,pattern);
		f_StripName(lastpath);
		strcat(lastpath, fileinfo.name);
		return lastpath;
	}
	_findclose(handle);
	delete [] temp_pat; temp_pat=NULL;
	last=last->next;
	return FirstFile();
}





/*---------------------------------------------------------------------------
	Functions
---------------------------------------------------------------------------*/

// CAUTION: string must be well formated ('\0' at the end)
inline void f_DosToUnixName(char *name){
	for(int i=0; ; i++){
		if(name[i]=='\0') break;
		if(name[i]=='\\') name[i]='/';
	}
}

inline void f_UnixToDosName(char *name){
	for(int i=0; ; i++){
		if(name[i]=='\0') break;
		if(name[i]=='/') name[i]='\\';
	}
}

void f_StripExtension (char *path){

	int length = strlen(path)-1;	// ¿?
	while (length > 0 && path[length] != '.'){
		length--;
		if (path[length] == '/')
			return;		// no extension
	}
	if (length)
		path[length] = 0;
}

// atencion!! path no debe acabar en '/' o '\\', ¿o sí?
void f_StripName(char *path){
	int             length;

	length = strlen(path)-1;	// ¿?
	while (length > 0 && path[length] != '/' && path[length] != '\\'){
		length--;
	}
	if (length)
		path[length+1] = 0;
	else 
		path[0] = 0;
}

char * f_Name(char *path){
	int length, l;
	l = length = strlen(path)-1;
	while (length > 0 && path[length] != '\\' && path[length] != '/'){
		length--;
	}
	return &path[length+1];
}

char * f_Extension(char *name){
	int length, l;
	l = length = strlen(name)-1;	// ¿?
	while (length > 0 && name[length] != '.'){
		length--;
		if (name[length] == '/')
			return &name[l];		// no extension
	}
	if(length==0) return &name[l];
	return &name[length];

	//while(*name!='.'&&*name!=0)name++;
	//return name;
}