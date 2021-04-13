/*===========================================================================
	Title: Timer.h
	Author: Ignacio Castaño Aguado
	Description: Timer

===========================================================================*/
#ifndef _TIMER_INCLUDE
#define _TIMER_INCLUDE


/*-----------------------------------------------------------------------------
	Timer:
-----------------------------------------------------------------------------*/

class Timer {
public:
	Timer();
	~Timer();

	int Refresh();

	double			fTime;			// time since windows started
	double			flastTime;		// last recorded time
	double			frametime;		// time elapsed in the last frame
	int				frames;			// number of frames

	LARGE_INTEGER	tFrequency;
	double			tResolution;
	
private:
	int				lowshift;
	byte			tPerformanceTimerEnabled;

	unsigned int	oldTime;
	int				sametimecount;	// counter for frames with the same time.
};


#endif