/*===========================================================================
	Title: math.cpp
	Author: Ignacio Castaño Aguado
	Description: lot of math utilities

===========================================================================*/
#include "shared.h"



/*-----------------------------------------------------------------------------
	Globals:
-----------------------------------------------------------------------------*/

//vec3_t	vec3_origin = {0,0,0};






/*---------------------------------------------------------------------------
	Integer math
---------------------------------------------------------------------------*/

//
// Find the closest power of 2 that is >= N.
//
DWORD NextPowerOfTwo( DWORD N ) {
	if (N<=0L		) return 0L;
	if (N<=1L		) return 1L;
	if (N<=2L		) return 2L;
	if (N<=4L		) return 4L;
	if (N<=8L		) return 8L;
	if (N<=16L	    ) return 16L;
	if (N<=32L	    ) return 32L;
	if (N<=64L 	    ) return 64L;
	if (N<=128L     ) return 128L;
	if (N<=256L     ) return 256L;
	if (N<=512L     ) return 512L;
	if (N<=1024L    ) return 1024L;
	if (N<=2048L    ) return 2048L;
	if (N<=4096L    ) return 4096L;
	if (N<=8192L    ) return 8192L;
	if (N<=16384L   ) return 16384L;
	if (N<=32768L   ) return 32768L;
	if (N<=65536L   ) return 65536L;
	else			  return 0;
}


DWORD Log2(DWORD val){
	DWORD answer=0;
	while (val>>=1)
		answer++;
	return answer;
}


/* q2code:		(now using appRound and appFloor (of UT))
#pragma check_stack (off)
#pragma warning (disable:4035)
__declspec( naked ) 
long __cdecl q2ftol( float f ){
	static int tmp;
	__asm fld dword ptr [esp+4]
	__asm fistp tmp
	__asm mov eax, tmp
	__asm ret
}
#pragma warning (default:4035)
#pragma check_stack
*/



/*---------------------------------------------------------------------------
	Floating Point math
---------------------------------------------------------------------------*/

__declspec(naked) float __fastcall FastAbs(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fabs
		ret 4
	}
}

__declspec(naked) float __fastcall FastSin(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fsin
		ret 4
	}
}

__declspec(naked) float __fastcall FastCos(float a) {
	__asm {
		fld		DWORD PTR [esp+4] 
		fcos
		ret 4
	}
}




/*---------------------------------------------------------------------------
	Aproximations
---------------------------------------------------------------------------*/

