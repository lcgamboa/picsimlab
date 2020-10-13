//system headers dependent


#ifdef LINUX
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/fcntl.h>
#include<netdb.h>
#define        min(a,b)        ((a) < (b) ? (a) : (b))
#else
#include<winsock2.h>
#include<ws2tcpip.h>
#define SHUT_RDWR SD_BOTH 
#define MSG_NOSIGNAL 0
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

//defines
#define        REQUEST      1024
#define        REPLY        1024
typedef struct sockaddr sockaddr;

void skt_start(void);

void skt_stop(void);


int skt_server (int port);
int skt_accept (int listenfd);
int skt_recv(int sockfd,char* buff,int size);
int skt_send(int sockfd,const char* buff,int size);
int skt_client (const char* endereco, int port);
int skt_close(int skt);


