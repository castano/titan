/*===========================================================================
	Title: BezierPatch.cpp
	Author: Ignacio Castaño Aguado
	Description: Bezier Patch rendering trough recursive subdivision.
		This object works as 'child' of the bsp object.
	Classes:
		BezierPatch
	TODO:
		colors arent interpolated, do i need to do it ?
		tesselate dynamicaly.

===========================================================================*/
#include "rfrontend.h"	// main header of the frontend files




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define MAX_SUBDIV 4

// usefull macro taken from golk:
#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IOutputDevice *out;
extern IRender *render;
extern vars *cvars;
extern World *world;


Var r_subdiv("r_subdiv", 4);


/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

inline static int colinear(vertex_t *a, vertex_t *b, vertex_t *c);
inline static void VertexLerp(vertex_t *a, vertex_t *b, vertex_t *dest);
inline static void VertexCopy(vertex_t *orig, vertex_t *dest);

inline static void VectorLerp(vec3_t a, vec3_t b, vec3_t c);



/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/
#if 1
BezierPatch::BezierPatch(void){
}

BezierPatch::~BezierPatch(void){
	delete [] verts;
	delete [] elems;
	delete verts_used;

}


void BezierPatch::Build(face_t *face, Surface *surf){
	int u, v;
	
	int control_x=face->mesh_cp[0];
	int control_y=face->mesh_cp[1];

	cvars->RegisterVar(r_subdiv);
	if(r_subdiv.ivalue<=0) r_subdiv=1;

	subdiv_x=u_size(control_x, control_y, surf->firstvert);
	subdiv_y=v_size(control_x, control_y, surf->firstvert);
	subdiv_x=subdiv_x>MAX_SUBDIV?MAX_SUBDIV:subdiv_x;				// clamp possible values
	subdiv_y=subdiv_y>MAX_SUBDIV?MAX_SUBDIV:subdiv_y;
	subdiv_x=subdiv_x<1?1:subdiv_x;
	subdiv_y=subdiv_y<1?1:subdiv_y;

	// calculate size of the arrays
	int step_x = 1<<(subdiv_x-1);
	int step_y = 1<<(subdiv_y-1);

	size_x = (control_x-1)*step_x+1;
	size_y = (control_y-1)*step_y+1;
	
	
	bz_type=BZ_NORMAL;
	if(v_planar(control_x, control_y, surf->firstvert)) bz_type|=BZ_VPLANAR;
	if(u_planar(control_x, control_y, surf->firstvert)) bz_type|=BZ_UPLANAR;

	if( bz_type&BZ_VPLANAR && bz_type&BZ_UPLANAR ) out->Insert("*** Warning: planar patch!");

	// special cases:
	if( bz_type&BZ_UPLANAR ){

		max_elems = (size_y-1)*6;
		num_verts = 2*size_y;
		size_x=2;

		// allocate memory for the arrays
		this->elems = new int[max_elems];
		this->verts = new vertex_t[num_verts];
		verts_used = new BitArray(num_verts);

		// write control points that lie on the curve
		vertex_t *vert = surf->firstvert;
	
		int i=0;
		for(v=0; v<size_y; v+=step_y, i++){
			memcpy( &this->verts[v * 2], &vert[i*control_x], sizeof(vertex_t) );
			memcpy( &this->verts[v * 2 + 1], &vert[i*control_x+control_x-1], sizeof(vertex_t) );
		}
		
		// interpolate control points vertically
		for (v = 0; v < size_y-1; v += step_y*2){			// for each patch
			subdivide_curve ( v*size_x, 2*step_y, subdiv_y);
			subdivide_curve ( v*size_x+1, 2*step_y, subdiv_y);
		}

	}
	else if( bz_type&BZ_VPLANAR ){

		max_elems = (size_x-1)*6;
		num_verts = 2*size_x;
		size_y=2;

		// allocate memory for the arrays
		this->elems = new int[max_elems];
		this->verts = new vertex_t[num_verts];
		verts_used = new BitArray(num_verts);


		// write control points that lie on the curve
		vertex_t *vert = surf->firstvert;
	
		int i=0;
		for(u=0; u<size_x; u+=step_x, i++){
			memcpy( &this->verts[u], &vert[i], sizeof(vertex_t) );
			memcpy( &this->verts[size_x + u], &vert[i+control_x*(control_y-1)], sizeof(vertex_t) );
		}
		

		// interpolate control points horizontally
		for (u = 0; u < size_x-1; u += step_x*2){			// for each patch
			subdivide_curve ( u, step_x, subdiv_x);
			subdivide_curve ( size_x+u, step_x, subdiv_x);
		}

	}
	// general case: homogeneus tesselation
	else {

		max_elems = (size_x-1)*(size_y-1)*6;
		num_verts = size_x*size_y;


		// allocate memory for the arrays
		this->elems = new int[max_elems];
		this->verts = new vertex_t[num_verts];
		verts_used = new BitArray(num_verts);


		// write control points that lie on the curve
		vertex_t *vert = surf->firstvert;
	
		for(v=0; v<size_y; v+=step_y){
			for(u=0; u<size_x; u+=step_x){
				int pos = v * size_x + u;
				memcpy( &this->verts[pos], vert, sizeof(vertex_t) );
				vert++;
			}
		}

		vert = surf->firstvert;

		// subdivide every patch homogeneously:

		// interpolate control points vertically
		for (u = 0; u < size_x; u += step_x){					// for each control point
			for (v = 0; v < size_y-1; v += step_y*2){			// for each patch
				subdivide_curve ( v*size_x+u, size_x*step_y, subdiv_y);
			}
		}

		// interpolate horizontally
		for (v = 0; v < size_y; v++){
			for (u = 0; u < size_x-1; u += step_x*2){
				subdivide_curve ( v*size_x+u, step_x, subdiv_x);
			}
		}
	}

	gen_elems(min(1, (1<<subdiv_x)), min(1, (1<<subdiv_y)));

	// write data on the surface
	surf->firstelem=elems;
	surf->firstvert=verts;
	surf->numelems=num_elems;
	surf->numverts=num_verts;
}