//
// Reciprocal square root
//
float rSqrt( float number ){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;						// evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

__declspec(naked) float __fastcall InverseSqrt(float a) {
	__asm {
		mov		eax, 0be6eb508h
		mov		DWORD PTR [esp-12],03fc00000h ;  1.5 on the stack
		sub		eax, DWORD PTR [esp+4]; a
		sub		DWORD PTR [esp+4], 800000h ; a/2 a=Y0
		shr		eax, 1     ; firs approx in eax=R0
		mov		DWORD PTR [esp-8], eax

		fld		DWORD PTR [esp-8] ;r
		fmul	st, st            ;r*r
		fld		DWORD PTR [esp-8] ;r
		fxch	st(1)
		fmul	DWORD PTR [esp+4];a ;r*r*y0
		fld		DWORD PTR [esp-12];load 1.5
		fld		st(0)
		fsub	st,st(2)			   ;r1 = 1.5 - y1
		;x1 = st(3)
		;y1 = st(2)
		;1.5 = st(1)
		;r1 = st(0)

		fld		st(1)
		fxch	st(1)
		fmul	st(3),st			; y2=y1*r1*...
		fmul	st(3),st			; y2=y1*r1*r1
		fmulp	st(4),st            ; x2=x1*r1
		fsub	st,st(2)               ; r2=1.5-y2
		;x2=st(3)
		;y2=st(2)
		;1.5=st(1)
		;r2 = st(0)

		fmul	st(2),st			;y3=y2*r2*...
		fmul	st(3),st			;x3=x2*r2
		fmulp	st(2),st			;y3=y2*r2*r2
		fxch	st(1)
		fsubp	st(1),st			;r3= 1.5 - y3
		;x3 = st(1)
		;r3 = st(0)
		fmulp	st(1), st
		ret 4
	}
}





/*---------------------------------------------------------------------------
	Vectors
---------------------------------------------------------------------------*/

//
// optimized dot product
//
#if ASM
#pragma warning( disable: 4035 )
//__declspec( naked )
float __cdecl DotProduct(const vec3_t v1, const vec3_t v2) {
	FLOAT dotret;
	_asm {
		mov     ecx, v1
		mov     eax, v2

		;optimized dot product; 15 cycles
		fld dword ptr   [eax+0]     ;starts & ends on cycle 0
		fmul dword ptr  [ecx+0]     ;starts on cycle 1
		fld dword ptr   [eax+4]     ;starts & ends on cycle 2
		fmul dword ptr  [ecx+4]     ;starts on cycle 3
		fld dword ptr   [eax+8]     ;starts & ends on cycle 4
		fmul dword ptr  [ecx+8]     ;starts on cycle 5
		fxch            st(1)       ;no cost
		faddp           st(2),st(0) ;starts on cycle 6, stalls for cycles 7-8
		faddp           st(1),st(0) ;starts on cycle 9, stalls for cycles 10-12
		fstp dword ptr  [dotret]    ;starts on cycle 13, ends on cycle 14
		
//		ret
	}
	return dotret;
}
#pragma warning( default: 4035 )
#else
float __cdecl DotProduct(const vec3_t v1, const vec3_t v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}
#endif


//
// Vector and matrix multiplication
//
void MultVect3x3(float *A, float *v, float *dest) {
	dest[0]=DotProduct(&A[0], v);
	dest[1]=DotProduct(&A[3], v);
	dest[2]=DotProduct(&A[6], v);
}


//
// Fast normalization of 3 component vector (does not test if the vector has 0 length)
//
void FastNormVect3(float *v){
	float ilength;

	ilength = rSqrt( DotProduct( v, v ) );

	v[0] *= ilength;
	v[1] *= ilength;
	v[2] *= ilength;
}



//
// Fast normalization of 2 component vector (does not test if the vector has 0 length)
//
void FastNormVect2(float *v){
	float ilength;

	ilength = rSqrt( v[0]*v[0] + v[1]*v[1] );

	v[0] *= ilength;
	v[1] *= ilength;
}



//
// Slow normalization that returns the norm
//
float VectorNormalize(float *v){
	float length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = (float)sqrt (length);

	if ( length ) {
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}


//
// Cross Product
//
void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross ) {
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}


/*---------------------------------------------------------------------------
	Plane operations:
---------------------------------------------------------------------------*/


//
// Generate a plane given 3 points
//
// Returns false if the triangle is degenrate.
// The normal will point out of the clock for clockwise ordered points
bool PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c ) {
	vec3_t	d1, d2;

	VectorSub( b, a, d1 );
	VectorSub( c, a, d2 );
	CrossProduct( d2, d1, plane );
	if ( VectorNormalize( plane ) == 0 ) {
		return false;
	}

	plane[3] = DotProduct( a, plane );
	return true;
}



//
// returns the side of the plane in wich the box is
//
#if !ASM
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p) {
	float	dist1, dist2;
	int		sides;

	// fast axial cases
	if (p->type < 3) {
		if (p->dist <= emins[p->type])
			return 1;
		if (p->dist >= emaxs[p->type])
			return 2;
		return 3;
	}

	// general case
	switch (p->signbits) {
	case 0:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		break;
	}

	sides = 0;
	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}
#else
#pragma warning( disable: 4035 )
__declspec( naked ) int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p){
	static int bops_initialized;
	static int Ljmptab[8];

	__asm {

		push ebx
			
		cmp bops_initialized, 1
		je  initialized
		mov bops_initialized, 1
		
		mov Ljmptab[0*4], offset Lcase0
		mov Ljmptab[1*4], offset Lcase1
		mov Ljmptab[2*4], offset Lcase2
		mov Ljmptab[3*4], offset Lcase3
		mov Ljmptab[4*4], offset Lcase4
		mov Ljmptab[5*4], offset Lcase5
		mov Ljmptab[6*4], offset Lcase6
		mov Ljmptab[7*4], offset Lcase7
			
initialized:

		mov edx,dword ptr[4+12+esp]
		mov ecx,dword ptr[4+4+esp]
		xor eax,eax
		mov ebx,dword ptr[4+8+esp]
		mov al,byte ptr[17+edx]
		cmp al,8
		jge Lerror
		fld dword ptr[0+edx]
		fld st(0)
		jmp dword ptr[Ljmptab+eax*4]
Lcase0:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase1:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase2:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase3:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase4:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase5:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase6:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase7:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
LSetSides:
		faddp st(2),st(0)
		fcomp dword ptr[12+edx]
		xor ecx,ecx
		fnstsw ax
		fcomp dword ptr[12+edx]
		and ah,1
		xor ah,1
		add cl,ah
		fnstsw ax
		and ah,1
		add ah,ah
		add cl,ah
		pop ebx
		mov eax,ecx
		ret
Lerror:
		int 3
	}
}
#pragma warning( default: 4035 )
#endif




