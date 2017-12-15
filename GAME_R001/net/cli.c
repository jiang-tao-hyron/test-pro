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

#define SERVPORT 48800  

#define MAXSLEEP 128  

#define BUFLEN 128  

#define DATALEN 200  

int connect_retry(int domain, int type, int protocol,
		const struct sockaddr *addr)

{

	int numsec, fd;

	//try to connect with exponential backoff  

	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1)

	{

		if ((fd = socket(domain, type, protocol)) < 0)

			return -1;

		if (connect(fd, addr, sizeof(struct sockaddr)) == 0)

		{

			return fd;

		}

		close(fd);

		//delay before trying again  

		if (numsec <= MAXSLEEP / 2)

			sleep(numsec);

	}

	return -1;

}

void *sendmessage(void *arg)

{

	int sockfd;

	char buf[BUFLEN];

	sockfd = *(int *) arg;

	memset(buf, 0, BUFLEN);
	printf("the message you want :\n");
	while (fgets(buf, BUFLEN, stdin) != NULL )

	{

		send(sockfd, buf, strlen(buf), 0);

		memset(buf, 0, BUFLEN);
		printf("the message you want :\n");

	}
	return NULL;
}

void communication(int sockfd)

{

	int n;

	pid_t pid;

	char buf[BUFLEN];

	char *recvtitle = "received from server: ";

	int fd;

	int err;

	pthread_t tid;

	memset(buf, 0, BUFLEN);

	//create thread to send message  

	if ((err = pthread_create(&tid, NULL, sendmessage, &sockfd)) == -1)

	{

		printf("pthread_create error\n");

		exit(0);

	}

	//receive message  

	while (1)

	{

		while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)

		{

			write(STDOUT_FILENO, recvtitle, strlen(recvtitle));

			write(STDOUT_FILENO, buf, n);

			memset(buf, 0, BUFLEN);

		}

		if (n < 0)

			printf("recv error\n");

	}

}

int main(int argc, char *argv[])

{
	
	int sockfd, err;

	struct sockaddr_in serversockaddr;

	const char *addr;

	char abuf[INET_ADDRSTRLEN];

	struct hostent *host;
//
//	if (argc != 2)
//
//	{
//
//		printf("usage: %s hostname\n", argv[0]);
//
//		exit(0);
//
//	}

	if ((host = gethostbyname("172.28.12.69")) == NULL )

	{

		perror("gethostbyname");

		exit(0);

	}

	serversockaddr.sin_family = AF_INET;

	serversockaddr.sin_port = htons(SERVPORT);

	serversockaddr.sin_addr = *((struct in_addr *) host->h_addr);

	bzero(&(serversockaddr.sin_zero), 0);

	if ((sockfd = connect_retry(serversockaddr.sin_family, SOCK_STREAM, 0,
			(struct sockaddr *) &serversockaddr)) < 0)

	{

		err = errno;

	}

	else

	{

		communication(sockfd);

		exit(0);

	}

	printf("can't connect to %s\n", "172.28.12.45");

	exit(0);

}
