#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#define D_PROCESS_FAILED		(-1)
#define D_PROCESS_SUCCESS		(0)
#define D_PORT_NUM				(8765)


typedef struct recvBuf
{
	int socket[50];

}recvbuff;

int main(int argc, char **argv) {

	char btn[3];
	int socketFd;
	int connectFd;
	struct sockaddr_in sockaddr;
	char buff[1024];
	struct recvBuf *pRecvBuf;

	pRecvBuf = (struct recvBuf *)malloc( sizeof(struct recvBuf) );

	memset(buff,0,1024);

	socketFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socketFd == D_PROCESS_FAILED )
	{
		printf( "socket create failed : %s(errno:%d)\n",strerror(errno), errno );
		close(connectFd);
		close(socketFd);
		return D_PROCESS_FAILED;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockaddr.sin_port = htons(D_PORT_NUM);

	connectFd = connect(socketFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr) );
	if( connectFd == -1 )
	{
		printf( "connect create failed : %s(errno:%d)\n",strerror(errno), errno );
		close(connectFd);
		close(socketFd);
		return D_PROCESS_FAILED;
	}
	while(1)
	{
		printf("to server please input the words you want:\n");
		fgets(buff,1024,stdin);
		send(socketFd, buff, 1024, 0);
		memset(buff,0,1024);


	}

	close(connectFd);
	close(socketFd);

//	printf("socketfd = %d\n",socketFd);

//	int ret = send(socketFd, buff, 5, 0);
//	printf("ret = %d\n",ret);
//	printf( "send failed : %s(errno:%d)\n",strerror(errno), errno );
	return D_PROCESS_SUCCESS;
}

