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
#include "../picsimlab5.h"
#include "rcontrol.h"

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
#include <sys/unistd.h>
#include <fcntl.h>
#else
#include<winsock.h>
#ifndef MSG_WAITALL
#define MSG_WAITALL (1 << 3)
#define MSG_NOSIGNAL 0
#endif
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <lxrad_NOGUI/mstring.h>

void setnblock(int sock_descriptor);

void setblock(int sock_descriptor);

//typedef struct sockaddr sockaddr;


static int sockfd = -1;
static int listenfd = -1;
static int server_started = 0;

#define BSIZE 1024
static char buffer[BSIZE];
static int bp = 0;

int
rcontrol_init(unsigned short tcpport)
{
 struct sockaddr_in serv;

 if (!server_started)
  {
   dprint ("rcontrol: init\n");

   if ((listenfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("rcontrol: socket error : %s \n", strerror (errno));
     return 1;
    };

   int reuse = 1;
   if (setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse)) < 0)
    perror ("rcontrol: setsockopt(SO_REUSEADDR) failed");

   memset (&serv, 0, sizeof (serv));
   serv.sin_family = AF_INET;
   serv.sin_addr.s_addr = htonl (INADDR_ANY);
   serv.sin_port = htons (tcpport);

   if (bind (listenfd, (sockaddr *) & serv, sizeof (serv)) < 0)
    {
     printf ("rcontrol: bind error : %s \n", strerror (errno));
     return 1;
    }

   if (listen (listenfd, SOMAXCONN) < 0)
    {
     printf ("rcontrol: listen error : %s \n", strerror (errno));
     return 1;
    }
   server_started = 1;

  }
 return 0;
}

static int
sendtext(const char * str)
{
 int size = strlen (str);

 if (send (sockfd, str, size, MSG_NOSIGNAL) != size)
  {
   printf ("rcontrol: send error : %s \n", strerror (errno));
   return 1;
  }

 return 0;
}

int
rcontrol_start(void)
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
 dprint ("rcontrol: Client connected!---------------------------------\n");

 memset (buffer, 0, BSIZE);
 bp = 0;

 return sendtext ("\nPICSimLab Remote Control Interface\n\n  Type help to see supported commands\n\n>");

}

void
rcontrol_stop(void)
{
 dprint ("rcontrol: Client disconnected!---------------------------------\n");
 if (sockfd >= 0)
  {
   shutdown (sockfd, SHUT_RDWR);
   close (sockfd);
  }
 sockfd = -1;
}

void
rcontrol_end(void)
{

 if (server_started)
  {
   rcontrol_stop ();
   dprint ("rcontrol: end\n");
   shutdown (listenfd, SHUT_RDWR);
   close (listenfd);
  }
 listenfd = -1;
 server_started = 0;
}

int
rcontrol_loop(void)
{
 int n;
 int ret = 0;
 lxString stemp;
 board * Board;
 part * Part;

 //open connection  
 if (sockfd < 0)
  {
   if (rcontrol_start ())return 1;
  }

 n = recv (sockfd, (char *) &buffer[bp], 1024 - bp, 0);


 if (n > 0)
  {

   if (strchr (buffer, '\n'))
    {
     char cmd[100];
     int cmdsize = 0;

     while (buffer[cmdsize] != '\n')
      {
       cmd[cmdsize] = buffer[cmdsize];
       cmdsize++;
      }
     cmd[cmdsize] = 0;

     for (int i = 0; i < BSIZE - cmdsize; i++)
      {
       buffer[i] = buffer[i + cmdsize];
      }

     cmd[cmdsize - 1] = 0; //strip \n
     if (cmd[cmdsize - 2] == '\r')
      {
       cmd[cmdsize - 2] = 0; //strip \r  
      }

     strcpy (cmd, (const char *) lowercase (cmd).c_str ());


     dprint ("cmd[%s]\n", cmd);

     switch (cmd[0])
      {
      case 'e':
       if (!strcmp (cmd, "exit"))
        {
         sendtext ("Ok\n");
         Window1.WDestroy ();
         return 0;
        }
       else
        {
         ret = sendtext ("ERROR\n>");
        }
       break;
      case 'h':
       if (!strcmp (cmd, "help"))
        {
         ret += sendtext ("List of supported commands:\n");
         ret += sendtext ("  exit    - shutdown PICSimLab\n");
         ret += sendtext ("  help    - show this message\n");
         ret += sendtext ("  info    - show actual setup info\n");
         ret += sendtext ("  quit    - exit remote control interface\n");
         ret += sendtext ("  reset   - reset the board\n");
         ret += sendtext ("  version - show PICSimLab version\n");

         ret += sendtext ("Ok\n>");
        }
       else
        {
         ret = sendtext ("ERROR\n>");
        }
       break;
      case 'i':
       if (!strcmp (cmd, "info"))
        {
         Board = Window1.GetBoard ();
         stemp.Printf ("Board:     %s\n", Board->GetName ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Processor: %s\n", Board->GetProcessorName ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Frequency: %f\n", Board->MGetFreq ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Use Spare: %i\n", Board->GetUseSpareParts ());
         ret += sendtext ((const char *) stemp.c_str ());

         if (Board->GetUseSpareParts ())
           {
            for (int i = 0; i < Window5.GetPartsCount (); i++)
             {
              Part = Window5.GetPart (i);

              stemp.Printf ("  Part %i: %s\n", i, (const char *)Part->GetName ());
              ret += sendtext ((const char *) stemp.c_str ());
             }
           }
             ret += sendtext ("Ok\n>");
        }
       else
        {
             ret = sendtext ("ERROR\n>");
        }
             break;
             case 'q':
             if (!strcmp (cmd, "quit"))
        {
             sendtext ("Ok\n");
             ret = 1;
        }
       else
        {
             ret = sendtext ("ERROR\n>");
        }
             break;
             case 'r':
             if (!strcmp (cmd, "reset"))
        {
             Window1.GetBoard ()->MReset (0);
             ret = sendtext ("Ok\n>");
        }
       else
        {
             ret = sendtext ("ERROR\n>");
        }
             break;
             case 'v':
             if (!strcmp (cmd, "version"))
        {
             stemp.Printf (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s\n"), lxT (_VERSION_), lxT (_DATE_), lxT (_ARCH_));
             ret += sendtext ((const char *) stemp.c_str ());
             ret += sendtext ("Ok\n>");
        }
       else
        {
             ret = sendtext ("ERROR\n>");
        }
             break;
             default:
             ret = sendtext ("ERROR\n>");
             break;
      }

    }
   else
    {
             bp += n;
             if (bp > BSIZE)bp = BSIZE;
     }




             /*
        if ((n = recv (sockfd, (char *) ramreceived, dbg_board->DBGGetRAMSize (), MSG_WAITALL)) != (int) dbg_board->DBGGetRAMSize ())
         {
          printf ("receive error : %s \n", strerror (errno));
          ret = 1;
          reply = 0x01;
         }
  
      if (send (sockfd, (char *) ramsend, dbg_board->DBGGetRAMSize (), MSG_NOSIGNAL) != (int) dbg_board->DBGGetRAMSize ())
         {
          printf ("send error : %s \n", strerror (errno));
          ret = 1;
          reply = 0x01;
         }
    */
             /*
      if (send (sockfd, (char *) &reply, 1, MSG_NOSIGNAL) != 1)
       {
        printf ("send error : %s \n", strerror (errno));
        ret = 1;
       }
    */
  }

             //close connection
             if (ret)rcontrol_stop ();

             return ret;
 }

