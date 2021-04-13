/*===========================================================================
	Title: BezierPatch.h
	Author: Ignacio Castaño Aguado
	Description: Bezier Patch rendering trough recursive subdivision
	Classes:
		BezierPatch
===========================================================================*/

#ifndef BEZIER__PATCH_
#define BEZIER__PATCH_

class BezierPatch {
public:
	BezierPatch(void);
	~BezierPatch(void);
	
	void Build(face_t *face, Surface *surf);
	
	void Init(void);

	//void SetSubdiv(int k);

private:
#if 1
	void subdivide_curve(int idx, int step, int level);
	void gen_elems(int step_x, int step_y);

	int u_planar( int cp_x, int cp_y, vertex_t *v );
	int v_planar( int cp_x, int cp_y, vertex_t *v );

	int u_size( int cp_x, int cp_y, vertex_t *v );
	int v_size( int cp_x, int cp_y, vertex_t *v );

	int size_x, size_y;
	int subdiv_x, subdiv_y;
	
	int max_elems;

	enum {
		BZ_NORMAL=0x00,								// default
		BZ_UPLANAR=0x01,							// colinear u bezier control points
		BZ_VPLANAR=0x02,							// colinear v bezier control points
		BZ_PLANAR=BZ_UPLANAR|BZ_VPLANAR,			// plane
		BZ_TRIANGULAR=0x04,							// edge collapsed
		BZ_CYLINDER=0x08,							// shared edge
	};
	int bz_type;

	BitArray *	verts_used;
#endif
	int *		elems;
	vertex_t *	verts;


	int num_elems;
	int num_verts;

#if 1
	int find_size(vec3_t v0, vec3_t v1, vec3_t v2);
	void fill_cp_curve(int *sizes, vertex_t *vert, vertex_t *cp, int num_cp);
	void fill_patch_v(int num_v, int *sizes_v, int size_u, vertex_t *vert);
	void fill_patch_u(int num_u, int *sizes_u, vertex_t *vert);
	void fill_curve(int size, int stride, vertex_t *p);
#endif


};

#endif