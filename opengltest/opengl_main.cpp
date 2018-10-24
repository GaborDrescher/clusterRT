#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "painter.h"
#include "eventhandler.h"
#include "../precision.h"
#include "../rt.h"
#include "../stopwatch.h"

using namespace std;

int main(int argc, char **argv) {

	real clock_mult = real(0.05);

	int width = 1024;
	int height = 768;

	int beginLine = 0;
	int endLine = height;

	real clock = real(42.0);
	int inc = 1;

	if(argc == 5)
	{
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		endLine = height;
		clock = atof(argv[3]) * clock_mult;
		inc = atoi(argv[4]);
	}
	else if(argc == 7)
	{
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		clock = atof(argv[3]) * clock_mult;

		beginLine = atoi(argv[4]);
		endLine = atoi(argv[5]);
		inc = atoi(argv[6]);
	}
	
	unsigned char *outdata = new unsigned char[3 * width * (endLine - beginLine)];

	Painter painter(width, height);
	EventHandler ev;
	StopWatch watch;

	int i = 0;
	while(ev.handleUserInput())
	{
		watch.start();
		render(width, height, beginLine, endLine, clock, inc, outdata);
		real time = watch.stop();

		cout << "time for frame: " << time << endl;

		//real sleeptime = real(30) - time;

		//if(sleeptime > real(0))
		//{
		//	watch.sleep(sleeptime);
		//}

		painter.paintImageData(outdata);
		clock += real(0.04);

		//inc = ( sin(i*0.017) + real(1) ) * real(0.5) * real(32);
		//if(inc == 0) inc = 1;

		++i;
	}

	delete outdata;

	return EXIT_SUCCESS;
}
