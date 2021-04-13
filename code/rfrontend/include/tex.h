/*===========================================================================
	Title: tex.h
	Author: Ignacio Castaño Aguado
	Description: texture loading and management

===========================================================================*/
#ifndef _TEX__H_
#define _TEX__H_




/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

typedef struct _texrefs{
	char *name;
	int count;					// TODO: count could be used to give them priorities
	int flags;
	texinfo *info;				// each texref has an unique texinfo
}TexRefs;





/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

void AdjustColor(texinfo *t, float overbright, float gamma, float lightness);





/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

class TexMng {
public:
	/* 
	FIXME:  instead of using a fixed size array, we could use a grow array,
			note that we need inmediate aleatory acces.
	*/

	TexMng(int num=1024);
	~TexMng(void);


	texinfo *AddTexinfo(const char *name, int flags);
	
	void DeleteTexinfo(texinfo *);

	void ResetAll(void);
	void Update(void);
	void DeleteAll(void);

private:

	void LoadTexture(TexRefs *tr);

	void TGA_Decode(VFile *vf, texinfo *tex);
	void JPG_Decode(VFile *vf, texinfo *tex);
	void WAL_Decode(VFile *vf, texinfo *tex);
	void LoadDefaultTex(texinfo *t);


	TexRefs *texrefs;
	int	max_refs;
	int num_refs;

};


#endif // _TEX__H_