//
// Dummy Init
//
void BezierPatch::Init(void){
}



//
// Generate Elems according to the tesselation
//
// TODO: i think it would be nice to choose elements according only to the threshold, and 
//		 increase or decrease it depending on the step.
void BezierPatch::gen_elems(int step_x, int step_y){
	int u, v, i = 0;
//	vec3_t v1, v2;
//	float d1, d2;

	if( bz_type&BZ_UPLANAR ){
		for (v = 0; v < size_y-1; v+=step_y){
//			VectorSub(verts[v*2].v_point, cam.c, v1);
//			VectorSub(verts[v*2+1].v_point, cam.c, v2);
//			d1=sqrt(VectorFakeMod(v1));
//			d2=sqrt(VectorFakeMod(v2));
/*			d1=DotProduct(verts[v*2].v_point, cam.eyedir)-cam.eyedist;
			d2=DotProduct(verts[v*2+1].v_point, cam.eyedir)-cam.eyedist;
			d1=min(d1,d2)/50;
			if(d1<2)d1=2;
			step_y=mylog2(d1);
			if(step_y<1)step_y=1;
			if(step_y>(size_y-v>>1)) step_y=size_y-v-1;
*/			
			this->elems[i++] = v * size_x;
			this->elems[i++] = (v+step_y) * size_x;
			this->elems[i++] = v * size_x + 1;
			this->elems[i++] = v * size_x + 1;
			this->elems[i++] = (v+step_y) * size_x;
			this->elems[i++] = (v+step_y) * size_x + 1;
		}
	}
	else if( bz_type&BZ_VPLANAR ){
		for (u = 0; u < size_x-1; u+=step_x){
//			VectorSub(verts[u].v_point, cam.c, v1);
//			VectorSub(verts[u+2].v_point, cam.c, v2);
//			d1=sqrt(VectorFakeMod(v1));
//			d2=sqrt(VectorFakeMod(v2));
/*			d1=DotProduct(verts[u].v_point, cam.eyedir)-cam.eyedist;
			d2=DotProduct(verts[u+2].v_point, cam.eyedir)-cam.eyedist;
			d1=min(d1,d2)/50;
			if(d1<2)d1=2;

			step_x=mylog2(d1);
			if(step_x<1)step_x=1;
			if(step_x>(size_x-u>>1)) step_x=size_x-u-1;
*/			
			this->elems[i++] = u;
			this->elems[i++] = 1*size_x + u;
			this->elems[i++] = u + step_x;
			this->elems[i++] = u + step_x;
			this->elems[i++] = 1*size_x + u;
			this->elems[i++] = 1*size_x + u + step_x;
		}
	}
	else {
		for (v = 0; v < size_y-1; v+=step_y){
			for (u = 0; u < size_x-1; u+=step_x){
				this->elems[i++] = v * size_x + u;
				this->elems[i++] = (v+step_y) * size_x + u;
				this->elems[i++] = v * size_x + u + step_x;
				this->elems[i++] = v * size_x + u + step_x;
				this->elems[i++] = (v+step_y) * size_x + u;
				this->elems[i++] = (v+step_y) * size_x + u + step_x;
			}
		}
	}
	
	num_elems=i;
}



