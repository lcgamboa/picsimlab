/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gamboa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include <netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fcntl.h>
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
#include <math.h>

#include"serial.h"
#include"tests.h"

static int sockfd = -1;
static char buff[2048];
//static char cmd[256];

//static void setblock(int sock_descriptor);
static void
setnblock(int sock_descriptor);

typedef struct
{
 char name[30];
 test_run_func trun;
 void * arg;
 int result;
} test_desc;

static test_desc tests_list[MAX_TESTS];

static int NUM_TESTS = 0;

static cserial serial;

static char pexe[256];

int
main(int argc, char ** argv)
{

 if (argc != 3)
  {
   printf ("use: %s picsimlab_executable serial_port\n", argv[0]);
   return -1;
  }


 if (!test_file_exist (argv[1]))
  {
   printf ("Picsimlab executable \"%s\" not found! \n", argv[1]);
   return -1;
  }


 if (!serial.Open (argv[2]))
  {
   printf ("Serial port \"%s\" can't be open! \n", argv[2]);
   return -1;
  }

 strncpy (pexe, argv[1], 255);

 serial.Config (9600);

 for (int i = 0; i < NUM_TESTS; i++)
  {
   printf ("======== test: %-20s ==============\n", tests_list[i].name);
   tests_list[i].result = tests_list[i].trun (tests_list[i].arg);
  }

 printf ("\n\n======== Results ==============\n");
 for (int i = 0; i < NUM_TESTS; i++)
  {
   printf ("test: %-20s : %s\n", tests_list[i].name, (tests_list[i].result ? "\033[1;32m Success\033[0m" : "\033[1;31m Fail\033[0m"));
  }

 serial.Close ();
 return 0;
}

void
test_register(const char * name, test_run_func trun, void* arg)
{

 strncpy (tests_list[NUM_TESTS].name, name, 20);
 tests_list[NUM_TESTS].trun = trun;
 tests_list[NUM_TESTS].arg = arg;
 tests_list[NUM_TESTS].result = 0;

 NUM_TESTS++;
}

int
test_load(const char * fname)
{
 struct sockaddr_in servaddr;
 char cmd[512];


 if (!test_file_exist (fname))
  {
   printf ("File not found %s\n", fname);
   return 0;
  }

 if (strstr (pexe, ".exe"))
  {
   sprintf (cmd, "wine %s %s &", pexe, fname);
   system (cmd);
   sleep (10); //wait
  }
 else
  {
   sprintf (cmd, "%s %s &", pexe, fname);
   system (cmd);
   sleep (1); //wait
  }


 if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
   printf ("socket error : %s \n", strerror (errno));
   exit (1);
  }
 memset (&servaddr, 0, sizeof (servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
 servaddr.sin_port = htons (5000);


 if (connect (sockfd, (struct sockaddr *) & servaddr, sizeof (servaddr)) < 0)
  {
#ifdef _WIN_
   printf ("connect error number: %i \n", WSAGetLastError ());
#else         
   printf ("connect error : %s \n", strerror (errno));
#endif
   close (sockfd);
   exit (1);
  }
 else
  {
   recv (sockfd, buff, 200, 0);
   printf ("%s", buff);

   setnblock (sockfd);
   test_send_rcmd ("reset");
   sleep (2); //bypass uno bootloader
  }

 return 1;
}

int
test_end()
{
 test_send_rcmd ("exit");
 close (sockfd);
 sleep (1);
 return 1;
}

int
test_serial_send(const char data)
{
 return serial.Send (data);
}

int
test_serial_recv(char *data)
{
 return serial.Receive ((unsigned char *) data);
}

int
test_serial_recv_wait(char * data, const int timeout)
{
 int ret = 0;
 int t = 0;
 do
  {
   ret = serial.Receive ((unsigned char *) data);
   usleep (1000);
   t++;
  }
 while ((!ret) && (t < timeout));

 return ret;
}

int
test_serial_recv_str(char * data, const int size, const int timeout)
{
 int i = 0;

 int ret = 0;


 while ((ret = test_serial_recv_wait (&data[i], timeout)))
  {

   if (data[i] == '\n')
    {
     data[i] = 0;
     break;
    }

   i++;
   if (i >= size)
    {

     data[size - 1] = 0;
     break;
    }

  }


 return ret;
}


//rcontrol 

int
test_send_rcmd(const char * message)
{
 strcpy (buff, message);
 strcat (buff, "\r\n");
 //printf ("sending '%s'\n", message);
 int n = strlen (buff);
 if (send (sockfd, buff, n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   close (sockfd);
   exit (-1);
  }

 int bp = 0;
 buff[0] = 0;
 do
  {

   if ((n = recv (sockfd, buff + bp, 1, 0)) > 0)
    {
     bp += n;
     buff[bp] = 0;
     //printf ("%c", buff[bp-1]);
    }
   else
    {
     if (n < 0)
      {
#ifndef _WIN32         
       if (errno != EAGAIN)
#else
       if (WSAGetLastError () != WSAEWOULDBLOCK)
#endif   
        {
         printf ("send error : %s \n", strerror (errno));
         close (sockfd);
         exit (-1);
        }
      }
    }

  }

 while (((bp < 5) || (strcmp (&buff[bp - 5], "Ok\r\n>"))) && ((bp < 8) || strcmp (&buff[bp - 8], "ERROR\r\n>")));

 return bp;
}

char *
test_get_cmd_resp(void)
{

 return buff;
}

#ifdef _WIN32
WORD wVersionRequested = 2;
WSADATA wsaData;

__attribute__ ((constructor))
static void
initialize_socket(void)
{
 WSAStartup (wVersionRequested, &wsaData);
 if (wsaData.wVersion != wVersionRequested)
  {

   fprintf (stderr, "\n Wrong version\n");
   return;
  }
}

__attribute__ ((destructor))
static void
finalize_socket(void)
{

 WSACleanup ();
}
#endif

void
setnblock(int sock_descriptor)
{
#ifndef _WIN_
 int flags;
 // Set socket to non-blocking 

 if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
  {
   // Handle error
   //printf("Error fcntl nblock !!!!!!!\n");  
  }


 if (fcntl (sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
  {
   // Handle error 
   //printf("Error fcntl nblock !!!!!!!\n");  
  }
#else

 unsigned long iMode = 1;
 ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif 
}

/*
void
setblock(int sock_descriptor)
{
#ifndef _WIN_
int flags;
// Set socket to blocking 

if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
{
// Handle error 
//printf("Error fcntl block !!!!!!!\n");  
}


if (fcntl (sock_descriptor, F_SETFL, flags & (~O_NONBLOCK)) < 0)
{
// Handle error 
//printf("Error fcntl block !!!!!!!\n");  
}
#else
unsigned long iMode = 0;
ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif
}
 */

int
test_file_exist(const char * fname)
{
 struct stat sb;

 sb.st_mode = 0;

 stat (fname, &sb);

 return S_ISREG (sb.st_mode);
}
