#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>


#define D_PROCESS_FAILED		(-1)
#define D_PROCESS_SUCCESS		(0)
#define D_PORT_NUM				(8765)


typedef struct recvBuf
{
	int socket[50];

}recvbuff;

int main(int argc, char **argv) {

	int socketFd;
	int connectFd;
	struct sockaddr_in sockaddr;
	struct sockaddr_in addr;
	socklen_t addrLen;
	char buff[1024];
	int recvRet;
	int pipeFd[2];

	int rc;

	rc = pipe(pipeFd);

	int pid[5];
	int n = 0;

	struct recvBuf *pRecvBuf;

	memset(buff,0,1024);
	pRecvBuf = (struct recvBuf *)malloc( sizeof(struct recvBuf) );

	socketFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socketFd == D_PROCESS_FAILED )
	{
		printf( "socket create failed : %s(errno:%d)\n",strerror(errno), errno );
		return D_PROCESS_FAILED;
	}
	int on;
	int ret;
	on = 1;
	ret = setsockopt( socketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockaddr.sin_port = htons(D_PORT_NUM);

	if( (bind(socketFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) == -1 )
	{
		printf( "bind socket error: %s(errno: %d)\n",strerror(errno), errno );
		return D_PROCESS_FAILED;
	}

	if( listen(socketFd, 10 ) == -1 )
	{
		return -1;
	}
	int temp;
	int i = 0;
	printf("wait for connect from client...\n");
	while(1)
	{
		connectFd = accept( socketFd, (struct sockaddr*)&addr, &addrLen );
		if( connectFd == (-1) )
		{
			printf( "accept socket error: %s(errno: %d)\n",strerror(errno), errno );
			continue;
		}

		if(!fork())
		{
			printf("client:ip:%s\n",(char *)inet_ntoa(addr.sin_addr));
			while(1)
			{
				printf("wait for message from client %d\n...\n",i+1);
				recvRet = recv(connectFd, buff, 1024, 0);
				if( recvRet <= 0 )
				{
					printf("client %d :%s up\n",i+1,(char *)inet_ntoa(addr.sin_addr));
					close(pipeFd[0]);
					write(pipeFd[1],&i,4);
					break;
				}
				printf("from client %d:%s : %s\n",i+1,(char *)inet_ntoa(addr.sin_addr),buff);
				memset(buff,0,1024);
			}
		}
		i++;
		close(pipeFd[0]);
		read(pipeFd[1],&temp,4);
	}
	close(connectFd);
	close(socketFd);

	return D_PROCESS_SUCCESS;
}

