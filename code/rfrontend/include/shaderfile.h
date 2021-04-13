/*===========================================================================
	Title: shaderfile.h
	Author: Ignacio Castaño Aguado
	Description: shader parsing
	Classes:
		ShaderFile extends VFile - uses Parser to parse the shaders

===========================================================================*/
#ifndef __SHADERFILE_H__
#define __SHADERFILE_H__



/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

//
//	Maps shader keywords to functions
//
typedef struct {
	char *keyword;
	int minargs, maxargs;
	void (* func)(Shader *shader, Layer *layer);
} shaderkey_t;




/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/

//
//	ShaderFile
//
class ShaderFile : public VFile {
public:
	ShaderFile(char *name):VFile(name){
		Parser::StartParseBuffer(mem, size);
	};
	char *GetShaderName(void);
	void Parse(Shader *s);
	void Skip(void);	// skip this shader
	// si hago lo que dije de no reservar memoria para todos los shaders a la vez, parse, se
	// debería encargar de hacerlo y devolver un puntero.

private:
	void ParseLayer(Shader *s, Layer *l);
	//void ParseParameters(Shader *shader, Layer *pass, const shaderkey_t *keys);
	float parseFunc(void);
	
};


#endif//__SHADERFILE_H__
