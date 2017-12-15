#include<stdio.h>  

#include <stdlib.h>   

#include <errno.h>   

#include <string.h>   

#include <netdb.h>   

#include <sys/types.h>   

#include <netinet/in.h>   

#include <sys/socket.h>  

#include <unistd.h>  

#include <arpa/inet.h>  

#include <fcntl.h>  

#include <pthread.h>  

#define BUFLEN 128  

#define QLEN   10  

#define DATALEN 200  

#define SERVPORT 48800  

#define HOST_NAME_MAX 256  
#define IPLEN  16  

//store accept id of client connect to server  

int acceptfd[QLEN];
//number of client connect to server   

int acceptnum = 0;

//information from main thread to created thread   
struct threadinfo
{
	int clfd;
	char ipaddr[IPLEN];
	int port;
};

void sys_err(char *errinfo)

{

	if (NULL == errinfo)

	{

		return;

	}

	perror("errinfo");

	exit(0);

}

int initsrver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)

{

	int fd;

	int err = 0;

	int reuse = -1;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)

	{

		return -1;

	}
	//set socket option  

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)

	{
		perror("setsocketopt failed!\n");
		exit(1);
	}

	if (bind(fd, addr, alen) < 0)
	{

		perror("bind failed!\n");
		exit(1);
	}

	if (type == SOCK_STREAM || type == SOCK_SEQPACKET)

	{
		//listen  

		if (listen(fd, qlen) < 0)

		{
			perror("listen failed!\n");
			exit(1);
		}

	}

	return fd;

}

void *recvmessage(void *arg)

{

	char buf[BUFLEN];

	char recvtitle[DATALEN] = "receive from ";

	int n;
	struct threadinfo *ti = arg;

	memset(buf, 0, BUFLEN);

	sprintf(recvtitle, "%s%s:%d ", recvtitle, ti->ipaddr, ti->port);

	while ((n = recv(ti->clfd, buf, BUFLEN, 0)) > 0)

	{
		printf("%s\n",buf);
		write(STDOUT_FILENO, recvtitle, strlen(recvtitle));

		write(STDOUT_FILENO, buf, n);

		memset(buf, 0, BUFLEN);

	}

	if (n < 0)

		printf("recv error\n");

	return NULL;
}

void *acceptThread(void *sockfd)

{

	int clfd;

	const char *addr;

	char buf[BUFLEN];

	int n;

	char abuf[INET_ADDRSTRLEN];

	struct sockaddr_in clientsockaddr;

	int clientlen;

	int err;

	pthread_t recvtid;
	struct threadinfo ti;

	clientlen = sizeof(clientsockaddr);
	//always accept new client to accept a lot of client   

	while (1)

	{

		if ((clfd = accept(*(int *) sockfd, (struct sockaddr *) &clientsockaddr,
				&clientlen)) < 0)

			sys_err("accept error");

		printf("%s:%d login in to server\n", inet_ntoa(clientsockaddr.sin_addr),
				clientsockaddr.sin_port);

		//when accept client, store it's accept id  

		acceptfd[acceptnum++] = clfd;

		//create thread to receive message of every client  
		ti.clfd = clfd;
		strcpy(ti.ipaddr, inet_ntoa(clientsockaddr.sin_addr));
		ti.port = clientsockaddr.sin_port;

		if ((err = pthread_create(&recvtid, NULL, recvmessage, &ti)) != 0)

			sys_err("pthread_create recvmessage error");

	}
	return NULL;
}

int communication(int sockfd)

{

	int clfd;

	pid_t pid;

	char buf[BUFLEN];

	int fd;

	int err;

	pthread_t acpttid;

	int i;

	memset(buf, 0, BUFLEN);

	//create thread to accept client, always accept to accept a lot of client  

	if ((err = pthread_create(&acpttid, NULL, acceptThread, &sockfd)) != 0)

		sys_err("pthread_create acceptThread error");

	//send message to client, but only send the same mesage to all connected client now  

	memset(buf, 0, BUFLEN);

	while (1)

	{

		if (fgets(buf, BUFLEN, stdin) != NULL )

		{
			//print the num of accept client connect to server  

			printf("acceptnum: %d\n", acceptnum);

			for (i = 0; i < acceptnum; ++i)

			{

				send(acceptfd[i], buf, strlen(buf), 0);

			}

			memset(buf, 0, BUFLEN);

		}

	}

	close(clfd);
	return 0;

}

int main(int argc, char *argv[])

{

	struct sockaddr_in serversockaddr, clientsockaddr;

	int sockfd, clientfd;

	int err, n;

	if (argc != 1)

	{

		printf("usage: %s\n", argv[0]);

		exit(0);

	}

	serversockaddr.sin_family = AF_INET;

	serversockaddr.sin_port = htons(SERVPORT);

	serversockaddr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(serversockaddr.sin_zero), 8);
	printf("wait for connection :\n");
	if ((sockfd = initsrver(SOCK_STREAM, (struct sockaddr *) &serversockaddr,
			sizeof(struct sockaddr), QLEN)) > 0)

	{
		//start communication  

		communication(sockfd);

		exit(0);

	}

	exit(0);

}
