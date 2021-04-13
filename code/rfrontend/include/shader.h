/*===========================================================================
	Title: shader.h
	Author: Ignacio Castaño Aguado
	Description: shader database, and shader loading
	Classes:
		ShaderMng	- manages the shaders
===========================================================================*/
#ifndef __SHADER_H
#define __SHADER_H




/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

typedef struct Node_s{
	int			lightmap;
	vertex_t *	firstvert;
	int			numverts;
	int	*		firstelem;
	int			numelems;
}Node;


typedef struct {
	int		num_nodes;
	Node*	list;		// list of elements asociated with this shader

	int		ref_count;	// number of times this shader has been referenced (for cache issues)
	Shader*	shader;		// the shader itself

	char*	name;		// name of the shader
	int		flags;		// surface flags
	int		type;		// ?

}ShaderHash;





/*---------------------------------------------------------------------------
	Shader manager
---------------------------------------------------------------------------*/

class ShaderMng {
public:

	ShaderMng(const int num=256);// allocate space for the references
	~ShaderMng(void);

	int	AddShader(const char *, int flags=0, int type=FACETYPE_NORMAL);		// adds shader to the references
	void DeleteShader(const int num);										// un-adds a shader reference

	
	inline Shader *GetShader(const int num){ return ShaderTable[num].shader;}// return shader of the reference

	void ResetAll(void);	// reset all reference counts
	void Update(void);		// updates the shaders acording to the references
	void DeleteAll(void);	// delete all the shaders


private:

	void DefaultShader(int i);
	Shader * CreateShader(void);
	Layer * CreateLayer(void);
	void DeleteReference(int i);


	ShaderHash *ShaderTable;
	int max_refs;
	int num_refs;

};




#endif	// __SHADER_H