//
// Función que subdivide una curva bezier de orden 2
// Se le pasan como parámetros el índice del primer punto de control en el array, el incremento 
// necesario para hallar el siguiente punto, y el grado de subdivisión de la curva
//
void BezierPatch::subdivide_curve(int idx, int step, int level){

	int next=idx+step;
	int mid=step>>1;
	
	// do not interpolate more...
	if( (--level)==0 ) {
		vertex_t a, b;

		VertexLerp( &verts[idx], &verts[next], &a);
		VertexLerp( &verts[next+step], &verts[next], &b);
		VertexLerp( &a, &b, &verts[next]);

		return;
	}

	VertexLerp( &verts[idx], &verts[next], &verts[idx+mid]);
	VertexLerp( &verts[next+step], &verts[next], &verts[next+mid]);
	VertexLerp( &verts[idx+mid], &verts[next+mid], &verts[next]);

	subdivide_curve(idx, mid, level);
	subdivide_curve(next, mid, level);
}




//
// test u planarity
//
int BezierPatch::u_planar( int cp_x, int cp_y, vertex_t *vxt ){
/*
	testar 
	si v[0], v[1]...v[cp_x-1] es colinear,
	si v[cp_x+0], v[cp_x+1]... v[cp_x+cp_x-1] es colinear,
	si ....
	si v[cp_x*(cp_y-1)+0], v[cp_x*(cp_y-1)+1]... v[cp_x*(cp_y-1)+cp_x-1], es colinear
	si todos lo son entonces el patch es u_planar
*/
	int u, v;

	for(v=0; v<cp_y; v++){
		for(u=0; u<cp_x-2; u++){
			if(!colinear(&vxt[cp_x*v+u], &vxt[cp_x*v+(u+1)], &vxt[cp_x*v+(u+2)])) return false;
		}
	}

	return true;
}



//
// test v planarity
//
int BezierPatch::v_planar( int cp_x, int cp_y, vertex_t *vxt ){

/*
	identico al de arriba, pero transponiendo la matriz de puntos de control.
*/
	int u, v;

	for(u=0; u<cp_x; u++){
		for(v=0; v<cp_y-2; v++){
			if(!colinear(&vxt[cp_x*v+u], &vxt[cp_x*(v+1)+u], &vxt[cp_x*(v+2)+u])) return false;
		}
	}

	return true;
}



//
// calc u size
//
int BezierPatch::u_size( int cp_x, int cp_y, vertex_t *vxt ){
	int u, v;

	float size=0;
	float inc=0;

	for(v=0; v<cp_y; v++){
		inc=0;
		for(u=0; u<cp_x-1; u++){
			inc+=/*sqrt(*/
				(vxt[cp_x*v+u+1].v_point[0]-vxt[cp_x*v+u].v_point[0])*
				(vxt[cp_x*v+u+1].v_point[0]-vxt[cp_x*v+u].v_point[0])+
				(vxt[cp_x*v+u+1].v_point[1]-vxt[cp_x*v+u].v_point[1])*
				(vxt[cp_x*v+u+1].v_point[1]-vxt[cp_x*v+u].v_point[1])+
				(vxt[cp_x*v+u+1].v_point[2]-vxt[cp_x*v+u].v_point[2])*
				(vxt[cp_x*v+u+1].v_point[2]-vxt[cp_x*v+u].v_point[2])/*)*/;
		}
		size=max(size, inc);
	}

	return Log2((DWORD) (Sqrt(size)/80.0f*r_subdiv.fvalue)/*(cp_x-1)*/);
	//return Log2((int) size/40.0f*r_subdiv.fvalue/(cp_x-1));
}



