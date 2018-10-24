#ifndef RT_STOPWATCH_H
#define RT_STOPWATCH_H

#include <time.h>
#include "precision.h"

class StopWatch
{
	private:
	struct timespec starttime;
	struct timespec stoptime;

	public:
	//saves current time
	void start()
	{
		clock_gettime(CLOCK_REALTIME, &starttime);
	}

	//return time in ms since last start()
	real stop()
	{
		clock_gettime(CLOCK_REALTIME, &stoptime);

		//do not use intermediate variables here
		real msec = (((real)(stoptime.tv_sec - starttime.tv_sec)) * real(1000))
					+ ( ((real)(stoptime.tv_nsec - starttime.tv_nsec)) / real(1000000) );
		
		return msec;
	}

	void sleep(real ms)
	{
		struct timespec time;
		time.tv_sec = ms / real(1000);
		time.tv_nsec = (ms - (time.tv_sec * 1000)) * 1000000;

		nanosleep(&time, 0);
	}
};

#endif
