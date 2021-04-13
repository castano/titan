/*===========================================================================
	Title: File.h
	Author: Ignacio Castaño Aguado
	Description: Simple but usefull file access classes
	Classes:
		FileEnvironment			- Global file acces functions
		VFile:FileEnvironment	- Read only file loader
		LumpFile:ParseFile		- File loader with lump header and parsing support
		SearchFile				- Searchs file names by given pattern
		DumpFile:IDumpFile		- Inserts strings into a file
		StringList:DumpFile		- Linked list of strings that are inserted into a file
	Functions:
		f_DosToUnixName			- toggles \ to /
		f_UnixToDosName			- toggles / to \
		f_StripName				- strips the filename of a path
		f_Name					- returns the filename of file path
		f_Extension				- returns the extension of a filename
		f_StripExtension		- strips the exension of a filename
	TODO: 
		- support for searches on unix filesystems
		- support for bigendian
===========================================================================*/
#ifndef _FILE_INCLUDE
#define _FILE_INCLUDE




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define ERROR_LOADING		1
#define ERROR_ALLOCATING	2
#define MAX_FILEPATH		1024
#define CONSOLE_LINE		1024
#define FILE_NAME			256



/*---------------------------------------------------------------------------
	Definición de funciones locales.
---------------------------------------------------------------------------*/
extern char  * UnZip( FILE *h );




/*---------------------------------------------------------------------------
	Types
---------------------------------------------------------------------------*/

//
//	Typical string list
//
typedef struct _s{
	char path[MAX_FILEPATH];
	_s *next;
}search;


#pragma pack( push, before_ZIPStructs )
#pragma pack(1)

#define ZIPHeaderSig	0x04034b50
typedef struct  {
	LONG	Signature;		   //  (0x04034b50)
	WORD	Version;
	WORD	Flags;
	WORD	Method;
	LONG	LastMod;
	LONG	CRC32;
	LONG	CompressedSize;
	LONG	UnCompressedSize;
	WORD	FileNameLength;
	WORD	ExtraLength;
} ZIPHeader;

#define ZIPCtrlHeaderSig	0x02014b50
typedef struct  {
	LONG	Signature;		   //  (0x02014b50)
	WORD	VersionMade;
	WORD	VersionNeeded;
	WORD	Flags;
	WORD	Method;
	LONG	LastMod;
	LONG	CRC32;
	LONG	CompressedSize;
	LONG	UnCompressedSize;
	WORD	FileNameLength;
	WORD	ExtraLength;
	WORD	CommentLength;
	WORD	StartDisk;
	WORD	IniternalAttribs;
	LONG	ExternalAttribs;
	LONG	Offset;
} ZIPCtrlHeader;

#define ZIPEndSig			0x06054b50
typedef struct  {
	LONG	Signature;		   //  (0x06054b50)
	WORD	DiskNumber;
	WORD	StartDiskNumber;
	WORD	FilesOnDisk;
	WORD	Files;
	LONG	Size;
	LONG	Offset;
	WORD	CommentLength;
} ZIPEnd;

#pragma pack( pop, before_ZIPStructs )






/*---------------------------------------------------------------------------
	Classes
---------------------------------------------------------------------------*/

//
//	Global file acces functions
//
class FileEnvironment {
public:
	static void Init(void);
	static void Shut(void);
	static void AddPath(const char *path);
	static void LoadPacks(void);
	static search *Search;
	static char main_path[MAX_FILEPATH];
};



//
// Read-only virtual file
//
class VFile : public FileEnvironment{
public:
	VFile(const char *name, bool pak_search=true);
	~VFile(void);
	char	fname[FILE_NAME];
	BYTE *	mem;
	int		size;
	int		error;
};




//
// Read-only virtual file with lump header and parsing support
//
// tipical q1, q2 or q3 files
class LumpFile : public VFile{
public:
	LumpFile(const char *name, int id, int ver, const int num_lumps);
	~LumpFile(void);
	
	int ReadLump(int Lump, void** mem, size_t elem);
protected:

	struct header{
		int id, ver;
		struct { int fileofs, filelen; } lump[];	// is that allowed on most compilers ???
	} *head;
};



//
// Searchs for a filenames given a pattern
//
class SearchFile {
public:
	SearchFile(char *pattern="*");
	~SearchFile(void);

	char *FirstFile();
	char *NextFile();

private:
	int handle;						// io handle
	char *pattern;
	char *temp_pat;					// last pattern used.
	search *last;					// last path searched.
	char lastpath[MAX_FILEPATH];	// this will save the name of the last path until f_FindNext is called.
};



//
// Inserts strings into a file
//
class DumpFile : public IOutputDevice {
public:
	DumpFile(char *name);
	~DumpFile(void);

	void Insert(const char *str, ...);
private:
	int FileCreated;
protected:
	FILE *fp;
	char InsertCad[CONSOLE_LINE];
};




/*---------------------------------------------------------------------------
	Functions
---------------------------------------------------------------------------*/


inline void f_DosToUnixName(char *name);
inline void f_UnixToDosName(char *name);

char * f_Name(char *path);
void f_StripName(char *path);
char * f_Extension(char *name);
void f_StripExtension (char *path);


#endif // _FILE_INCLUDE