/*===========================================================================
	Title: bsp.h
	Author: Ignacio Castaño Aguado
	Description: Bsp loader and renderer, main object
	Classes:
		Q3BSP					- quake3 bsp extends Object and LumpFile
===========================================================================*/






/*---------------------------------------------------------------------------
	Data of the bsp:
---------------------------------------------------------------------------*/

// Model 0 is the main map, others are doors, gates, buttons, etc.
typedef struct {
    bboxf_t bbox;
    int firstface, numfaces;
	int firstbrush, numbrushes;
} model_t;


//
// Planes
//
typedef struct {
	vec3_t	normal;
	float	dist;
}plane_t;


//
// Nodes in the BSP tree
//
typedef struct {
    int plane;			// Dividing plane
    int children[2];	// Left and right node.  Negatives are leafs
    bbox_t bbox;
} node_t;


//
// Leafs in BSP tree
//
typedef struct {
    int cluster;		// Visibility cluster number
    int area;			// area minus one ???
    bbox_t bbox;
    int firstface, numfaces;
    int firstbrush, numbrushes;
} leaf_t;


//
// Brushes in BSP tree
//
typedef struct {
	int firstside;
	int numsides;
	int contents;
}brush_t;


//
// Brushes sides in BSP tree
//
typedef struct {
	int planenum;
	int content;			// ¿?shader¿?
}brushside_t;


//
// Shader references (indexed from faces)
//
typedef struct {
    char name[64];
	int surface_flags;
    int content_flags;
} shaderref_t;


//
// Visibility info
//
typedef struct {
    int numclusters;   /* Number of PVS clusters */
    int rowsize;
    BYTE data[1];
} visibility_t;


/*---------------------------------------------------------------------------
	Data of the bsp:
---------------------------------------------------------------------------*/

//
// Faces (or surfaces)
//
typedef struct {
    int shader;			// Shader reference
    int facetype;		// FACETYPE enum
    vertex_t *firstvert;
	int numverts;
    int *firstelem;
	int numelems;
    int lm_texnum;
    vec3_t v_orig;		// FACETYPE_NORMAL only
    vec3_t v_norm;		// FACETYPE_NORMAL only
	float planedist;
} cnormalface_t;


//
// Brushes sides in BSP tree
//
typedef struct {
	cplane_t *planenum;
	int content;			// ¿?shader¿?
}cbrushside_t;


//
// Brushes in BSP tree
//
typedef struct {
	cbrushside_t *firstside;
	int numsides;
	int contents;
}cbrush_t;


//
// Nodes in the BSP tree
//
typedef struct cnode_s {
	cnode_s *	parent;
	int			framenum;
    cplane_t *	plane;			// Dividing plane
    int			children[2];	// Left and right node.  Negatives are leafs
    bbox_t		bbox;
} cnode_t;


//
// Leafs in BSP tree
//
typedef struct {
	cnode_s *	parent;
	int			framenum;
    int			cluster;		// Visibility cluster number
    int			area;			// area minus one ???
    bbox_t		bbox;
    int *		firstface;
	int			numfaces;
    int *		firstbrush;
	int			numbrushes;
} cleaf_t;



// Model 0 is the main map, others are doors, gates, buttons, etc.
typedef struct {
	bboxf_t bbox;
	face_t *firstface;
	int numfaces;
	cbrush_t *firstbrush;
	int numbrushes;
} cmodel_t;










/*---------------------------------------------------------------------------
	Q3BSP
---------------------------------------------------------------------------*/

class Q3BSP : public Object, LumpFile {
public:
	Q3BSP(char *);
	~Q3BSP(void);

	void		Init(void);
	void		Update(void);

	OBJECT_TYPE Id(void){return OB_Q3BSP;};

	void		Report(void);

	
	void		Render(void);

private:

	int			FindLeaf(vec3_t p);	
	void		MarkLeafs(int vis_cluster);
	void		WalkTree(int idx, int planeflags);
	void		DrawFaces(void);


	// callback to sort the faces
	static int face_cmp(const void *a, const void *b);

	
	void		LoadShaders(void);
	void		LoadPlanes(void);
	void		LoadNodes(void);
	void		LoadLeafs(void);
	void		LoadLFaces(void);
	void		LoadLBrushes(void);
	void		LoadModels(void);
	void		LoadBrushes(void);
	void		LoadBrushSides(void);
	void		LoadVerts(void);
	void		LoadElems(void);
	void		LoadFaces(void);
	void		LoadLightmaps(void);
	void		LoadVisibility(void);

	void		SetParent(int node, cnode_t *parent);
	void		CalcFaceBounds(Surface *f);


	// sizes of the loaded data
	int				r_nummodels;
	int				r_numverts;
	int				r_numplanes;
	int				r_numleafs;
	int				r_numnodes;
	int				r_numshaders;
	int				r_numfaces;			// total number of faces
	int				r_numlfaces;
	int				r_numelems;
	int				r_numlightmaps;
	int				r_numlbrushes;
	int				r_numbrushes;
	int				r_numbrushsides;


	// pointers to loaded data
	model_t *		r_models;
	vertex_t *		r_verts;
	cleaf_t *		r_leafs;
	cnode_t *		r_nodes;
	shaderref_t *	r_shaderrefs;
	int *			r_lfaces;
	int *			r_elems;
	visibility_t *	r_visibility;
	texinfo	*		r_lightmapinfos;
	int *			r_lbrushes;
	brush_t *		r_brushes;
	brushside_t *	r_brushsides;
	cplane_t *		r_clipplanes;


	int				r_numpatches;

	Surface	*		r_faces;
	BezierPatch	*	r_patches;	
	

	struct render_face_s {
		Surface *face;
		int sort_value;
	} *face_list;

	
	
	// frame counters to know if a node or a leaf must be walked or drawn
	int nodecount;			// nodecount is different because nodes only are updated when the camera leaf changes
	int viscount;

};
