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

#define BUFLEN 3  

#define DATALEN 200  

#include "chess.h"
char table[8][8]={
	{1,2,3,5,4,3,2,1},
	{6,6,6,6,6,6,6,6},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{13,13,13,13,13,13,13,13},
	{8,9,10,11,12,10,9,8},
};
unsigned char bg[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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
	int count=1;
	char buf[BUFLEN];
	char buf2[BUFLEN*2];

	sockfd = *(int *) arg;

	memset(buf, 0, BUFLEN);
	memset(buf2, 0, BUFLEN*2);
	system("clear");
	show2(buf2,buf);
	printf("the message you want :\n");
	
	while (fgets(buf, BUFLEN, stdin) != NULL )

	{
		if(count%2){
			if (getkey1(buf) == 0) {
				memset(buf, 0, BUFLEN);
				continue;
			}
			system("clear");
			show1(buf);
			count++;
			strcpy(buf2,buf);
		}
		else{
			if (getkey2(buf) == 0) {
				memset(buf, 0, BUFLEN);
				continue;
			}
			walk(buf2,buf);
			system("clear");
			show2(buf2,buf);
			count++;
			strcat(buf2,buf);
			send(sockfd, buf2, strlen(buf2), 0);
			memset(buf2, 0, BUFLEN*2);
		}
		memset(buf, 0, BUFLEN);
		printf("the message you want :\n");

	}
	
	return NULL;
}

void communication(int sockfd)

{

	int n;

	pid_t pid;

	char buf[BUFLEN*2];

	char *recvtitle = "received from server: ";

	int fd;

	int err;

	pthread_t tid;

	memset(buf, 0, BUFLEN*2);

	//create thread to send message  

	if ((err = pthread_create(&tid, NULL, sendmessage, &sockfd)) == -1)

	{

		printf("pthread_create error\n");

		exit(0);

	}

	//receive message  

	while (1)

	{

		while ((n = recv(sockfd, buf, BUFLEN*2, 0)) > 0)

		{
			system("clear");
			buf[0]='H'-buf[0]+'A';
			buf[1]='8'-buf[1]+'1';
			buf[2]='H'-buf[2]+'A';
			buf[3]='8'-buf[3]+'1';
			walk(buf,buf+2);
			show2(buf,buf+2);
			write(STDOUT_FILENO, recvtitle, strlen(recvtitle));
			write(STDOUT_FILENO, buf, n);
			printf("\n");

			/******* ser  recv  buf ********/
			memset(buf, 0, BUFLEN*2);

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
	
	char ID[16]={0};
	printf("connect to server:");
	scanf("%s",ID);
	if ((host = gethostbyname(ID)) == NULL )

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
