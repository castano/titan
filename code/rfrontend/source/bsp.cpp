/*===========================================================================
	Title: bsp.cpp
	Author: Ignacio Castaño Aguado
	Description: Bsp loader and renderer, main object
	Classes:
		Q3BSP					- quake3 bsp
	Rendering algo: 
		go trough the nodes:
			if my clild is potentially visible, so i am.
		go trough the pv. nodes:
			if i am inside the frustum, my childs are also inside
			if we find a leaf mark his faces as visible.
		visible faces are send to the renderer

===========================================================================*/
#include "rfrontend.h"	// main header of the frontend files





/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IOutputDevice *out;
extern vars * cvars;
extern IRender *render;
extern World *world;
extern Error *err;




/*---------------------------------------------------------------------------
	Definitions:
---------------------------------------------------------------------------*/

#define BSPHEADERID  (*(int*)"IBSP")
#define BSPVERSION 46

#define BANK_SIZE (128*128*3)


// BSP lumps in the order they appear in the header
enum {
    ENTITIES,
    SHADERREFS,
    _PLANES,
    NODES,
    LEAFS,
    LFACES,				// leaf faces
    LBRUSHES,			// leaf brushes
    MODELS,
    BRUSHES,
	BRUSH_SIDES,
    VERTS,
    ELEMS,
    FOG,
    FACES,
    LIGHTMAPS,
    LIGHTGRID,
    VISIBILITY,
    NUM_LUMPS
};






// taken from aftershock:
// PVS test macro:  PVS table is a packed single bit array, rowsize
// bytes times numclusters rows
#define BSP_TESTVIS(from,to) \
        (*(r_visibility->data + (from)*r_visibility->rowsize + \
           ((to)>>3)) & (1 << ((to) & 7)))






/*---------------------------------------------------------------------------
	Initialization report and shutdown:
---------------------------------------------------------------------------*/

//
// Initialization
//
Q3BSP::Q3BSP(char *name):LumpFile(name, BSPHEADERID, BSPVERSION, NUM_LUMPS){
	guard(Q3BSP::Q3BSP);

	if(!mem) {
		err->Log(ERROR_LOADING_FILE, "%s is not a valid map name\n");
		return;
	}

	// --- Loading
	out->Insert("--- Loading bsp %s\n", name);
	// --- Parse Entities
	// TODO: parse entities:
	
	LoadShaders();
	LoadPlanes();
	LoadNodes();
	LoadLFaces();		// leaf faces before leafs
	LoadLBrushes();		// leaf brushes before leafs
	LoadLeafs();
	LoadBrushSides();	// brush sides before brushes
	LoadBrushes();
	LoadModels();
	LoadVerts();
	LoadElems();
	LoadFaces();
	LoadLightmaps();
	LoadVisibility();


	// init counters
	nodecount=0;
	viscount=0;


	// delete after loading
	delete [] mem; mem=NULL;

	unguard;
}



//
// Shutdown
//
Q3BSP::~Q3BSP(void){
	guard(Q3BSP::~Q3BSP);
	int i;


	free(r_shaderrefs);		r_shaderrefs=NULL;
	delete [] r_clipplanes;	r_clipplanes=NULL;
	delete [] r_nodes;		r_nodes=NULL;
	free(r_leafs);			r_leafs=NULL;	
	free(r_lfaces);			r_lfaces=NULL;
	free(r_lbrushes);		r_lbrushes=NULL;
	free(r_models);			r_models=NULL;
	free(r_brushes);		r_brushes=NULL;
	free(r_brushsides);		r_brushsides=NULL;
	free(r_verts);			r_verts=NULL;
	free(r_elems);			r_elems=NULL;

	for(i=0; i<r_numlightmaps; i++) render->UnregisterTexInfo(&r_lightmapinfos[i]);
	delete [] r_lightmapinfos;	r_lightmapinfos=NULL;

	free(r_visibility);		r_visibility=NULL;

	delete [] face_list;
	delete [] r_faces;

	delete [] r_patches;


	unguard;
}