//
// calc v size
//
int BezierPatch::v_size( int cp_x, int cp_y, vertex_t *vxt ){
	int u, v;

	float size=0;
	float inc;

	for(u=0; u<cp_x; u++){
		inc=0;
		for(v=0; v<cp_y-1; v++){
			inc+=/*sqrt(*/
				(vxt[cp_x*(v+1)+u].v_point[0]-vxt[cp_x*v+u].v_point[0])*
				(vxt[cp_x*(v+1)+u].v_point[0]-vxt[cp_x*v+u].v_point[0])+
				(vxt[cp_x*(v+1)+u].v_point[1]-vxt[cp_x*v+u].v_point[1])*
				(vxt[cp_x*(v+1)+u].v_point[1]-vxt[cp_x*v+u].v_point[1])+
				(vxt[cp_x*(v+1)+u].v_point[2]-vxt[cp_x*v+u].v_point[2])*
				(vxt[cp_x*(v+1)+u].v_point[2]-vxt[cp_x*v+u].v_point[2])/*)*/;
		}
		size=max(size, inc);
	}

	return Log2((DWORD) (Sqrt(size)/80.0f*r_subdiv.fvalue)/*(cp_y-1)*/);
	//return Log2((int) size/40.0f*r_subdiv.fvalue/(cp_y-1));
}

#else


//
// This is a better implementation of Bezier Patch, that has currently some errors.
//

BezierPatch::BezierPatch(void){
}

BezierPatch::~BezierPatch(void){
	delete [] verts;
	delete [] elems;
}



void BezierPatch::Build(face_t *face, Surface *surf){
	int u, v;
	vec3_t a, b, c;
	int size_u=1, size_v=1;

	int num_u=face->mesh_cp[0];
	int num_v=face->mesh_cp[1];
	
	vertex_t *vert;
	vertex_t *cp=surf->firstvert;


	cvars->RegisterVar(r_subdiv);
	if(r_subdiv.ivalue<=0) r_subdiv=1;


	int *sizes_u = new int[(num_u-1)/2];
	int *sizes_v = new int[(num_v-1)/2];
	appMemzero(sizes_u, sizeof(int)*(num_u-1)/2);
	appMemzero(sizes_v, sizeof(int)*(num_v-1)/2);


	//
	// I don't know why, but the sizes arent right 
	//

	// calculate u size
	for(v=0; v<num_v; v++){
		for(u=0; u<num_u-1; u+=2){
			VectorCopy(cp[num_u*v+u+0].v_point, a);
			VectorCopy(cp[num_u*v+u+1].v_point, b);
			VectorCopy(cp[num_u*v+u+2].v_point, c);
			sizes_u[u/2]=max(find_size(a, b, c), sizes_u[u/2]);
		}
	}
	for(u=0; u<(num_u-1)/2; u+=1) size_u+=sizes_u[u]-1;


	// calculate v size
	for(u=0; u<num_u; u++){
		for(v=0; v<num_v-1; v+=2){
			VectorCopy(cp[num_u*(v+0)+u].v_point, a);
			VectorCopy(cp[num_u*(v+1)+u].v_point, b);
			VectorCopy(cp[num_u*(v+2)+u].v_point, c);
			sizes_v[v/2]=max(find_size(a, b, c), sizes_v[v/2]);
		}
	}
	for(v=0; v<(num_v-1)/2; v+=1) size_v+=sizes_v[v]-1;


	// if we want uniform tesselation:
/*	size_u=1;
	for(u=0; u<num_u-1; u+=2){
		sizes_u[u/2]=5;
		size_u+=4;
	}

	size_v=1;
	for(v=0; v<num_v-1; v+=2){
		sizes_v[v/2]=2;
		size_v+=1;
	}
*/


	// allocate space for the mesh
	num_elems=(size_u-1)*(size_v-1)*6;
	this->elems = new int[num_elems];
	num_verts=size_u*size_v;
	verts = new vertex_t[num_verts];
	appMemzero(verts, sizeof(verts)*num_verts);	// FIXME: only for debuging purpouses



	vert=verts;
	cp=surf->firstvert;

	// fill mesh control points:
	fill_cp_curve(sizes_u, vert, cp, num_u);
	for(v=1; v<num_v; v+=2){
		int inc = sizes_v[(v-1)/2];
		vert+=size_u*(inc/2);
		cp+=num_u;
		if(inc!=2){
			fill_cp_curve(sizes_u, vert, cp, num_u);
			vert+=size_u*(inc/2);
		}
		cp+=num_u;
		fill_cp_curve(sizes_u, vert, cp, num_u);
	}


	// fill vertical lines
	vert=verts;
	fill_patch_v(num_v, sizes_v, size_u, vert);
	for(u=1; u<num_u; u+=2){
		int inc = sizes_u[(u-1)/2];		// FIXME: la division trunca !
		if(inc!=2){						// we have to copy a middle control point.
			vert += (inc/2);
			fill_patch_v(num_v, sizes_v, size_u, vert);
		}
		vert += inc/2;
		fill_patch_v(num_v, sizes_v, size_u, vert);
	}

	
	// NOTE: the major work is here:
	// fill horizontal lines:
	vert=verts;
	for(v=0; v<size_v; v++){
		fill_patch_u(num_u, sizes_u, vert);
		vert+=size_u;
	}


	int i=0;
	for (v = 0; v < size_v-1; v++){
		for (u = 0; u < size_u-1; u++){
			this->elems[i++] = v    *size_u	+ u;
			this->elems[i++] = (v+1)*size_u + u;
			this->elems[i++] = v	*size_u	+ u+1;
			this->elems[i++] = v	*size_u	+ u+1;
			this->elems[i++] = (v+1)*size_u + u;
			this->elems[i++] = (v+1)*size_u + u+1;
		}
	}


	// write data on the surface
	surf->firstelem=elems;
	surf->firstvert=verts;
	surf->numelems=num_elems;
	surf->numverts=num_verts;

	delete [] sizes_u;
	delete [] sizes_v;
}


