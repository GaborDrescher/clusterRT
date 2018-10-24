#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "../cnetwork.h"
#include "../stopwatch.h"
#include "../opengltest/painter.h"
#include "../minilzo.h"

using namespace std;

int main(int argc, char **argv)
{
	if(argc != 3) return 1;

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);

	lzo_init();

	int serverFD = enableTCPServer(1338);

	int connFD = acceptIncConnection(serverFD);
	FILE *infile = fdopen(connFD, "r");

	const int buffer_size = width*height*3;
	unsigned char *buffer = new unsigned char[buffer_size];
	unsigned char *buffer_decomp = new unsigned char[buffer_size];

	StopWatch watch;
	Painter painter(width, height);

	cout << "\n";
	while(true)
	{
		watch.start();
		int size = 0;
		if(fread(&size, sizeof(int), 1, infile) <= 0) break;
		fread(buffer, size, 1, infile);
		real time = watch.stop();
		real mbps = (buffer_size / real(1024*1024)) / time*1000;
		cout << "time: " << time << "ms  @  " << mbps << "MB/s" << endl;
		cout << "compressed frame size (MB): " << (size/real(1024*1024)) << " uncompressed: " << ((width * height * 3)/real(1024*1024)) << " rate: " << size/(real)(width*height*3) << endl;

		lzo_uint decsize = 0;
		lzo1x_decompress(buffer, size, buffer_decomp, &decsize, 0);
		

		painter.paintImageData(buffer_decomp);
	}

	close(connFD);
	close(serverFD);

	return 0;
}
