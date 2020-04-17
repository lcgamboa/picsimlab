/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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


#include "../picsimlab1.h"
#include "mplabxd.h"

#define dprint if (0) {} else printf


#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
//system headers dependent
#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define        min(a,b)        ((a) < (b) ? (a) : (b))
#include <sys/unistd.h>
#include <fcntl.h>
#else
#include<winsock.h>
WORD wVersionRequested = 2;
WSADATA wsaData;
#ifndef MSG_WAITALL
#define MSG_WAITALL (1 << 3)
#endif
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//defines
#define        TCP_SERV_PORT        1234

typedef struct sockaddr sockaddr;



#define STARTD 0xF0
#define STOPD  0xFF


#define STEP   0x01
#define RESET  0x05
#define RUN    0x10
#define HALT   0x15
#define GETPC  0x20
#define SETPC  0x25
#define SETBK  0x30
#define STRUN  0x31
#define GETID  0x35
#define GETNAM 0x36
#define PROGD  0x40
#define PROGP  0x45
#define PROGC  0x50
#define PROGI  0x55
#define PROGE  0x57
#define READD  0x60
#define READDV 0x61
#define READP  0x65
#define READC  0x70
#define READI  0x75
#define READE  0x80



int sockfd = -1;
int listenfd = -1;

void
setnblock(int sock_descriptor)
{
#ifndef _WIN_
 int flags;
 /* Set socket to non-blocking */

 if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl nblock !!!!!!!\n");  
  }


 if (fcntl (sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
  {
   /* Handle error */
   //printf("Error fcntl nblock !!!!!!!\n");  
  }
#else
 unsigned long iMode = 1;
 ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif 
}

void
setblock(int sock_descriptor)
{
#ifndef _WIN_
 int flags;
 /* Set socket to blocking */

 if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl block !!!!!!!\n");  
  }


 if (fcntl (sock_descriptor, F_SETFL, flags & (~O_NONBLOCK)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl block !!!!!!!\n");  
  }
#else
 unsigned long iMode = 0;
 ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif
}

int server_started = 0;

board * dbg_board = NULL;

int
mplabxd_init(board * mboard)
{
 struct sockaddr_in serv;

 dbg_board = mboard;

 if (!server_started)
  {
   dprint ("mplabxd_init\n");
#ifdef _WIN_
   WSAStartup (wVersionRequested, &wsaData);
   if (wsaData.wVersion != wVersionRequested)
    {
     fprintf (stderr, "\n Wrong version\n");
     return 1;
    }

#endif

   if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("socket error : %s \n", strerror (errno));
     return 1;
    };

   int reuse = 1;
   if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
    perror ("setsockopt(SO_REUSEADDR) failed");

   memset (&serv, 0, sizeof (serv));
   serv.sin_family = AF_INET;
   serv.sin_addr.s_addr = htonl (INADDR_ANY);
   serv.sin_port = htons (TCP_SERV_PORT);

   if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)) < 0)
    {
     printf ("bind error : %s \n", strerror (errno));
     return 1;
    };

   if (listen (listenfd, SOMAXCONN) < 0)
    {
     printf ("listen error : %s \n", strerror (errno));
     return 1;
    };
   server_started = 1;
  }
 return 0;
}

int
mplabxd_start(void)
{
 struct sockaddr_in cli;
#ifndef _WIN_
 unsigned int clilen;
#else
 int clilen;
#endif
 clilen = sizeof (cli);

 setnblock (listenfd);

 if (
     (sockfd =
      accept (listenfd, (sockaddr *) & cli, & clilen)) < 0)
  {
   return 1;
  }


 setnblock (sockfd);
 dprint ("Debug connected!---------------------------------\n");
 return 0;
}

void
mplabxd_stop(void)
{
 dprint ("Debug disconnected!---------------------------------\n");
 if (sockfd >= 0)
  {
   shutdown (sockfd, SHUT_RDWR);
   close (sockfd);
  }
 sockfd = -1;
}

void
mplabxd_end(void)
{
 if (server_started)
  {
   mplabxd_stop ();
   dprint ("mplabxd_end\n");
   shutdown (listenfd, SHUT_RDWR);
   close (listenfd);
  }
 listenfd = -1;
 server_started = 0;
 dbg_board = NULL;
#ifdef _WIN_
 WSACleanup ();
#endif
};


int bpc = 0;
unsigned short bp[100];