//
// more initialization
//
void Q3BSP::Init(void){
	guard(Q3BSP::Init);
	// FIXME: sort allways when we look from any other content

	Surface *f;
	Shader *s;


	for(int i=0; i<r_numfaces; i++){
		f = face_list[i].face;
		if( f ){
			s = world->shaders.GetShader(f->shader);
			face_list[i].sort_value = ((s->sortvalue)<<16)+(f->shader<<8)+(f->lm_texnum+1);
		}
		else {
			face_list[i].sort_value = 0;
		}
	}
	
	// that should be done each time we go int or out water 
	qsort((void*)face_list, r_numfaces, sizeof(render_face_s), face_cmp);

	MarkLeafs(-1);

	unguard;
}


//
// Face comparator for qsort
//
int Q3BSP::face_cmp(const void *a, const void *b) {
	return ((render_face_s*)a)->sort_value-((render_face_s*)b)->sort_value;

}


//
// Report info of the bsp
//
void Q3BSP::Report(void){
	guard(Q3BSP::Report);

	// report info
	out->Insert("--- BSP Report:\n",r_numlightmaps);
	out->Insert("---  num lightmaps = %d\n",r_numlightmaps);
	out->Insert("---  num shaders = %d\n",r_numshaders);
	out->Insert("---  num planes = %d\n",r_numplanes);
	out->Insert("---  num nodes = %d\n",r_numnodes);
	out->Insert("---  num leafs = %d\n",r_numleafs);
	out->Insert("---  num lfaces = %d\n",r_numlfaces);
	out->Insert("---  num models = %d\n",r_nummodels);
	out->Insert("---  num verts = %d\n",r_numverts);
	out->Insert("---  num elems = %d\n",r_numelems);
	out->Insert("---  num faces = %d\n",r_numfaces);
	unguard
}






/*---------------------------------------------------------------------------
	Drawing:
---------------------------------------------------------------------------*/


//
// Render a new frame of the bsp
//
void Q3BSP::Update(void){
	guard(Q3BSP::Update);

	static int last_cluster=-1;
	int	vis_cluster;

	DWORD time_start=appCycles();

		// find camera cluster
		vis_cluster = r_leafs[FindLeaf(world->current_cam->c)].cluster;

	DWORD time_findleaf=appCycles();

		if(vis_cluster != last_cluster) {
			nodecount++;
			last_cluster=vis_cluster;
			MarkLeafs(vis_cluster);
		}

	DWORD time_propagate=appCycles();

		viscount++;
		WalkTree(0, PLB_ALL);

	DWORD time_culling=appCycles();

	world->find_leaf =	time_findleaf - time_start;
	world->mark_nodes=	time_propagate - time_findleaf;
	world->mark_faces=	time_culling - time_propagate;

	unguard;
}


void Q3BSP::Render(void){
	guard(Q3BSP::Render);

	DWORD time_startdraw=appCycles();

		DrawFaces();

	DWORD time_enddraw=appCycles();

	world->push_faces = time_enddraw - time_startdraw;

	unguard;
}



//
// Finds leaf of the current position
//
int Q3BSP::FindLeaf(vec3_t p){
	guard(Q3BSP::FindLeaf);
	cnode_t		*node;
	cplane_t		*plane;
	INT			idx, side;
	FLOAT		dot;

	idx=0;
	while(idx>=0){						// if idx <0 we have found a leaf.
		node=&r_nodes[idx];				
		plane=node->plane;

		if(plane->type<3)
			dot = p[plane->type] - plane->dist;
		else
			dot = DotProduct (p, plane->normal) - plane->dist;
	
		if (dot >= 0.0f) side = 0;
		else side = 1;

		idx=node->children[side];
	}

	return ~idx;		//return (-(idx+1));
	unguard;
}



