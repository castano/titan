/*===========================================================================
	Title: layer.h
	Author: Ignacio Castaño Aguado
	Description: layer allocation
	Classes:
		LayerMng	- manages the layers
===========================================================================*/
#ifndef __LAYER_H
#define __LAYER_H


/*
This class is much simpler than the ShaderMng class, it only allocates layers and fills
them with default values.
In the future a best allocation scheme could be used, to maintain all the layers in 
blocks.
This class could also be used to create special layers without effort, example: 
usual lightmap layer, usual model layer, or the usual wall texture layer.
*/


/*---------------------------------------------------------------------------
	Layer manager
---------------------------------------------------------------------------*/

class LayerMng {
public:
	
	LayerMng(void);
	~LayerMng(void);

	Layer *CreateLayer(void);
	void DeleteLayer(Layer *);

private:
	// useless statistics
	int num_layers;

};



#endif	// __LAYER_H