/*---------------------------------------------------------------------------
	Matrix Classes
---------------------------------------------------------------------------*/

//
// 3x3 matrix class
//
class Mat3x3 {
public:

	Mat3x3();
	Mat3x3(float*);
	Mat3x3(const Mat3x3& rhs);
	~Mat3x3();
	const Mat3x3 &operator=(const Mat3x3 &src);

	const float *GetData() const { return &data[0][0]; }
	float *GetData() { return &data[0][0]; }

	void  Set(int i1, int i2, float);
	float Get(int i1, int i2) const;


protected:
	float data[3][3];
};


//
// 4x4 matrix class
//
class Mat4x4 {
public:

	Mat4x4();
	Mat4x4(float*);
	Mat4x4(const Mat4x4& rhs);
	~Mat4x4();
	const Mat4x4 &operator=(const Mat4x4 &src);
	
	const float *GetData() const { return &data[0][0]; }
	float *GetData() { return &data[0][0]; }

	void  Set(int i1, int i2, float);
	float Get(int i1, int i2) const;


protected:
	float data[4][4];
};


//
// Mat3x2
//
// 3x3 matrix in homogeneus coordinates (used for texture transformations)

Mat3x2::Mat3x2(){
//	Identity();
}

Mat3x2::Mat3x2(float *p){
	data[0][0] = p[0];
	data[0][1] = p[1];
	data[1][0] = p[2];
	data[1][1] = p[3];
	data[2][0] = p[4];
	data[2][1] = p[5];
}

Mat3x2::Mat3x2(const Mat3x2& src){
	data[0][0] = src.data[0][0];
	data[0][1] = src.data[0][1];
	data[1][0] = src.data[1][0];
	data[1][1] = src.data[1][1];
	data[2][0] = src.data[2][0];
	data[2][1] = src.data[2][1];
}

Mat3x2::~Mat3x2(){
}

const Mat3x2 &Mat3x2::operator=(const Mat3x2 &src){
	Copy(src);
	return *this;
}
	
void Mat3x2::Set(int i1, int i2, float f){
//	assert(i1 >=0 && i1 < 3 && i2 >=0 && i2 < 3);
	data[i1][i2] = f;
}

float Mat3x2::Get(int i1, int i2) const{
//	assert(i1 >=0 && i1 < 3 && i2 >=0 && i2 < 3);
	return data[i1][i2];
}

void Mat3x2::Copy (const Mat3x2 &src){
	data[0][0] = src.data[0][0];
	data[0][1] = src.data[0][1];
	data[1][0] = src.data[1][0];
	data[1][1] = src.data[1][1];
	data[2][0] = src.data[2][0];
	data[2][1] = src.data[2][1];
}

void Mat3x2::Copy (const float *p){
	data[0][0] = p[0];
	data[0][1] = p[1];
	data[1][0] = p[2];
	data[1][1] = p[3];
	data[2][0] = p[4];
	data[2][1] = p[5];
}

void Mat3x2::Identity(){
	data[0][0] = 1.0f;
	data[0][1] = 0.0f;
	data[1][0] = 0.0f;
	data[1][1] = 1.0f;
	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
}

void Mat3x2::Zero(){
	data[0][0] = 0.0f;
	data[0][1] = 0.0f;
	data[1][0] = 0.0f;
	data[1][1] = 0.0f;
	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
}

void Mat3x2::Add(const Mat3x2 &a, const Mat3x2 &b){
	data[0][0] = a.data[0][0] + b.data[0][0];
	data[0][1] = a.data[0][1] + b.data[0][1];
	data[1][0] = a.data[1][0] + b.data[1][0];
	data[1][1] = a.data[1][1] + b.data[1][1];
	data[2][0] = a.data[2][0] + b.data[2][0];
	data[2][1] = a.data[2][1] + b.data[2][1];
}

void Mat3x2::Sub(const Mat3x2 &a, const Mat3x2 &b){
	data[0][0] = a.data[0][0] - b.data[0][0];
	data[0][1] = a.data[0][1] - b.data[0][1];
	data[1][0] = a.data[1][0] - b.data[1][0];
	data[1][1] = a.data[1][1] - b.data[1][1];
	data[2][0] = a.data[2][0] - b.data[2][0];
	data[2][1] = a.data[2][1] - b.data[2][1];
}

void Mat3x2::ScaleMatrix(const vec2_t v){
    data[0][0] = v[0];
    data[0][1] = 0.0f;
    data[1][0] = 0.0f;
    data[1][1] = v[1];
    data[2][0] = 0.0f;
    data[2][1] = 0.0f;
}

