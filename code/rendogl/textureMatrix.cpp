/*===========================================================================
	Title: textureMatrix.h
	Author: Ignacio Castaño Aguado
	Description: texture matrix implementation
===========================================================================*/
#include "render.h"



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern Framework Import;




/*---------------------------------------------------------------------------
	textureMatrix:
---------------------------------------------------------------------------*/

void TextureMatrix::Apply(float *v, float *st, int num){
	vec2_t cpy;

	if(!turb){
		for(int i=0; i<num; i++, st+=2){
			cpy[0] = st[0]-0.5f;
			cpy[1] = st[1]-0.5f;

			matrix.TransformPoint(cpy, st);

			st[0]+=0.5f;
			st[1]+=0.5f;
		}
	}
	else {
		for(int i=0; i<num; i++, st+=2, v+=4){
			cpy[0]=st[0]-0.5f+FastSin((v[0]+v[2])*0.1*speed+Import.timer->fTime+phase)*amplitude;
			cpy[1]=st[1]-0.5f+FastSin((v[1])*0.1*speed+Import.timer->fTime+phase)*amplitude;
			//cpy[0]=st[0]-0.5f+amplitude*FastSin((v[0]+v[2])+phase);
			//cpy[1]=st[1]-0.5f+amplitude*FastSin((v[1])+phase);


			matrix.TransformPoint(cpy, st);

			st[0]+=0.5f;
			st[1]+=0.5f;

			 
			

		}
	}
}

void TextureMatrix::Build(funclist *tcmod){
	Mat3x2 trans;
	Mat3x2 copy;

	matrix.Identity();
	turb=false;

	for(funclist *f=tcmod; f; f=f->next){
		switch(f->func){
			case TCMOD_ROTATE:{
				float angulo=f->p[0]*Import.timer->fTime/180*3.14159;
				trans.RotationMatrix(angulo);
			}break;
			case TCMOD_SCALE:
				trans.ScaleMatrix(f->p);
			break;
			case TCMOD_SCROLL:{
				vec2_t scroll;
				scroll[0] = f->p[0]*Import.timer->fTime;
				scroll[1] = f->p[1]*Import.timer->fTime;
				scroll[0] -= floor(scroll[0]);
				scroll[1] -= floor(scroll[1]);

				trans.TranslationMatrix(scroll);
			}break;
			case TCMOD_STRETCH:{
				float factor=1/State::Eval(f->p);
				trans.ScaleMatrix(factor);
			}break;
			case TCMOD_TRANSFORM:
				trans.Copy(f->p);
			break;
			case TCMOD_TURB:{
				//phase=(Import.timer->fTime+f->p[2])*f->p[3];
				turb=true;
				amplitude=f->p[1];
				phase=f->p[2];
				speed=f->p[3];
				continue;
			}break;
		}

		copy.Copy(matrix);
		matrix.Multiply(trans, copy);
	}
}
