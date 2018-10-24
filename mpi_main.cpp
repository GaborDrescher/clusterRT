#include <mpi.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "rt.h"
#include "precision.h"
#include "stopwatch.h"
#include "cnetwork.h"
#include "minilzo.h"

using namespace std;

static int width;
static int height;

static void encodeToFile(const char *file, int id, int w, int h, unsigned char *data)
{
	//flipping data
	//we rendered the piture upside down in opengl
	//so this is a workaround
	unsigned char *tmp = new unsigned char[w*h*3];
	for(int i = 0; i < h; ++i)
	{
		memcpy(tmp + w*3*i, data + w*3*((h-1)-i), w*3);
	}
	
	ostringstream strname;
	strname << file << id << ".ppm";
	string filename = strname.str();

	MPI_File mpifile;
	MPI_Status stat;

	MPI_File_open(MPI_COMM_SELF, ((char*)filename.c_str()), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &mpifile);

	ostringstream str;
	str << "P6\n" << w << " " << h << "\n255\n";
	string s = str.str();

	MPI_File_write(mpifile, ((char*)s.c_str()), s.length(), MPI_BYTE, &stat);
	MPI_File_write(mpifile, tmp, w*h*3, MPI_BYTE, &stat);

	MPI_File_close(&mpifile);

	delete tmp;
}

static void encodeToTCP(int fd, int size, unsigned char *data)
{
	write(fd, &size, sizeof(int));
	write(fd, data, size);
}

static int calcNewInc(real time, int old_inc)
{
	const int MAXINC = 32;
	const real min = 30;
	const real max = 35;

	int newinc = old_inc;

	if(time < min)
	{
		newinc = old_inc - 1;
	}
	else if(time > max)
	{
		newinc = old_inc + 1;
	}

	if(newinc < 1) newinc = 1;
	else if(newinc > MAXINC) newinc = MAXINC;

	return newinc;
}

static void master(int frames, int nnodes)
{
	//cout << "master\n";

	int buffer_size = width * height * 3;

	int step = height / nnodes;

	unsigned char *buffer_old = new unsigned char[buffer_size];
	unsigned char *buffer_new = new unsigned char[buffer_size];

	unsigned char *compr_buffer = new unsigned char[buffer_size];
	long lzowrk[LZO1X_1_MEM_COMPRESS];

	memset(buffer_old, 0, buffer_size);//first frame is black

	MPI_Request *requests = new MPI_Request[nnodes];
	MPI_Request *send_requests = new MPI_Request[nnodes];
	MPI_Status *stats = new MPI_Status[nnodes];

	StopWatch watch;
	int inc = 32;

	int connectFD = connectToServer("faui02a.informatik.uni-erlangen.de", 1338);
	//int connectFD = connectToServer("localhost", 1338);

	lzo_init();
	
	for(int count = 0; count < frames; ++count)
	{
		for(int i = 0; i < nnodes; ++i)
		{
			int recv_bytes = step * width * 3;
			if(i == (nnodes - 1)) recv_bytes = (step + (height % nnodes)) * width * 3;
			
			MPI_Irecv(buffer_new + (step * width*3) * i, recv_bytes, MPI_BYTE, i+1, MPI_ANY_TAG, MPI_COMM_WORLD, requests + i);

			//async. send new resolution
			int sendinc = inc;
			if(count == frames - 1) sendinc = -1;
			MPI_Isend(&sendinc, 1, MPI_INTEGER, i+1, 0, MPI_COMM_WORLD, send_requests + i);
		}

		watch.start();
		MPI_Barrier(MPI_COMM_WORLD);

		//compress buffer_old to compr_buffer
		//encode compr_buffer
		lzo_uint compr_len = 0;

		lzo1x_1_compress(buffer_old, buffer_size, compr_buffer, &compr_len, lzowrk);
		encodeToTCP(connectFD, compr_len, compr_buffer);
		//encodeToFile("image", count, width, height, buffer_old);
		
		if(count == frames - 1)
		{
			close(connectFD);
			break;
		}

		//wait for all receives to finish
		MPI_Waitall(nnodes, requests, stats);

		//take time that rendering and encoding took and calculate new resolution
		real time = watch.stop();

		real sleeptime = real(35) - time;

		if(sleeptime > real(0))
		{
			watch.sleep(sleeptime);
		}

		//cout << "time: " << time << endl;
		inc = calcNewInc(time, inc);

		//new data is ready now
		//swap buffers
		unsigned char *tmp = buffer_old;
		buffer_old = buffer_new;
		buffer_new = tmp;
	}

	delete [] buffer_old;
	delete [] buffer_new;
	delete [] compr_buffer;
	delete [] requests;
	delete [] send_requests;
	delete [] stats;
}

static void worker(int id, int nnodes)
{
	//cout << "worker " << id << endl;

	int step = height/nnodes;
	int start = id * step;
	int end = start + step;

	if(id == (nnodes - 1)) end = height;

	int buffer_size = 3 * width * (end - start);

	unsigned char *outdata = new unsigned char[buffer_size];

	real clock(0);

	int inc = 1;

	MPI_Status status;

	while(true)
	{
		MPI_Barrier(MPI_COMM_WORLD);

		//receive next resolution and set inc
		MPI_Recv(&inc, 1, MPI_INTEGER, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		//if(id == 0) cout << inc << endl;

		if(inc == -1) break;

		render(width, height, start, end, clock, inc, outdata);

		MPI_Ssend(outdata, buffer_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

		clock += real(0.04);
	}

	delete [] outdata;
}

int main(int argc, char **argv)
{
	width = 800;
	height = 600;
	int frames = 2500;

	if(argc == 4)
	{
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		frames = atoi(argv[3]);
	}

	int id = -1;
	int nnodes = -1;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nnodes);

	//cout << "processes started: " << nnodes << endl;

	if(nnodes <= 1)
	{
		cerr << "specify at least 2 nodes" << endl;
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	if(id == 0)
	{
		master(frames, nnodes-1);
	}
	else
	{
		worker(id-1, nnodes-1);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
