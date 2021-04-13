/*===========================================================================
	Title: overlay.h
	Author: Ignacio Castaño Aguado
	Description: draws bitmaps and strings
	Classes:
		Overlay					- overlay extends Object
===========================================================================*/


/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define MAX_VERTS		4096
#define MAX_ELEMS		8192
#define MAX_QUADS		1024



/*---------------------------------------------------------------------------
	Overlay
---------------------------------------------------------------------------*/

class Overlay {
public:
	void Init(void);
	void Render(void);

	void Quad(int shader, float x, float y, float w, float h, float repx=1, float repy=1);
	void String(char *str, int shader, float x, float y, float size=1, int color=0);
private:

	int num_quads;
	int num_elems;
	int num_verts;

	/*struct quad_s {
		int *first_elem;
		int num_elems;
		vertex_t *first_vert;
		int num_verts;
		int shader;
	} quads[MAX_QUADS];
*/
	Surface quads[MAX_QUADS];

	int elems[MAX_ELEMS];

	// FIXME: use a different vertex format
	vertex_t verts[MAX_VERTS];

//	DataSource data;

};