void Mat3x2::ScaleMatrix(float f){
    data[0][0] = f;
    data[0][1] = 0.0f;
    data[1][0] = 0.0f;
    data[1][1] = f;
    data[2][0] = 0.0f;
    data[2][1] = 0.0f;
}

void Mat3x2::TranslationMatrix(const vec2_t v){
    data[0][0] = 1.0f;
    data[0][1] = 0.0f;
    data[1][0] = 0.0f;
    data[1][1] = 1.0f;
    data[2][0] = v[0];
    data[2][1] = v[1];
}

void Mat3x2::RotationMatrix(float theta){
    float cost, sint;

    cost = FastCos(theta);
    sint = FastSin(theta);

	data[0][0] = cost;
	data[0][1] = -sint;
	data[1][0] = sint;
	data[1][1] = cost;
	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
}

void Mat3x2::Multiply(const Mat3x2 &a, const Mat3x2 &b){
	data[0][0] = a.data[0][0] * b.data[0][0] + a.data[0][1] * b.data[1][0];
	data[0][1] = a.data[0][0] * b.data[0][1] + a.data[0][1] * b.data[1][1];

	data[1][0] = a.data[1][0] * b.data[0][0] + a.data[1][1] * b.data[1][0];
	data[1][1] = a.data[1][0] * b.data[0][1] + a.data[1][1] * b.data[1][1];

	data[2][0] = a.data[2][0] + b.data[2][0];
	data[2][1] = a.data[2][1] + b.data[2][1];
}


//
// transform 2d point
//
void Mat3x2::TransformPoint(const vec2_t v, vec2_t dest){
//	dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + data[2][0];
//	dest[1] = v[0]*data[0][1] + v[1]*data[1][1] + data[2][1];

	__asm{
		mov	eax, DWORD PTR this
		mov	ebx, DWORD PTR v
		mov	ecx, DWORD PTR dest

		fld   DWORD PTR [ebx]
		fmul  DWORD PTR [eax+4]			// v[0] * data[0][1]
		fld   DWORD PTR [ebx]
		fmul  DWORD PTR [eax]			// v[0] * data[0][0]
		fld   DWORD PTR [ebx+4]
		fmul  DWORD PTR [eax+12]		// v[1] * data[1][1]
		fld   DWORD PTR [ebx+4]
		fmul  DWORD PTR [eax+8]			// v[1] * data[1][0]
		faddp ST(2), ST					// v[0] * data[1][0] + v[1] * data[0][0]
		faddp ST(2), ST					// v[0] * data[1][1] + v[1] * data[0][1]
		fadd  DWORD PTR [eax+16]		
		fstp  DWORD PTR [ecx]			// dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + data[2][0];
		fadd  DWORD PTR [eax+20]
		fstp  DWORD PTR [ecx+4]			// dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + data[2][0];
	}

}


//
// transform 2d point with turbulences
//
void Mat3x2::TransformPointX(const vec3_t v, vec2_t dest){
//	dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + FastSin(v[0])*data[2][0];
//	dest[1] = v[0]*data[0][1] + v[1]*data[1][1] + FastSin(v[1])*data[2][1];

	__asm{
		mov	eax, DWORD PTR this
		mov	ebx, DWORD PTR v
		mov	ecx, DWORD PTR dest

		//fld   DWORD PTR [ebx]				// x
		//fld   DWORD PTR [ebx+4]			// x y
		//fld	  st(0)						// x y y
		//fxch  st(2)						// y y x
		//fld	  st(0)						// y y x x

		fld   DWORD PTR [ebx]
		fmul  DWORD PTR [eax]			// v[0] * data[0][0]

		fld   DWORD PTR [ebx]
		fmul  DWORD PTR [eax+4]			// v[0] * data[0][1]

		fld   DWORD PTR [ebx+4]
		fmul  DWORD PTR [eax+8]			// v[1] * data[1][0]

		fld   DWORD PTR [ebx+4]
		fmul  DWORD PTR [eax+12]		// v[1] * data[1][1]

		fld   DWORD PTR [ebx]
		fsin
		fmul  DWORD PTR [eax+16]		// sin(v[0]) * data[2][0]

		fld   DWORD PTR [ebx+4]
		fsin
		fmul  DWORD PTR [eax+20]		// sin(v[0]) * data[2][1]

		faddp ST(2), ST
		faddp ST(2), ST
		faddp ST(2), ST
		faddp ST(2), ST
		fxch  ST(1)

		fstp  DWORD PTR [ecx]			// dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + data[2][0];
		fstp  DWORD PTR [ecx+4]			// dest[0] = v[0]*data[0][0] + v[1]*data[1][0] + data[2][0];
	}

}