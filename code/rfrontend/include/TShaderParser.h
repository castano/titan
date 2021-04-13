/*===========================================================================
	Title: TShaderParser.h
	Author: Ignacio Castaño Aguado
	Description: shader parsing
	Classes:
		ShaderParser

===========================================================================*/
#ifndef __SHADERPARSER_H__
#define __SHADERPARSER_H__




/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/


//
//	ShaderParser
//
class ShaderParser : public Parser {
public:

	char *	GetFirstShader(void);
	char *	GetNextShader(void);
	void	SkipShader(void);

private:
	// methods:
	void	ParseShader(void);
	void	ParseLayer(void);
	float	ParseFunc(void);
	
	bool	GetName(void);
	bool	GetCommand(void);
	bool	GetParameter(void);

	void	InvalidToken(void);
	void	SkipLine(void);
	void	IgnoreLine(void);

	// general shader commands
	virtual void ShaderCommand_skyParms(void);
	virtual void ShaderCommand_cull(void);
	virtual void ShaderCommand_deformVertexes(void);	
	virtual void ShaderCommand_fogParams(void);
	virtual void ShaderCommand_nopicmip(void);
	virtual void ShaderCommand_nomipmaps(void);
	virtual void ShaderCommand_polygonOffset(void);
	virtual void ShaderCommand_portal(void);
	virtual void ShaderCommand_sort(void);

	// map processing tools shader commands
	virtual void ShaderCommand_tessSize(void);
	virtual void ShaderCommand_q3map_backshader(void);
	virtual void ShaderCommand_q3map_globaltexture(void);
	virtual void ShaderCommand_q3map_sun(void);
	virtual void ShaderCommand_q3map_surfaceLight(void);
	virtual void ShaderCommand_q3map_lightImage(void);
	virtual void ShaderCommand_q3map_lightSubdivide(void);
	virtual void ShaderCommand_surfaceParm(void);

	// qeradiant shadercommands
	virtual void ShaderCommand_qer_editorimage(void);
	virtual void ShaderCommand_qer_nocarve(void);
	virtual void ShaderCommand_qer_trans(void);

	// undocumented shader commands
	virtual void ShaderCommand_backsided(void);
	virtual void ShaderCommand_cloudParms(void);
	virtual void ShaderCommand_entityMergable(void);
	virtual void ShaderCommand_fogGen(void);
	virtual void ShaderCommand_fogOnly(void);
	virtual void ShaderCommand_light(void);
	virtual void ShaderCommand_lightning(void);
	virtual void ShaderCommand_q3map_backsplash(void);
	virtual void ShaderCommand_q3map_flare(void);
	virtual void ShaderCommand_sky(void);

	// layer commands
	virtual void LayerCommand_map(void);
	virtual void LayerCommand_clampMap(void);
	virtual void LayerCommand_animMap(void);
	virtual void LayerCommand_blendFunc(void);
	virtual void LayerCommand_rgbGen(void);
	virtual void LayerCommand_alphaGen(void);
	virtual void LayerCommand_tcGen(void);
	virtual void LayerCommand_tcMod(void);
	virtual void LayerCommand_depthFunc(void);
	virtual void LayerCommand_depthWrite(void);
	virtual void LayerCommand_detail(void);
	virtual void LayerCommand_alphaFunc(void);

	// undocumented layer commands
	virtual void LayerCommand_alphaMap(void);
	virtual void LayerCommand_depthMask(void);



	// members:
	VFile	shaderFile;
	int		errors;

};



#endif//__SHADERPARSER_H__