//
// Dummy init
//
void BezierPatch::Init(void){
}







//
// returns number of vertexes
//
/*      @		 |
       /|\		 | dist
    @/__@__\@	 |_      mid
   /    |    \	 |
 /______@______\ | dist
@               @|

*/
int BezierPatch::find_size(vec3_t v0, vec3_t v1, vec3_t v2){
/*
	vec3_t mid;

	// find midle point
	VectorLerp( v0, v1, a );
	VectorLerp( v1, v2, b );
	VectorLerp( a, b, mid);

	// if distance of the midle point to the control point < max_error then tolerance reached
	VectorSub(mid, v1, dist);
	if( DotProduct(dist, dist) < lodCurveError*lodCurveError ) return 1;

	return find_size(idx, mid, level)+find_size(next, mid, level);
*/

	// FIXME: is tolerance reached in the same level for every subdivision?
	//		  if not, we should subdivide allways and record the last level reached

	int level;
	vec3_t a, b, dist;
	const float lodCurveError = 5;	// lodCurveError is the maximum error we may commit


	for (level=0; level < r_subdiv.ivalue; level++) {
		// subdivide on the left
		VectorLerp(v0, v1, a);
		VectorLerp(v1, v2, b);
		VectorLerp(a, b, v2);

		// find distance moved
		VectorSub(v2, v1, dist);

		// check for tolerance
		if (VectorDot(dist, dist) < lodCurveError*lodCurveError) break;

		// insert new middle vertex
		VectorCopy(a, v1);
	}

	return (1<<level)+1;	// number of vertexes 2^level+1 -> 2, 3, 5, 9...
}


//
// Fill the control points of the curve
//
void BezierPatch::fill_cp_curve(int *sizes, vertex_t *vert, vertex_t *cp, int num_cp){
	int inc;

	VertexCopy(cp, vert);

	for(int p=1; p<num_cp; p+=2){
		
		inc = sizes[(p-1)/2];		// FIXME: la division trunca !
		cp++;

		if(inc!=2){						// we have to copy a middle control point.
			vert += (inc/2);
			VertexCopy(cp, vert);
		}

		cp++;
		vert += inc/2;

		VertexCopy(cp, vert);

	}

}


//
// Fills a column of the bezier patch
//
void BezierPatch::fill_patch_v(int num_v, int *sizes_v, int size_u, vertex_t *vert){
	int v;

	for(v=0; v<num_v-1; v+=2){
		fill_curve(sizes_v[v/2], size_u, vert);
		vert+=sizes_v[v/2]*size_u;
	}
}


//
// Fills a row of the bezier patch
//
void BezierPatch::fill_patch_u(int num_u, int *sizes_u, vertex_t *vert){
	int u;

	for(u=0; u<num_u-1; u+=2){
		fill_curve(sizes_u[u/2], 1, vert);
		vert+=sizes_u[u/2]-1;
	}
}