int
mplabxd_testbp(void)
{
 int i;
 if (!Window1.Get_mcudbg ())
  for (i = 0; i < bpc; i++)
   {
    if (dbg_board->MTestBP (bp[i]))
     {
      dprint ("breakpoint 0x%04X!!!!!=========================\n", bp[i]);
      Window1.SetCpuState (CPU_BREAKPOINT);
      Window1.Set_mcudbg (1);
      return Window1.Get_mcudbg ();
     }
   }

 return Window1.Get_mcudbg ();
}

int
mplabxd_loop(void)
{
 unsigned short pc;
 int i;

 int n;
 int ret = 0;


 unsigned char cmd, reply;

 //open connection  
 if (sockfd < 0)
  if (mplabxd_start ())return 1;

 if ((n = recv (sockfd, (char *) &cmd, 1, 0)) < 0)
  {
   //printf ("receive error : %s \n", strerror (errno));
   //exit (1);
  };


 if (n == 1)
  {
   setblock (sockfd);


   dprint ("cmd %02X  ", cmd);
   //fflush(stdout);  

   reply = 0x00;

   switch (cmd)
    {
    case STARTD:
     dprint ("STARTD cmd ----------------------\n");
     Window1.Set_mcudbg (1);
     break;
    case STOPD:
     dprint ("STOPD cmd -----------------------\n");
     ret = 1;
     Window1.Set_mcudbg (0);
     Window1.SetCpuState (CPU_RUNNING);
     bpc = 0;
     Window1.SetJUMPSTEPS (DEFAULTJS);
     break;
    case STEP:
     dprint ("STEP cmd\n");
     dbg_board->MStep ();
     Window1.SetCpuState (CPU_STEPPING);
     break;
    case RESET:
     Window1.Set_mcudbg (1);
     dprint ("RESET cmd\n");
     dbg_board->MStepResume ();
     dbg_board->MReset (1);
     dbg_board->MStep ();
     break;
    case RUN:
     Window1.Set_mcudbg (0);
     Window1.SetJUMPSTEPS (DEFAULTJS);
     dprint ("RUN cmd\n");
     dbg_board->MStep (); //to go out break point
     Window1.SetCpuState (CPU_RUNNING);
     break;
    case HALT:
     Window1.Set_mcudbg (1);
     dbg_board->MStepResume ();
     Window1.SetJUMPSTEPS (1);
     dprint ("HALT cmd\n");
     Window1.SetCpuState (CPU_HALTED);
     break;
    case GETPC:
     pc=dbg_board->MGetPC();
     dprint ("GETPC %04Xcmd\n", pc);
     if (send (sockfd, (char *) &pc, 2, 0) != 2)
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     break;
    case SETPC:
     if ((n = recv (sockfd, (char *) &pc, 2, MSG_WAITALL)) != 2)
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
      dbg_board->MSetPC (pc);
     dprint ("SETPC cmd\n");
     break;
    case SETBK:
     if ((n = recv (sockfd, (char *) &bpc, 2, MSG_WAITALL)) != 2)
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("bp count =%i\n", bpc);
     if (bpc >= 100)bpc = 100;
     if (bpc > 0)
      {
       if ((n = recv (sockfd, (char *) &bp, bpc * 2, MSG_WAITALL)) != bpc * 2)
        {
         printf ("receive error : %s \n", strerror (errno));
         ret = 1;
         reply = 0x01;
        };
#ifdef _DEBUG_
       for (i = 0; i < bpc; i++)
        printf ("bp %i = %#06X\n", i, bp[i]);
#endif
      }
     dprint ("SETBK cmd\n");
     break;
    case STRUN:
     i = Window1.Get_mcudbg ();
     if (send (sockfd, (char *) &i, 1, 0) != 1)
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("STRUN cmd =%i\n", Window1.Get_mcudbg ());
     break;
    case GETID:
     dprint ("GETID cmd\n");
     if (send (sockfd, (char *) dbg_board->MGetProcID_p (), 2, 0) != 2)
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     break;
    case GETNAM:
     dprint ("GETNAM cmd\n");
     char buff[20];
     buff[0] = dbg_board->proc.length ();
     strncpy (buff + 1, (const char *) dbg_board->proc.c_str (), 18);
     if (send (sockfd, buff, 20, 0) != 20)
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     break;
    case PROGD:
     if ((n = recv (sockfd, (char *)dbg_board->MGetRAM_p (), dbg_board->MGetRAMSize (), MSG_WAITALL)) != (int) dbg_board->MGetRAMSize ())
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("PROGD cmd\n");
     break;
    case PROGP:
     if ((n = recv (sockfd, (char *)dbg_board->MGetROM_p (), dbg_board->MGetROMSize (), MSG_WAITALL)) != (int) dbg_board->MGetROMSize ())
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
#ifdef _DEBUG_
     for (i = 0; i < (int) dbg_board->MGetROMSize (); i++)printf ("%#02X ", dbg_board->MGetROM_p ()[i]);
#endif
     dprint ("PROGP cmd  %i of %i\n", n, dbg_board->MGetROMSize ());
     break;
    case PROGC:
     if ((n = recv (sockfd, (char *)dbg_board->MGetCONFIG_p (), dbg_board->MGetCONFIGSize (), MSG_WAITALL)) != (int)dbg_board->MGetCONFIGSize ())
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
#ifdef _DEBUG_
     for (i = 0; i < (int) dbg_board->MGetCONFIGSize (); i++)printf ("%#02X ", dbg_board->MGetCONFIG_p ()[i]);
#endif
     dprint ("PROGC cmd  %i of %i\n", n, dbg_board->MGetCONFIGSize ());
     break;
    case PROGI:
     if ((n = recv (sockfd, (char *)dbg_board->MGetID_p (), dbg_board->MGetIDSize (), MSG_WAITALL)) != (int) dbg_board->MGetIDSize ())
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
#ifdef _DEBUG_
     for (i = 0; i < (int) dbg_board->MGetIDSize (); i++)printf ("%#02X ", dbg_board->MGetID_p ()[i]);
#endif
     dprint ("PROGI cmd\n");
     break;
    case PROGE:
     if ((n = recv (sockfd, (char *)dbg_board->MGetEEPROM_p (), dbg_board->MGetEEPROM_Size (), MSG_WAITALL)) != (int) dbg_board->MGetEEPROM_Size ())
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      }
#ifdef _DEBUG_
     for (i = 0; i < (int) dbg_board->MGetEEPROM_Size (); i++)printf ("%#02X ", dbg_board->MGetEEPROM_p ()[i]);
#endif
     dprint ("PROGE cmd\n");
     break;
    case READD:
     if (send (sockfd,(char *) dbg_board->MGetRAM_p (), dbg_board->MGetRAMSize (), 0) != (int) dbg_board->MGetRAMSize ())
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      }
     dprint ("READD cmd  size=0x%04X, ret= %i\n",dbg_board->MGetRAMSize(),ret);
     break;
    case READDV:
     if ((n = recv (sockfd, (char *) &bp, 4, MSG_WAITALL)) != 4)
      {
       printf ("receive error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("address=%02X  values=%i \n", bp[0], bp[1]);
     if (send (sockfd, (char *) &dbg_board->MGetRAM_p()[bp[0]], bp[1], 0) != bp[1])
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("READDV cmd\n");
     break;
    case READP:
     if (send (sockfd, (const char *)dbg_board->MGetROM_p (), dbg_board->MGetROMSize (), 0) != (int) dbg_board->MGetROMSize ())
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("READP cmd\n");
     break;
    case READC:
     if (send (sockfd, (const char *)dbg_board->MGetCONFIG_p (), dbg_board->MGetCONFIGSize (), 0) != (int) dbg_board->MGetCONFIGSize ())
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("READC cmd\n");
     break;
    case READI:
     if (send (sockfd, (char *)dbg_board->MGetID_p (), dbg_board->MGetIDSize (), 0) != (int) dbg_board->MGetIDSize ())
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("READI cmd\n");
     break;
    case READE:
     if (send (sockfd,(char *) dbg_board->MGetEEPROM_p (), dbg_board->MGetEEPROM_Size (), 0) != (int) dbg_board->MGetEEPROM_Size ())
      {
       printf ("send error : %s \n", strerror (errno));
       ret = 1;
       reply = 0x01;
      };
     dprint ("READI cmd\n");
     break;
    default:
     dprint ("UNKNOWN cmd !!!!!!!!!!!!!\n");
     break;
    }


   if (send (sockfd, (char *) &reply, 1, 0) != 1)
    {
     printf ("send error : %s \n", strerror (errno));
     ret = 1;
    };

   setnblock (sockfd);
  }

 //close connection
 if (ret)mplabxd_stop ();

 return ret;
}

