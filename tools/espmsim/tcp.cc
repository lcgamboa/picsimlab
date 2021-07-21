#include"tcp.h"

#ifndef LINUX
WORD wVersionRequested = 2;
WSADATA wsaData;

const char* inet_ntop(int af, const void* src, char* dst, int cnt){
 
    struct sockaddr_in srcaddr;
 
    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));
 
    srcaddr.sin_family = af;
    if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        printf("WSAAddressToString() : %ld\n",rv);
        return NULL;
    }
    return dst;
}
#endif

void setnblock(int sock_descriptor)
{
#ifdef LINUX
  int flags; 
  /* Set socket to non-blocking */

  if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0)
  {
    /* Handle error */ 
    printf("Error fcntl!!!!!!!\n");
  }


  if (fcntl(sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
  {
    /* Handle error */ 
    printf("Error fcntl!!!!!!!\n");
  }
#else
        unsigned long iMode=1;
        ioctlsocket(sock_descriptor,FIONBIO,&iMode);
#endif
}

void setblock(int sock_descriptor)
{
#ifdef LINUX
  int flags;
  /* Set socket to blocking */

  if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0)
  {
    /* Handle error */
    printf("Error fcntl!!!!!!!\n");
  }


  if (fcntl(sock_descriptor, F_SETFL, flags & (~O_NONBLOCK)) < 0)
  {
    /* Handle error */
    printf("Error fcntl!!!!!!!\n");
  }
#else
   unsigned long iMode=0;
  ioctlsocket(sock_descriptor,FIONBIO,&iMode);
#endif
}




void skt_start(void)
{
#ifndef LINUX
  WSAStartup (wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
    {
      fprintf (stderr, "\n Wrong version\n");
      exit (-1);
    }
#endif
}

void skt_stop(void)
{
#ifndef LINUX
  WSACleanup ();
#endif
}

int skt_close(int skt)
{
  shutdown (skt,SHUT_RDWR );
  return close (skt);
}


int skt_recv(int sockfd,char* buff,int size)
{
  return recv (sockfd, buff, size, 0); 
}

int skt_send(int sockfd,const char* buff,int size)
{
  return send (sockfd, buff, size, MSG_NOSIGNAL);
}
        
int
skt_server (int port)	/* simple TCP server */
{
  struct sockaddr_in serv;//, cli;

  int listenfd;//, sockfd, n;
//  unsigned int clilen;



  if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf ("socket error : %s \n", strerror (errno));
      return -1;
    };
  int reuse = 1;
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

  memset (&serv, 0, sizeof (serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = htonl (INADDR_ANY);
  serv.sin_port = htons (port);


  if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)))
    {
      printf ("bind error : %s \n", strerror (errno));
      return -2;
    };

  if (listen (listenfd, SOMAXCONN))
    {
      printf ("listen error : %s \n", strerror (errno));
      return -3;
    };
    
    setnblock(listenfd);

    return listenfd;
};


int
skt_accept (int listenfd)
{
   struct sockaddr_in cli;

   int  sockfd;
#ifdef LINUX   
   unsigned int clilen;
#else
   int clilen;
#endif   
   
   clilen = sizeof (cli);
  
   if ((sockfd =accept (listenfd, (sockaddr *) & cli,  & clilen)) < 0)
        {
          //printf ("accept error : %s \n", strerror (errno));
          return -1;
        };
        
   setnblock(sockfd);
   
//===============================================
socklen_t len;
struct sockaddr_storage addr;
char ipstr[INET6_ADDRSTRLEN];
int port;

len = sizeof addr;
getpeername(sockfd, (struct sockaddr*)&addr, &len);

// deal with both IPv4 and IPv6:
if (addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
} else { // AF_INET6
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
}

printf("Peer IP address: %s\n", ipstr);
printf("Peer port      : %d\n", port);
printf("\n");
//===============================================

   return sockfd;     
}

int
skt_client (const char* endereco, int port)	/* simple TCP client */
{

  struct sockaddr_in serv;

  int sockfd;//, n;


  if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf ("socket error : %s \n", strerror (errno));
      exit (1);
    }
  memset (&serv, 0, sizeof (serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = inet_addr (endereco);
  serv.sin_port = htons (port);


  if (connect (sockfd, (sockaddr *) & serv, sizeof (serv)) < 0)
    {
      printf ("connect error : %s \n", strerror (errno));
      exit (1);
    };
    

    return 0;
}