//
// update leaf framenum according to the visibility
//
// TODO: write 8 leafs at once
void Q3BSP::MarkLeafs(int vis_cluster){
	guard(Q3BSP::MarkLeafs);
	cnode_t *node;

	for(int i=0; i<r_numleafs; i++){
		if(BSP_TESTVIS(vis_cluster, r_leafs[i].cluster) || vis_cluster==-1){
			r_leafs[i].framenum = nodecount;
			node = r_leafs[i].parent;
			while(node){
				if(node->framenum == nodecount) break;

				node->framenum = nodecount;
				node = node->parent;
			};
		}
	}
	unguard;
}




//
// Walk trough pv nodes inside the frustum
//
// TODO: we could iterate while we have only one visible node.
void Q3BSP::WalkTree(int idx, int planeflags){
	guard(Q3BSP::WalkTree);	
	if(idx>=0){
		if(r_nodes[idx].framenum!=nodecount) return;

		if(world->frustum.testBBox(r_nodes[idx].bbox, planeflags)){
			WalkTree(r_nodes[idx].children[0], planeflags);
			WalkTree(r_nodes[idx].children[1], planeflags);
		}
	}
	else {
		idx=~idx;
		if(r_leafs[idx].framenum!=nodecount) return;

		if(world->frustum.testBBox(r_leafs[idx].bbox, planeflags)){
			int *p = r_leafs[idx].firstface;
			for(int i=0; i<r_leafs[idx].numfaces; p++, i++){
				r_faces[*p].framenum=viscount;
			}
		}
	}
	unguard;
}




void Q3BSP::DrawFaces(void){
	guard(Q3BSP::DrawFaces);
	
	Surface *f;
	int last_shader=-1;
	int last_lightmap=-1;

	

	for(int i=0; i<r_numfaces; i++){
		f=face_list[i].face;
		
		if(!f || f->framenum != viscount) continue;
		
		Shader *s = world->shaders.GetShader(f->shader);
		if( f->shader!=last_shader || f->lm_texnum!=last_lightmap ){
			if ( s->surface_flags&SURF_SKY ) continue;
			render->SetRenderState(s, f->lm_texnum==~0?~0:r_lightmapinfos[f->lm_texnum].num);
			last_shader=f->shader;
			last_lightmap=f->lm_texnum;
		}

		if( f->facetype==FACETYPE_NORMAL ){
			if(s->cull==CULL_FRONT){
				if(DotProduct(f->v_norm, world->current_cam->c)-f->dist>0) continue;
			}
			else if(s->cull==CULL_BACK){
				if(DotProduct(f->v_norm, world->current_cam->c)-f->dist<0) continue;
			}
			render->PushTriangles(f);
		}
		else if(f->facetype==FACETYPE_PATCH) {
			if(world->frustum.testBBox(f->bbox)) 
				render->PushTriangles(f);
		}
		else if(f->facetype==FACETYPE_MESH){
			if(world->frustum.testBBox(f->bbox)) 
				render->PushTriangles(f);
		}
	}

	unguard;
}









/*---------------------------------------------------------------------------
	Data Loading Routines:
---------------------------------------------------------------------------*/


//
// Load Shaders
//
void Q3BSP::LoadShaders(void){
	guard(Q3BSP::LoadShaders);
	r_numshaders = ReadLump(SHADERREFS, (void**)&r_shaderrefs, sizeof(*r_shaderrefs));
	unguard;
}


