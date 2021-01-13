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

#define dprint if (1) {} else printf

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
 int i, j;
 int n;
 int ret = 0;
 lxString stemp;
 board * Board;
 part * Part;
 input_t * Input;
 output_t * Output;

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

     if (cmd[cmdsize - 1] == '\r')
      {
       cmd[cmdsize - 1] = 0; //strip \r  
      }

     strcpy (cmd, (const char *) lowercase (cmd).c_str ());


     dprint ("cmd[%s]\n", cmd);

     switch (cmd[0])
      {
      case 'e':
       if (!strcmp (cmd, "exit"))
        {
         //Command exit ========================================================
         sendtext ("Ok\n");
         Window1.WDestroy ();
         return 0;
        }
       else
        {
         ret = sendtext ("ERROR\n>");
        }
       break;
      case 'g':
       if (!strncmp (cmd, "get ", 4))
        {
         //Command get==========================================================
         char * ptr;
         char * ptr2;
         Board = Window1.GetBoard ();

         if ((ptr = strstr (cmd, "board.in[")))
          {
           int in = (ptr[9] - '0')*10 + (ptr[10] - '0');

           if (in < Board->GetInputCount ())
            {
             Input = Board->GetInput (in);

             if (Input->status != NULL)
              {
               stemp.Printf ("board.in[%02i] %s = %i\n", in, Input->name, *((unsigned char *) Input->status));
               sendtext ((const char *) stemp.c_str ());
               sendtext ("Ok\n>");
              }
             else
              {
               ret = sendtext ("ERROR\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\n>");

            }
          }
         else if ((ptr = strstr (cmd, "board.out[")))
          {
           int out = (ptr[10] - '0')*10 + (ptr[11] - '0');

           if (out < Board->GetOutputCount ())
            {
             Output = Board->GetOutput (out);

             if (Output->status != NULL)
              {
               stemp.Printf ("board.out[%02i] %s = %i\n", out, Output->name, (int) *((float *) Output->status));
               sendtext ((const char *) stemp.c_str ());
               sendtext ("Ok\n>");
              }
             else
              {
               ret = sendtext ("ERROR\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\n>");

            }
          }
         else if (Board->GetUseSpareParts ())
          {
           if ((ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].in[")))
            {
             int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
             int in = (ptr2[5] - '0')*10 + (ptr2[5] - '0');

             if (pn < Window5.GetPartsCount ())
              {
               Part = Window5.GetPart (pn);
               if (in < Part->GetInputCount ())
                {
                 Input = Part->GetInput (in);

                 if (Input->status != NULL)
                  {
                   stemp.Printf ("part[%02i].in[%02i] %s = %i\n", pn, in, Input->name, *((unsigned char *) Input->status));
                   sendtext ((const char *) stemp.c_str ());
                   sendtext ("Ok\n>");
                  }
                 else
                  {
                   ret = sendtext ("ERROR\n>");
                  }
                }
               else
                {
                 ret = sendtext ("ERROR\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\n>");

              }
            }
           else if ((ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].out[")))
            {
             int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
             int out = (ptr2[6] - '0')*10 + (ptr2[7] - '0');

             if (pn < Window5.GetPartsCount ())
              {
               Part = Window5.GetPart (pn);
               if (out < Part->GetOutputCount ())
                {
                 Output = Part->GetOutput (out);

                 if (Output->status != NULL)
                  {
                   stemp.Printf ("part[%02i].out[%02i] %s = %i\n", pn, out, Output->name, (int) *((float *) Output->status));
                   sendtext ((const char *) stemp.c_str ());
                   sendtext ("Ok\n>");
                  }
                 else
                  {
                   ret = sendtext ("ERROR\n>");
                  }
                }
               else
                {
                 ret = sendtext ("ERROR\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\n>");
            }
          }
         else
          {
           ret = sendtext ("ERROR\n>");
          }
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
         //Command help ========================================================
         ret += sendtext ("List of supported commands:\n");
         ret += sendtext ("  exit      - shutdown PICSimLab\n");
         ret += sendtext ("  get ob    - get object value\n");
         ret += sendtext ("  help      - show this message\n");
         ret += sendtext ("  info      - show actual setup info and objects\n");
         ret += sendtext ("  quit      - exit remote control interface\n");
         ret += sendtext ("  reset     - reset the board\n");
         ret += sendtext ("  set ob vl - set object with value\n");
         ret += sendtext ("  version   - show PICSimLab version\n");

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
         //Command info ========================================================
         Board = Window1.GetBoard ();
         stemp.Printf ("Board:     %s\n", Board->GetName ().c_str ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Processor: %s\n", Board->GetProcessorName ().c_str ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Frequency: %f\n", Board->MGetFreq ());
         ret += sendtext ((const char *) stemp.c_str ());
         stemp.Printf ("Use Spare: %i\n", Board->GetUseSpareParts ());
         ret += sendtext ((const char *) stemp.c_str ());

         for (i = 0; i < Board->GetInputCount (); i++)
          {
           Input = Board->GetInput (i);
           //stemp.Printf ("  Input %i : %s\n", i, Input->name);
           //ret += sendtext ((const char *) stemp.c_str ());

           if (/*(Input->name[0] == 'P')&&(Input->name[1] == 'B')&&*/ (Input->status != NULL))
            {
             stemp.Printf ("    board.in[%02i] %s = %i\n", i, Input->name, *((unsigned char *) Input->status));
             ret += sendtext ((const char *) stemp.c_str ());
            }

          }

         for (i = 0; i < Board->GetOutputCount (); i++)
          {
           Output = Board->GetOutput (i);
           //stemp.Printf ("  Output %i: %s\n", i, Output->name);
           //ret += sendtext ((const char *) stemp.c_str ());

           if (/*(Output->name[0] == 'L')&&(Output->name[1] == 'D')&& */(Output->status != NULL))
            {
             stemp.Printf ("    board.out[%02i] %s = %3.0f\n", i, Output->name, *((float *) Output->status));
             ret += sendtext ((const char *) stemp.c_str ());
            }
          }

         if (Board->GetUseSpareParts ())
          {
           for (i = 0; i < Window5.GetPartsCount (); i++)
            {
             Part = Window5.GetPart (i);
             stemp.Printf ("  part[%02i]: %s\n", i, (const char *) Part->GetName ());
             ret += sendtext ((const char *) stemp.c_str ());

             for (j = 0; j < Part->GetInputCount (); j++)
              {
               Input = Part->GetInput (j);
               //stemp.Printf ("  Input %i : %s\n", i, Input->name);
               //ret += sendtext ((const char *) stemp.c_str ());

               if (/*(Input->name[0] == 'P')&&(Input->name[1] == 'B')&&*/ (Input->status != NULL))
                {
                 stemp.Printf ("    part[%02i].in[%02i] %s = %i\n", i, j, Input->name, *((unsigned char *) Input->status));
                 ret += sendtext ((const char *) stemp.c_str ());
                }
              }
             for (j = 0; j < Part->GetOutputCount (); j++)
              {
               Output = Part->GetOutput (j);
               //stemp.Printf ("  Input %i : %s\n", i, Input->name);
               //ret += sendtext ((const char *) stemp.c_str ());

               if (/*(Input->name[0] == 'P')&&(Input->name[1] == 'B')&&*/ (Output->status != NULL))
                {
                 stemp.Printf ("    part[%02i].out[%02i] %s = %i\n", i, j, Output->name, (int) *((float *) Output->status));
                 ret += sendtext ((const char *) stemp.c_str ());
                }
              }
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
         //Command quit ========================================================
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
         //Command reset =======================================================
         Window1.GetBoard ()->MReset (0);
         ret = sendtext ("Ok\n>");
        }
       else
        {
         ret = sendtext ("ERROR\n>");
        }
       break;
      case 's':
       if (!strncmp (cmd, "set ", 4))
        {
         //Command set =========================================================
         char * ptr;
         char * ptr2;
         Board = Window1.GetBoard ();

         if ((ptr = strstr (cmd, "board.in[")))
          {
           int in = (ptr[9] - '0')*10 + (ptr[10] - '0');
           int value;

           sscanf (ptr + 12, "%i", &value);

           printf ("board.in[%02i] = %i \n", in, value);

           if (in < Board->GetInputCount ())
            {
             Input = Board->GetInput (in);

             if (Input->status != NULL)
              {
               *((unsigned char *) Input->status) = value;
               sendtext ("Ok\n>");
              }
             else
              {
               ret = sendtext ("ERROR\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\n>");

            }
          }
         else if (Board->GetUseSpareParts ()&&(ptr = strstr (cmd, "part["))&&(ptr2 = strstr (cmd, "].in[")))
          {
           int pn = (ptr[5] - '0')*10 + (ptr[6] - '0');
           int in = (ptr2[5] - '0')*10 + (ptr2[6] - '0');
           int value;

           sscanf (ptr2 + 8, "%i", &value);

           printf ("part[%02i].in[%02i] = %i \n", pn, in, value);

           if (pn < Window5.GetPartsCount ())
            {
             Part = Window5.GetPart (pn);

             if (in < Part->GetInputCount ())
              {
               Input = Part->GetInput (in);

               if (Input->status != NULL)
                {
                 *((unsigned char *) Input->status) = value;
                 sendtext ("Ok\n>");
                }
               else
                {
                 ret = sendtext ("ERROR\n>");
                }
              }
             else
              {
               ret = sendtext ("ERROR\n>");
              }
            }
           else
            {
             ret = sendtext ("ERROR\n>");

            }
          }
         else
          {
           ret = sendtext ("ERROR\n>");
          }
         return 0;
        }
       else
        {
         ret = sendtext ("ERROR\n>");
        }
       break;
      case 'v':
       if (!strcmp (cmd, "version"))
        {
         //Command version =====================================================
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
       //Uknown command ========================================================
       ret = sendtext ("ERROR\n>");
       break;
      }
    }
   else
    {
     bp += n;
     if (bp > BSIZE)bp = BSIZE;
    }
  }

 //close connection
 if (ret)rcontrol_stop ();

 return ret;
}

