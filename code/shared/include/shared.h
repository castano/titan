/*============================================================================
	Title: shared.h
	Author: Ignacio Castaño Aguado
	Description: Main header of all the shader files. All the files of the
		project include this file.

============================================================================*/
#ifndef _SHARED_H_
#define _SHARED_H_



/*---------------------------------------------------------------------------
	Definitions
---------------------------------------------------------------------------*/

#define TIT_NOERROR		0
#define TIT_ERROR		1





/*-----------------------------------------------------------------------------
	Definitions:
-----------------------------------------------------------------------------*/

enum {
	VF_CHEAT=0x01,			// value can only be changed if cheating
	VF_SYSTEM=0x02,			// value can only be changed by the system
	VF_PERSISTENT=0x04,		// value is stored in default.cmd
	VF_LATCH=0x08,
	VF_USER=0x10,			// created by the user
	VF_TEMP=0x20,			// temporal variables are to be replaced with local varibles.
};


class Var {
public:

	Var(const char *name, int ivalue=0, int flags=0);
	Var(const char *name, float fvalue, int flags=0);
	Var(const char *name, const char *svalue, int flags=0);
	~Var(void);
	void SetString(const char *svalue);
	void SetInteger(int ivalue);
	void SetFloat(float fvalue);
	
	int operator= ( const int ivalue );
	float operator= ( const float fvalue );
	void operator= ( const char *svalue );


	char *	name;
	float	fvalue;
	int		ivalue;
	char *	svalue;
	int		flags;

	Var *	next;	// if next is here, the cost of recorrer the list is very hight, because
					// vars are stored in very different places of the memory.
};






/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>

#if WIN32
#if _MSC_VER
	#include <windows.h>
	#include "TDefs_VcWin32.h"
#else
	#error Unknown Compiler
#endif

#endif

#include "../../interfaces/framework.h"
#include "errorlog.h"

#include "alloc.h"
#include "TString.h"
#include "THashTable.h"
#include "TGrowArray.h"
#include "TBitarray.h"

#include "vars.h"
#include "file.h"
#include "cmd.h"
#include "timer.h"

#include "TParser.h"
#include "TSystem.h"

/*---------------------------------------------------------------------------
	Surface flags
---------------------------------------------------------------------------*/

#include "surfaceflags.h"		// shared beetween utilities and app





/*---------------------------------------------------------------------------
	Dinamic Linking
---------------------------------------------------------------------------*/

LIBRARY TLoadLibrary(const char *path);
void TUnloadLibrary(LIBRARY library);
bool TBindSymbol(void** ptr, LIBRARY library, const char* symbol);




/*---------------------------------------------------------------------------
	Math stuff
---------------------------------------------------------------------------*/

//
// Basic types
//
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];
typedef float texcoord_t[2];	// Texture s&t coordinates
typedef int bbox_t[6];			// Integer bounding box (mins, maxs)
typedef float bboxf_t[6];		// Float bounding box
//typedef BYTE colour_t[4];		// RGBA
typedef int colour_t;			// RGBA


//
// Definitions
//
#ifndef M_PI
#define M_PI		3.14159265358979323846		// matches value in gcc v2 math.h
#define M_TWO_PI	2*3.14159265358979323846
#endif

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( (a) * 180.0f ) / M_PI )


//
// Integer Math
//
DWORD NextPowerOfTwo( DWORD N );
DWORD Log2(DWORD val);


//
// vector encoding and decoding
//
#define NUMVERTEXNORMALS	216
extern	vec3_t	bytedirs[NUMVERTEXNORMALS];
int DirToByte( vec3_t dir );
void ByteToDir( int b, vec3_t dir );


//
// Floating Point Math
//
float __fastcall FastAbs(float a);
float __fastcall FastSin(float a);
float __fastcall FastCos(float a);


//
// Aproximations:
//
float rSqrt( float number );
#define Sqrt(n)		(n*rSqrt(n))
float __fastcall InverseSqrt(float a);
inline float FastSqrt(float a) { return a? a * InverseSqrt(a) : 0; }


//
// Vector Math
//

//	3d Vector macros:
#define VectorDot(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSub(a,b,c)		((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorFakeMod(v)		(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])
#define VectorMDot(a, b, c)		(a[0]*(b[0]-c[0])+a[1]*(b[1]-c[1])+a[2]*(b[2]-c[2]))

//	2d Vector macros:
#define TexcoordAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1])
#define	TexcoordScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s))
#define TexcoordCopy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1])

//	4d Vector macros:
#define ColorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2],(c)[3]=(a)[3]+(b)[3])
#define	ColorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s),(o)[3]=(v)[3]*(s))
#define ColorCopy(a,c)		((c)[0]=(a)[0],(c)[1]=(a)[1],(c)[2]=(a)[2],(c)[3]=(a)[3])

// BBox macros:
#define BoxCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3],(b)[4]=(a)[4],(b)[5]=(a)[5])

// vector functions:
float __cdecl DotProduct(const vec3_t v1, const vec3_t v2);
void FastNormVect2(float *v);
void FastNormVect3(float *v);
float VectorNormalize(float *v);
void MultVect3x3(float *A, float *v, float *dest);
void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross );

// plane functions
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p);


//
// matrix classes
//
// 3x3 matrix in homogeneus coordinates (used for texture transformations)
class Mat3x2 {
public:

	Mat3x2();
	Mat3x2(float*);
	Mat3x2(const Mat3x2& rhs);
	~Mat3x2();
	const Mat3x2 &operator=(const Mat3x2 &src);
	
	const float *GetData() const { return &data[0][0]; }
	float *GetData() { return &data[0][0]; }

	void  Set(int i1, int i2, float);
	float Get(int i1, int i2) const;

	void Copy (const Mat3x2 &src);
	void Copy (const float *);
	void Identity();
	void Zero();
	void Add(const Mat3x2 &m1, const Mat3x2 &m2);
	void Sub(const Mat3x2 &m1, const Mat3x2 &m2);
	void ScaleMatrix(const vec3_t);
	void ScaleMatrix(float);
	void TranslationMatrix(const vec2_t);
	void RotationMatrix(float theta);
	void Multiply(const Mat3x2&, const Mat3x2&);
	
	void TransformPoint(const vec2_t v, vec2_t dest);
	void TransformPointX(const vec3_t v, vec2_t dest);

protected:
	float data[3][2];
};







#endif	// _SHARED_H_