//
// Interpolates a cuadratic bezier curve with 3 control points.
//
void BezierPatch::fill_curve(int size, int stride, vertex_t *p){
	int step, halfstep, i, mid;
	vertex_t a, b;

	if(size==2) return;

	step = (size-1) / 2;

	while (step > 0) {
		halfstep = step / 2;
		for (i=0; i < size-1; i += step*2) {
			mid = (i+step)*stride;
			VertexLerp(&p[i*stride], &p[mid], &a);
			VertexLerp(&p[mid], &p[(i+step*2)*stride], &b);
			VertexLerp(&a, &b, &p[mid]);

			if (halfstep > 0) {
				VertexCopy(&a, &p[(i+halfstep)*stride]);
				VertexCopy(&b, &p[(i+3*halfstep)*stride]);
			}
		}

		step /= 2;
	}
}






#endif













/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/


//
// test colinearity of 3 control points
//
// FIXME: remove the first tests? or are good to be faster?
inline static int colinear(vertex_t *a, vertex_t *b, vertex_t *c){

	// degenerate bezier control points are colinear !?!
	if( a->v_point[0]==b->v_point[0] && a->v_point[0]==c->v_point[0] &&
		a->v_point[1]==b->v_point[1] && a->v_point[1]==c->v_point[1] &&
		a->v_point[2]==b->v_point[2] && a->v_point[2]==c->v_point[2]){
		return true;
	}

	if( a->v_point[0]==b->v_point[0] && a->v_point[1]==b->v_point[1] && a->v_point[2]==b->v_point[2] ||
		b->v_point[0]==c->v_point[0] && b->v_point[1]==c->v_point[1] && b->v_point[2]==c->v_point[2] ||
		a->v_point[0]==c->v_point[0] && a->v_point[1]==c->v_point[1] && a->v_point[2]==c->v_point[2]){
		return true;
	}


	// we are only testing colinearity in the vertexes, but texcoords... could be not colinear
	if(
		FastAbs((b->v_point[1]-a->v_point[1])*(c->v_point[0]-a->v_point[0])-(c->v_point[1]-a->v_point[1])*(b->v_point[0]-a->v_point[0]))<0.02
		&&
		FastAbs((b->v_point[2]-a->v_point[2])*(c->v_point[0]-a->v_point[0])-(c->v_point[2]-a->v_point[2])*(b->v_point[0]-a->v_point[0]))<0.02
		&&
		FastAbs((b->v_point[2]-a->v_point[2])*(c->v_point[1]-a->v_point[1])-(c->v_point[2]-a->v_point[2])*(b->v_point[1]-a->v_point[1]))<0.02
		){
		return true;
	}
	return false;
}


//
// interpolate two control points
//
inline static void VertexLerp(vertex_t *a, vertex_t *b, vertex_t *dest){
	VectorAdd( a->v_point, b->v_point, dest->v_point);
	VectorScale(dest->v_point, 0.5f, dest->v_point);

	// FIXME: normalize the normals
	VectorAdd( a->v_norm, b->v_norm, dest->v_norm);
	VectorScale(dest->v_norm, 0.5f, dest->v_norm);
	FastNormVect3(dest->v_norm);	// FIXME: take care of divide by zero errors

	TexcoordAdd( a->tex_st, b->tex_st, dest->tex_st);
	TexcoordScale(dest->tex_st, 0.5f, dest->tex_st);
	TexcoordAdd( a->lm_st, b->lm_st, dest->lm_st);
	TexcoordScale(dest->lm_st, 0.5f, dest->lm_st);

//	ColorAdd( a->lm_st, b->lm_st, dest->lm_st);
//	ColorScale(dest->lm_st, 0.5f, dest->lm_st);
}

inline static void VertexCopy(vertex_t *orig, vertex_t *dest){
	VectorCopy( orig->v_point, dest->v_point);
	VectorCopy( orig->v_norm, dest->v_norm);

	TexcoordCopy( orig->tex_st, dest->tex_st);
	TexcoordCopy( orig->lm_st, dest->lm_st);

//	ColorCopy( orig->colour, dest->colour);
	dest->colour=orig->colour;
}

inline static void VectorLerp(vec3_t a, vec3_t b, vec3_t c){
	VectorAdd(a, b, c);
	VectorScale(c, 0.5f, c);
}