//
// Build clip planes
//
void Q3BSP::LoadPlanes(void){
	guard(Q3BSP::LoadPlanes);
	cplane_t	*cplane;
	plane_t		*p, *planes;
	int			bits;

	r_numplanes = ReadLump(_PLANES, (void**)&planes, sizeof(plane_t));
	r_clipplanes = new cplane_t[r_numplanes];

	for(int i=0; i<r_numplanes; i++){
		cplane=&r_clipplanes[i];
		p=&planes[i];

		cplane->normal[0]=p->normal[0];
		cplane->normal[1]=p->normal[1];
		cplane->normal[2]=p->normal[2];
		cplane->dist=p->dist;
		
		if(p->normal[0]==1.0f) cplane->type=PLANE_X;
		else if(p->normal[1]==1.0f) cplane->type=PLANE_Y;
		else if(p->normal[2]==1.0f) cplane->type=PLANE_Z;
		else cplane->type=PLANE_NON_AXIAL;

		// for fast box on planeside test
		bits = 0;
		for (int j=0 ; j<3 ; j++) {
			if (p->normal[j] < 0) {
				bits |= 1<<j;
			}
		}
		cplane->signbits = bits;
	}

	free(planes); planes=NULL;
	unguard;
}


//
// Load the nodes
//
void Q3BSP::LoadNodes(void){
	guard(Q3BSP::LoadNodes);
	node_t *nodes;

	r_numnodes = ReadLump(NODES, (void**)&nodes, sizeof(node_t));
	
	r_nodes = new cnode_t[r_numnodes];

	for(int i=0; i<r_numnodes; i++){
		r_nodes[i].plane = r_clipplanes+nodes[i].plane;
		r_nodes[i].children[0] = nodes[i].children[0];
		r_nodes[i].children[1] = nodes[i].children[1];
		BoxCopy(nodes[i].bbox, r_nodes[i].bbox);
	}

	free(nodes); nodes=NULL;
	unguard;
}


//
// this are the indices of the leafs to the faces and the brushes:
//
// FIXME:	this way it is very easy to load the map, but there is a lot of indirection
//			we could avoid that by using non-fixed sized leaf structures but that hurts me.
void Q3BSP::LoadLFaces(void){
	guard(Q3BSP::LoadLFaces);
	r_numlfaces = ReadLump(LFACES, (void**)&r_lfaces, sizeof(*r_lfaces));
	unguard;
}
void Q3BSP::LoadLBrushes(void){
	guard(Q3BSP::LoadLBrushes);
	r_numlbrushes = ReadLump(LBRUSHES, (void**)&r_lbrushes, sizeof(*r_lbrushes));
	unguard;
}


//
// Load the leafs
//
void Q3BSP::LoadLeafs(void){
	guard(Q3BSP::LoadLeafs);
	leaf_t *leafs;
	
	r_numleafs = ReadLump(LEAFS, (void**)&leafs, sizeof(leaf_t));

	r_leafs = new cleaf_t[r_numleafs];

	for(int i=0; i<r_numleafs; i++){
		r_leafs[i].parent=NULL;
		r_leafs[i].cluster = leafs[i].cluster;
		r_leafs[i].area = leafs[i].area;
		BoxCopy(leafs[i].bbox, r_leafs[i].bbox);
		r_leafs[i].firstface = r_lfaces + leafs[i].firstface;
		r_leafs[i].numfaces = leafs[i].numfaces;
		r_leafs[i].firstbrush = r_lbrushes + leafs[i].firstbrush;
		r_leafs[i].numbrushes = leafs[i].numbrushes;
	}

	free(leafs); leafs=NULL;

	SetParent(0, NULL);
	unguard;
}


