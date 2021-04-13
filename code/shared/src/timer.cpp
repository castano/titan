/*===========================================================================
	Title: timer.cpp
	Author: Ignacio Castaño Aguado
	Description: timer object
	FIXME: thats used only by the app, so it shouldnt be shared

===========================================================================*/
#include "shared.h"




/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern IOutputDevice *out;



/*---------------------------------------------------------------------------
	timer
---------------------------------------------------------------------------*/

//
// Constructor
//
Timer::Timer(){
	frames=0;
	frametime=0;
	fTime=0.0f;


	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &tFrequency)) {
		// no performance counter available, you might be using w98 osr1
		out->Insert("--- No Performance Counter available\n");
		tResolution = 0.001f;
		tPerformanceTimerEnabled = 0;
		//exit(-1);	// I usually exit in this situation
	}
	else { 
		// performance counter is available, use it instead of multimedia timer
		out->Insert("--- Performance Counter available\n");
		tPerformanceTimerEnabled = 1;

		// get 32 out of the 64 time bits such that we have around
		// 1 microsecond resolution
		unsigned int lowpart = (unsigned int)tFrequency.LowPart;
		unsigned int highpart = (unsigned int)tFrequency.HighPart;
		lowshift = 0;

		while (highpart || (lowpart > 2000000.0)){
			lowshift++;
			lowpart >>= 1;
			lowpart |= (highpart & 1) << 31;
			highpart >>= 1;
		}
		tResolution = 1.0/(double)lowpart;

	}
}



//
// Destructor
//
Timer::~Timer(){

}




//
// refresh the timers and the counters (called each frame)
//
int Timer::Refresh(){
	LARGE_INTEGER	count;
	unsigned int	temp, t2;


	if(tPerformanceTimerEnabled){
		
		QueryPerformanceCounter((LARGE_INTEGER *) &count);

		temp = ((unsigned int)count.LowPart >> lowshift) |
			   ((unsigned int)count.HighPart << (32 - lowshift));


		// check for firstframe, turnover or backward time
		if (frames==0 || ((temp <= oldTime) && ((oldTime - temp) < 0x10000000))){
			oldTime = temp;
		}
		else {
			t2 = temp - oldTime;
			frametime = (double)t2 * tResolution;
			fTime+=frametime;

			oldTime = temp;

			if (fTime == flastTime){
				sametimecount++;

				if (sametimecount > 100000){
					fTime += 1.0;
					sametimecount = 0;
				}
			}
			else sametimecount = 0;

			flastTime = fTime;
		}
		frames++;
	}
	else {
		if(frames==0){
			flastTime=fTime=timeGetTime()*tResolution;
			frametime=0;
		}else{
			flastTime=fTime;
			fTime = timeGetTime()*tResolution;
			frametime=fTime-flastTime;
		}
	}

	frames++;
	return frames;
}