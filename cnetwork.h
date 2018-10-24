#ifndef CNETWORK_H
#define CNETWORK_H

#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int connectToServer(const char *hostname, int port)
{
	struct hostent *host;
	struct sockaddr_in saddr;

	int sockFD;

	if((sockFD = socket(PF_INET, SOCK_STREAM, 0)) == -1) return -1;
	if((host = gethostbyname(hostname)) == NULL) return -1;

	memset(&saddr, 0, sizeof(saddr));
	memcpy(&saddr.sin_addr, host->h_addr, host->h_length);
	saddr.sin_family = AF_INET;
	saddr.sin_port   = htons(port);

	if(connect(sockFD, (struct sockaddr*)&saddr, sizeof(saddr))) return -1;

	return sockFD;
}

int acceptIncConnection(int serverFD)
{
	return accept(serverFD, NULL, 0);
}

int enableTCPServer(int port)
{
	#define QUEUE_LENGTH 64

	struct sockaddr_in name;
	int fd;
	int flag = 1;

	if((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) return -1;

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) return -1;

	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(fd, (const struct sockaddr *)&name, sizeof(name))) return -1;

	if(listen(fd, QUEUE_LENGTH)) return -1;

	return fd;
}

#endif