void Q3BSP::LoadModels(void){
	guard(Q3BSP::LoadModels);
	r_nummodels = ReadLump(MODELS, (void**)&r_models, sizeof(*r_models));
	unguard;
}
void Q3BSP::LoadBrushes(void){
	guard(Q3BSP::LoadBrushes);
	r_numbrushes = ReadLump(BRUSHES, (void**)&r_brushes, sizeof(*r_brushes));
	unguard;
}
void Q3BSP::LoadBrushSides(void){
	guard(Q3BSP::LoadBrushSides);
	r_numbrushsides = ReadLump(BRUSH_SIDES, (void**)&r_brushsides, sizeof(*r_brushsides));
	unguard;
}
void Q3BSP::LoadVerts(void){
	guard(Q3BSP::LoadVerts);
	r_numverts = ReadLump(VERTS, (void**)&r_verts, sizeof(*r_verts));
	unguard;
}
void Q3BSP::LoadElems(void){
	guard(Q3BSP::LoadElems);
	r_numelems = ReadLump(ELEMS, (void**)&r_elems, sizeof(*r_elems));
	unguard;
}
void Q3BSP::LoadFaces(void){
	guard(Q3BSP::LoadFaces);
	face_t *faces;

	r_numfaces = ReadLump(FACES, (void**)&faces, sizeof(face_t));
	r_numpatches=0;

	r_faces = new Surface[r_numfaces];
	face_list = new render_face_s[r_numfaces];

	for(int i=0; i<r_numfaces; i++){
		if(faces[i].facetype==FACETYPE_PLANAR) {
			r_faces[i].shader	= world->shaders.AddShader( r_shaderrefs[faces[i].shader].name, r_shaderrefs[faces[i].shader].surface_flags, faces[i].facetype );
			r_faces[i].facetype	= faces[i].facetype;
			r_faces[i].firstvert= r_verts + faces[i].firstvert;
			r_faces[i].numverts	= faces[i].numverts;
			r_faces[i].firstelem= r_elems + faces[i].firstelem;
			r_faces[i].numelems	= faces[i].numelems;
			r_faces[i].lm_texnum= faces[i].lm_texnum;
			r_faces[i].dist		= DotProduct(faces[i].v_orig, faces[i].v_norm);
			VectorCopy(faces[i].v_norm, r_faces[i].v_norm);
			VectorCopy(faces[i].v_orig, r_faces[i].v_orig);

			face_list[i].face = &r_faces[i];
		}
		else if(faces[i].facetype==FACETYPE_MESH) {
			r_faces[i].shader	= world->shaders.AddShader(	r_shaderrefs[faces[i].shader].name, r_shaderrefs[faces[i].shader].surface_flags, faces[i].facetype );
			r_faces[i].facetype	= faces[i].facetype;
			r_faces[i].firstvert= r_verts + faces[i].firstvert;
			r_faces[i].numverts	= faces[i].numverts;
			r_faces[i].firstelem= r_elems + faces[i].firstelem;
			r_faces[i].numelems	= faces[i].numelems;
			r_faces[i].lm_texnum= faces[i].lm_texnum;

			CalcFaceBounds(&r_faces[i]);

			face_list[i].face = &r_faces[i];
		}
		else if(faces[i].facetype==FACETYPE_PATCH) {
			r_faces[i].shader		= world->shaders.AddShader(	r_shaderrefs[faces[i].shader].name, r_shaderrefs[faces[i].shader].surface_flags, faces[i].facetype );
			r_faces[i].facetype		= faces[i].facetype;
			r_faces[i].firstvert	= r_verts + faces[i].firstvert;
			r_faces[i].numverts		= faces[i].numverts;
			r_faces[i].firstelem	= r_elems + faces[i].firstelem;
			r_faces[i].numelems		= faces[i].numelems;
			r_faces[i].lm_texnum	= faces[i].lm_texnum;

			r_faces[i].dist			= r_numpatches;		// HACK!
			r_numpatches++;

			BoxCopy(faces[i].bbox, r_faces[i].bbox);

			face_list[i].face = &r_faces[i];
		}
		else {
			face_list[i].face = NULL;
		}
	}

	r_patches = new BezierPatch[r_numpatches];
	for(i=0; i<r_numfaces; i++){
		if(faces[i].facetype==FACETYPE_PATCH) {
			r_patches[r_faces[i].dist].Build(&faces[i], &r_faces[i]);
		}
	}
	


	free(faces); faces=NULL;

	unguard;
}



