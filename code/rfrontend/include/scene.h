/*===========================================================================
	Title: scene.h
	Author: Ignacio Castaño Aguado
	Description: List of objects
	Classes:
		Scene	- object list

===========================================================================*/
#ifndef _SCENE_H_
#define _SCENE_H_





/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

typedef struct _linked_obj{
	Object *obj;
	_linked_obj *next;
}linked_obj;





/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/


class Scene {
public:
	Scene(void);

	void Init(void);
	void Update(void);
	void Render(void);

	void Add(Object *obj);
	void DeleteAll();

private:
	linked_obj *first;
	linked_obj *current;

};



#endif //_SCENE_H_