//
// Load the lightmaps from the bsp
//
void Q3BSP::LoadLightmaps(void){
	guard(Q3BSP::LoadLightmaps);
	BYTE *lightmapdata;


	int lightmapsize = ReadLump(LIGHTMAPS, (void**)&lightmapdata, 1);
	r_numlightmaps = lightmapsize/BANK_SIZE;
	r_lightmapinfos = new texinfo[r_numlightmaps];

	

	//
	// lightmaps are allways overbrightened
	float overbright=2.0f;
	for(int i=0; i<r_numlightmaps; i++){
		r_lightmapinfos[i].mem=&lightmapdata[i * BANK_SIZE];
		r_lightmapinfos[i].bpp=3;
		r_lightmapinfos[i].height=128;
		r_lightmapinfos[i].width=128;
		r_lightmapinfos[i].num=~0;

		if (overbright != 1.0f){
			int j;
			BYTE *c;
			float r, g, b;

			c = &lightmapdata[i * BANK_SIZE];

			for (j=0; j < BANK_SIZE/3; j++, c+=3){
				float scale=1.0f;
				float tmp_scale;

				r = (float) c[0];
				g = (float) c[1];
				b = (float) c[2];
				r = r * overbright / 255.0f;
				g = g * overbright / 255.0f;
				b = b * overbright / 255.0f;
				
				if(r>1.0f && (tmp_scale=(1.0f/r))<scale) scale=tmp_scale;
				if(g>1.0f && (tmp_scale=(1.0f/g))<scale) scale=tmp_scale;
				if(b>1.0f && (tmp_scale=(1.0f/b))<scale) scale=tmp_scale;

				scale *= 255.0f;
				r *= scale;
				g *= scale;
				b *= scale;

				c[0] = appRound(r);
				c[1] = appRound(g);
				c[2] = appRound(b);
			}
		}
		
		render->RegisterTexInfo(&r_lightmapinfos[i], LF_LIGHTMAP|LF_NOMIPMAP|LF_NOPICMIP|LF_CLAMP);

		r_lightmapinfos[i].mem=NULL;
	}

	// We don't need this data any more 
	free(lightmapdata); lightmapdata=NULL;
	unguard;
}


void Q3BSP::LoadVisibility(void){
	guard(Q3BSP::LoadVisibility);
	ReadLump(VISIBILITY, (void**)&r_visibility, 1);
	unguard;
}





/*---------------------------------------------------------------------------
	Data Preprocessing Routines:
---------------------------------------------------------------------------*/


//
// Calculate the parent of each leaf and each node
//
void Q3BSP::SetParent(int node, cnode_t *parent){
	guard(Q3BSP::SetParent);
	if(node<0) {
		r_leafs[~node].parent = parent;
		return;
	}
	
	r_nodes[node].parent = parent;
	SetParent (r_nodes[node].children[0], &r_nodes[node]);
	SetParent (r_nodes[node].children[1], &r_nodes[node]);
	unguard;
}


//
// Calculate the bounding box of a mesh surface
//
// FIXME: there are some problems with this function.
void Q3BSP::CalcFaceBounds(Surface *f){
	float minx, miny, minz, maxx, maxy, maxz;

	minx=miny=minz=1000000000000000000000000.0f;		// FIXME: are this values right??
	maxx=maxy=maxz=-1000000000000000000000000.0f;

	vertex_t * v=f->firstvert;
	
	for(int i=0; i<f->numverts; i++, v++){
		minx=min(minx, v->v_point[0]);
		maxx=max(minx, v->v_point[0]);
		miny=min(miny, v->v_point[1]);
		maxy=max(maxy, v->v_point[1]);
		minz=min(minz, v->v_point[2]);
		maxz=max(maxz, v->v_point[2]);
	}
	f->bbox[0]=minx;
	f->bbox[1]=miny;
	f->bbox[2]=minz;
	f->bbox[3]=maxx;
	f->bbox[4]=maxy;
	f->bbox[5]=maxz;
};




